#include <iostream>

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http_parser.h"

#undef TRUE
#define TRUE 1
#undef FALSE
#define FALSE 0

#define MAX_HEADERS 13
#define MAX_ELEMENT_SIZE 2048
#define MAX_CHUNKS 16

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static http_parser* parser;

struct http_message
{
  const char* name; // for debugging purposes
  const char* raw;
  enum http_parser_type type;
  unsigned int method;
  int status_code;
  char response_status[MAX_ELEMENT_SIZE];
  char request_path[MAX_ELEMENT_SIZE];
  char request_url[MAX_ELEMENT_SIZE];
  char fragment[MAX_ELEMENT_SIZE];
  char query_string[MAX_ELEMENT_SIZE];
  char body[MAX_ELEMENT_SIZE];
  size_t body_size;
  const char* host;
  const char* userinfo;
  uint16_t port;
  int num_headers;
  enum
  {
    NONE = 0,
    FIELD,
    VALUE
  } last_header_element;
  char headers[MAX_HEADERS][2][MAX_ELEMENT_SIZE];
  int should_keep_alive;

  int num_chunks;
  int num_chunks_complete;
  int chunk_lengths[MAX_CHUNKS];

  const char* upgrade; // upgraded body

  unsigned short http_major;
  unsigned short http_minor;

  int message_begin_cb_called;
  int headers_complete_cb_called;
  int message_complete_cb_called;
  int message_complete_on_eof;
  int body_is_final;
};

static int currently_parsing_eof;

static struct http_message messages[5];
static int num_messages;
static http_parser_settings* current_pause_parser;

size_t
strnlen(const char* s, size_t maxlen)
{
  const char* p;

  p = (const char*)memchr(s, '\0', maxlen);
  if (p == NULL)
    return maxlen;

  return p - s;
}

size_t
strlncat(char* dst, size_t len, const char* src, size_t n)
{
  size_t slen;
  size_t dlen;
  size_t rlen;
  size_t ncpy;

  slen = strnlen(src, n);
  dlen = strnlen(dst, len);

  if (dlen < len) {
    rlen = len - dlen;
    ncpy = slen < rlen ? slen : (rlen - 1);
    memcpy(dst + dlen, src, ncpy);
    dst[dlen + ncpy] = '\0';
  }

  assert(len > slen + dlen);
  return slen + dlen;
}

size_t
strlcat(char* dst, const char* src, size_t len)
{
  return strlncat(dst, len, src, (size_t)-1);
}

size_t
strlncpy(char* dst, size_t len, const char* src, size_t n)
{
  size_t slen;
  size_t ncpy;

  slen = strnlen(src, n);

  if (len > 0) {
    ncpy = slen < len ? slen : (len - 1);
    memcpy(dst, src, ncpy);
    dst[ncpy] = '\0';
  }

  assert(len > slen);
  return slen;
}

size_t
strlcpy(char* dst, const char* src, size_t len)
{
  return strlncpy(dst, len, src, (size_t)-1);
}

int
request_url_cb(http_parser* p, const char* buf, size_t len)
{
  assert(p == parser);
  strlncat(messages[num_messages].request_url,
           sizeof(messages[num_messages].request_url),
           buf,
           len);
  return 0;
}

int
header_field_cb(http_parser* p, const char* buf, size_t len)
{
  assert(p == parser);
  struct http_message* m = &messages[num_messages];

  if (m->last_header_element != http_message::FIELD)
    m->num_headers++;

  strlncat(m->headers[m->num_headers - 1][0],
           sizeof(m->headers[m->num_headers - 1][0]),
           buf,
           len);

  m->last_header_element = http_message::FIELD;

  return 0;
}

int
header_value_cb(http_parser* p, const char* buf, size_t len)
{
  assert(p == parser);
  struct http_message* m = &messages[num_messages];

  strlncat(m->headers[m->num_headers - 1][1],
           sizeof(m->headers[m->num_headers - 1][1]),
           buf,
           len);

  m->last_header_element = http_message::VALUE;

  return 0;
}

void
check_body_is_final(const http_parser* p)
{
  if (messages[num_messages].body_is_final) {
    fprintf(stderr,
            "\n\n *** Error http_body_is_final() should return 1 "
            "on last on_body callback call "
            "but it doesn't! ***\n\n");
    assert(0);
    abort();
  }
  messages[num_messages].body_is_final = http_body_is_final(p);
}

int
body_cb(http_parser* p, const char* buf, size_t len)
{
  assert(p == parser);
  strlncat(
    messages[num_messages].body, sizeof(messages[num_messages].body), buf, len);
  messages[num_messages].body_size += len;
  check_body_is_final(p);
  // printf("body_cb: '%s'\n", requests[num_messages].body);
  return 0;
}

int
count_body_cb(http_parser* p, const char* buf, size_t len)
{
  assert(p == parser);
  assert(buf);
  messages[num_messages].body_size += len;
  check_body_is_final(p);
  return 0;
}

int
message_begin_cb(http_parser* p)
{
  assert(p == parser);
  messages[num_messages].message_begin_cb_called = TRUE;
  return 0;
}

int
headers_complete_cb(http_parser* p)
{
  assert(p == parser);
  messages[num_messages].method = parser->method;
  messages[num_messages].status_code = parser->status_code;
  messages[num_messages].http_major = parser->http_major;
  messages[num_messages].http_minor = parser->http_minor;
  messages[num_messages].headers_complete_cb_called = TRUE;
  messages[num_messages].should_keep_alive = http_should_keep_alive(parser);
  return 0;
}

int
message_complete_cb(http_parser* p)
{
  assert(p == parser);
  if (messages[num_messages].should_keep_alive !=
      http_should_keep_alive(parser)) {
    fprintf(stderr,
            "\n\n *** Error http_should_keep_alive() should have same "
            "value in both on_message_complete and on_headers_complete "
            "but it doesn't! ***\n\n");
    assert(0);
    abort();
  }

  if (messages[num_messages].body_size && http_body_is_final(p) &&
      !messages[num_messages].body_is_final) {
    fprintf(stderr,
            "\n\n *** Error http_body_is_final() should return 1 "
            "on last on_body callback call "
            "but it doesn't! ***\n\n");
    assert(0);
    abort();
  }

  messages[num_messages].message_complete_cb_called = TRUE;

  messages[num_messages].message_complete_on_eof = currently_parsing_eof;

  num_messages++;
  return 0;
}

int
response_status_cb(http_parser* p, const char* buf, size_t len)
{
  assert(p == parser);
  strlncat(messages[num_messages].response_status,
           sizeof(messages[num_messages].response_status),
           buf,
           len);
  return 0;
}

int
chunk_header_cb(http_parser* p)
{
  assert(p == parser);
  int chunk_idx = messages[num_messages].num_chunks;
  messages[num_messages].num_chunks++;
  if (chunk_idx < MAX_CHUNKS) {
    messages[num_messages].chunk_lengths[chunk_idx] = p->content_length;
  }

  return 0;
}

int
chunk_complete_cb(http_parser* p)
{
  assert(p == parser);

  /* Here we want to verify that each chunk_header_cb is matched by a
   * chunk_complete_cb, so not only should the total number of calls to
   * both callbacks be the same, but they also should be interleaved
   * properly */
  assert(messages[num_messages].num_chunks ==
         messages[num_messages].num_chunks_complete + 1);

  messages[num_messages].num_chunks_complete++;
  return 0;
}

static http_parser_settings settings = {
  /*.on_message_begin = */ message_begin_cb,
  /*.on_url = */ request_url_cb,
  /*.on_status = */ response_status_cb,
  /*.on_header_field = */ header_field_cb,
  /*.on_header_value = */ header_value_cb,
  /*.on_headers_complete = */ headers_complete_cb,
  /*.on_body = */ body_cb,
  /*.on_message_complete = */ message_complete_cb,
  /*.on_chunk_header = */ chunk_header_cb,
  /*.on_chunk_complete = */ chunk_complete_cb
};

static http_parser_settings settings_count_body = {
  /*.on_message_begin = */ message_begin_cb,
  /*.on_url = */ request_url_cb,
  /*.on_status = */ response_status_cb,
  /*.on_header_field = */ header_field_cb,
  /*.on_header_value = */ header_value_cb,
  /*.on_headers_complete = */ headers_complete_cb,
  /*.on_body = */ count_body_cb,
  /*.on_message_complete = */ message_complete_cb,
  /*.on_chunk_header = */ chunk_header_cb,
  /*.on_chunk_complete = */ chunk_complete_cb
};

static http_parser_settings settings_null = {
  /*.on_message_begin =*/0, /*.on_url =*/0,
  /*.on_status =*/0,        /*.on_header_field =*/0,
  /*.on_header_value =*/0,  /*.on_headers_complete =*/0,
  /*.on_body =*/0,          /*.on_message_complete =*/0,
  /*.on_chunk_header =*/0,  /*.on_chunk_complete =*/0
};

void
parser_init(enum http_parser_type type)
{
  num_messages = 0;

  assert(parser == NULL);

  parser = (http_parser*)malloc(sizeof(http_parser));

  http_parser_init(parser, type);

  memset(&messages, 0, sizeof messages);
}

void
parser_free()
{
  assert(parser);
  free(parser);
  parser = NULL;
}
const char*
str_http_type(unsigned int type)
{
  switch (type) {
    case HTTP_REQUEST:
      return "request";
    case HTTP_RESPONSE:
      return "response";
    case HTTP_BOTH:
      return "request_response";
  }
}
const char*
str_http_flags(unsigned int flags)
{
  switch (flags) {
    case 0:
      return "no flags";
    case F_CHUNKED:
      return "chunked";
    case F_CONNECTION_KEEP_ALIVE:
      return "connection keep alive";
    case F_CONNECTION_CLOSE:
      return "connection close";
    case F_CONNECTION_UPGRADE:
      return "connection upgrade";
    case F_TRAILING:
      return "trailing";
    case F_UPGRADE:
      return "upgrade";
    case F_SKIPBODY:
      return "skipbody";
    default:
      return "<unknown>";
  }
}
#define MAKE_CASE_STR(index, vlr)                                              \
  case index:                                                                  \
    return #vlr;

const char*
str_http_state(unsigned int state)
{
  switch (state) {
    MAKE_CASE_STR(1, s_dead)
    MAKE_CASE_STR(2, s_start_req_or_res)
    MAKE_CASE_STR(3, s_res_or_resp_H)
    MAKE_CASE_STR(4, s_start_res)
    MAKE_CASE_STR(5, s_res_H)
    MAKE_CASE_STR(6, s_res_HT)
    MAKE_CASE_STR(7, s_res_HTT)
    MAKE_CASE_STR(8, s_res_HTTP)
    MAKE_CASE_STR(9, s_res_first_http_major)
    MAKE_CASE_STR(10, s_res_http_major)
    MAKE_CASE_STR(11, s_res_first_http_minor)
    MAKE_CASE_STR(12, s_res_http_minor)
    MAKE_CASE_STR(13, s_res_first_status_code)
    MAKE_CASE_STR(14, s_res_status_code)
    MAKE_CASE_STR(15, s_res_status_start)
    MAKE_CASE_STR(16, s_res_status)
    MAKE_CASE_STR(17, s_res_line_almost_done)
    MAKE_CASE_STR(18, s_start_req)
    MAKE_CASE_STR(19, s_req_method)
    MAKE_CASE_STR(20, s_req_spaces_before_url)
    MAKE_CASE_STR(21, s_req_schema)
    MAKE_CASE_STR(22, s_req_schema_slash)
    MAKE_CASE_STR(23, s_req_schema_slash_slash)
    MAKE_CASE_STR(24, s_req_server_start)
    MAKE_CASE_STR(25, s_req_server)
    MAKE_CASE_STR(26, s_req_server_with_at)
    MAKE_CASE_STR(27, s_req_path)
    MAKE_CASE_STR(28, s_req_query_string_start)
    MAKE_CASE_STR(29, s_req_query_string)
    MAKE_CASE_STR(30, s_req_fragment_start)
    MAKE_CASE_STR(31, s_req_fragment)
    MAKE_CASE_STR(32, s_req_http_start)
    MAKE_CASE_STR(33, s_req_http_H)
    MAKE_CASE_STR(34, s_req_http_HT)
    MAKE_CASE_STR(35, s_req_http_HTT)
    MAKE_CASE_STR(36, s_req_http_HTTP)
    MAKE_CASE_STR(37, s_req_first_http_major)
    MAKE_CASE_STR(38, s_req_http_major)
    MAKE_CASE_STR(39, s_req_first_http_minor)
    MAKE_CASE_STR(40, s_req_http_minor)
    MAKE_CASE_STR(41, s_req_line_almost_done)
    MAKE_CASE_STR(42, s_header_field_start)
    MAKE_CASE_STR(43, s_header_field)
    MAKE_CASE_STR(44, s_header_value_discard_ws)
    MAKE_CASE_STR(45, s_header_value_discard_ws_almost_done)
    MAKE_CASE_STR(46, s_header_value_discard_lws)
    MAKE_CASE_STR(47, s_header_value_start)
    MAKE_CASE_STR(48, s_header_value)
    MAKE_CASE_STR(49, s_header_value_lws)
    MAKE_CASE_STR(50, s_header_almost_done)
    MAKE_CASE_STR(51, s_chunk_size_start)
    MAKE_CASE_STR(52, s_chunk_size)
    MAKE_CASE_STR(53, s_chunk_parameters)
    MAKE_CASE_STR(54, s_chunk_size_almost_done)
    MAKE_CASE_STR(55, s_headers_almost_done)
    MAKE_CASE_STR(56, s_headers_done)
    MAKE_CASE_STR(57, s_chunk_data)
    MAKE_CASE_STR(58, s_chunk_data_almost_done)
    MAKE_CASE_STR(59, s_chunk_data_done)
    MAKE_CASE_STR(60, s_body_identity)
    MAKE_CASE_STR(61, s_body_identity_eof)
    MAKE_CASE_STR(62, s_message_done)
  }
}
const char*
str_http_header_states(unsigned int hstate)
{
  switch (hstate) {

    MAKE_CASE_STR(0, h_general)
    MAKE_CASE_STR(1, h_C)
    MAKE_CASE_STR(2, h_CO)
    MAKE_CASE_STR(3, h_CON)
    MAKE_CASE_STR(4, h_matching_connection)
    MAKE_CASE_STR(5, h_matching_proxy_connection)
    MAKE_CASE_STR(6, h_matching_content_length)
    MAKE_CASE_STR(7, h_matching_transfer_encoding)
    MAKE_CASE_STR(8, h_matching_upgrade)
    MAKE_CASE_STR(9, h_connection)
    MAKE_CASE_STR(10, h_content_length)
    MAKE_CASE_STR(11, h_transfer_encoding)
    MAKE_CASE_STR(12, h_upgrade)
    MAKE_CASE_STR(13, h_matching_transfer_encoding_chunked)
    MAKE_CASE_STR(14, h_matching_connection_token_start)
    MAKE_CASE_STR(15, h_matching_connection_keep_alive)
    MAKE_CASE_STR(16, h_matching_connection_close)
    MAKE_CASE_STR(17, h_matching_connection_upgrade)
    MAKE_CASE_STR(18, h_matching_connection_token)
    MAKE_CASE_STR(19, h_transfer_encoding_chunked)
    MAKE_CASE_STR(20, h_connection_keep_alive)
    MAKE_CASE_STR(21, h_connection_close)
    MAKE_CASE_STR(22, h_connection_upgrade)
  }
}
const char*
str_http_status_code(unsigned int scode)
{
  switch (scode) {
    MAKE_CASE_STR(0, unknown)
    MAKE_CASE_STR(100, continue)
    MAKE_CASE_STR(101, switching_protocols)
    MAKE_CASE_STR(200, ok)
    MAKE_CASE_STR(201, created)
    MAKE_CASE_STR(202, accepted)
    MAKE_CASE_STR(203, non - authoritative_information)
    MAKE_CASE_STR(204, no_content)
    MAKE_CASE_STR(205, reset_content)
    MAKE_CASE_STR(206, partial_content)
    MAKE_CASE_STR(300, multiple_choices)
    MAKE_CASE_STR(301, moved_permanently)
    MAKE_CASE_STR(302, found)
    MAKE_CASE_STR(303, see_other)
    MAKE_CASE_STR(304, not_modified)
    MAKE_CASE_STR(305, use_proxy)
    MAKE_CASE_STR(306, unused)
    MAKE_CASE_STR(307, temporary_redirect)
    MAKE_CASE_STR(400, bad_request)
    MAKE_CASE_STR(401, unauthorized)
    MAKE_CASE_STR(402, payment_required)
    MAKE_CASE_STR(403, forbidden)
    MAKE_CASE_STR(404, not_found)
    MAKE_CASE_STR(405, method_not_allowed)
    MAKE_CASE_STR(406, not_acceptable)
    MAKE_CASE_STR(407, proxy_authentication_required)
    MAKE_CASE_STR(408, request_timeout)
    MAKE_CASE_STR(409, conflict)
    MAKE_CASE_STR(410, gone)
    MAKE_CASE_STR(411, length_required)
    MAKE_CASE_STR(412, precondition_failed)
    MAKE_CASE_STR(413, request_entity_too_large)
    MAKE_CASE_STR(414, request - URI_too_long)
    MAKE_CASE_STR(415, unsupported_media_type)
    MAKE_CASE_STR(416, requested_range_not_satisfiable)
    MAKE_CASE_STR(417, expectation_failed)
    MAKE_CASE_STR(500, internal_server_error)
    MAKE_CASE_STR(501, not_implemented)
    MAKE_CASE_STR(502, bad_gateway)
    MAKE_CASE_STR(503, service_unavailable)
    MAKE_CASE_STR(504, gateway_timeout)
    MAKE_CASE_STR(505, HTTP_version_not_supported)
  }
}
size_t
parse(const char* buf, size_t len)
{
  size_t nparsed;
  currently_parsing_eof = (len == 0);
  nparsed = http_parser_execute(parser, &settings, buf, len);

  printf("HTTP->\n"
         "\ttype[%s]\n"
         "\tflags[%s]\n"
         "\tstate[%s]\n"
         "\theader_state[%s]\n"
         "\tindex[%u]\n"
         "\tnread[%u]\n"
         "\tcontent_length[%" PRId64 "]\n"
         "\thttp_major[%hd]\n"
         "\thttp_minor[%hd]\n"
         "\tstatus_code[%s]\n"
         "\tmethod[%s]\n"
         "\thttp_errno[%s]\n"
         "\tupgrade[%u]\n",
         str_http_type(parser->type),
         str_http_flags(parser->flags),
         str_http_state(parser->state),
         str_http_header_states(parser->header_state),
         parser->index,
         parser->nread,
         parser->content_length,
         parser->http_major,
         parser->http_minor,

         str_http_status_code(parser->status_code),

         http_method_str(parser->method),
         http_errno_name(parser->http_errno),
         parser->upgrade);
  return nparsed;
}

size_t
parse_count_body(const char* buf, size_t len)
{
  size_t nparsed;
  currently_parsing_eof = (len == 0);
  nparsed = http_parser_execute(parser, &settings_count_body, buf, len);
  return nparsed;
}

struct url_test
{
  const char* name;
  const char* url;
  int is_connect;
  struct http_parser_url u;
  int rv;
};

const struct url_test url_tests[8] = {
  { /*.name=*/"proxy request",
    /*.url=*/"http://hostname/",
    /*.is_connect=*/0, /*.u=*/
    { /*.field_set=*/(1 << UF_SCHEMA) | (1 << UF_HOST) | (1 << UF_PATH),
      /*.port=*/0, /*.field_data=*/
      {
        { 0, 4 } /* UF_SCHEMA */
        ,
        { 7, 8 } /* UF_HOST */
        ,
        { 0, 0 } /* UF_PORT */
        ,
        { 15, 1 } /* UF_PATH */
        ,
        { 0, 0 } /* UF_QUERY */
        ,
        { 0, 0 } /* UF_FRAGMENT */
        ,
        { 0, 0 } /* UF_USERINFO */
      } },
    /*.rv=*/0 }

  ,
  { "proxy request with port",
    "http://hostname:444/",
    0,
    { (1 << UF_SCHEMA) | (1 << UF_HOST) | (1 << UF_PORT) | (1 << UF_PATH),
      444,
      {
        { 0, 4 } /* UF_SCHEMA */
        ,
        { 7, 8 } /* UF_HOST */
        ,
        { 16, 3 } /* UF_PORT */
        ,
        { 19, 1 } /* UF_PATH */
        ,
        { 0, 0 } /* UF_QUERY */
        ,
        { 0, 0 } /* UF_FRAGMENT */
        ,
        { 0, 0 } /* UF_USERINFO */
      } },
    0 }

  ,
  { "CONNECT request",
    "hostname:443",
    1,
    { (1 << UF_HOST) | (1 << UF_PORT),
      443,
      {
        { 0, 0 } /* UF_SCHEMA */
        ,
        { 0, 8 } /* UF_HOST */
        ,
        { 9, 3 } /* UF_PORT */
        ,
        { 0, 0 } /* UF_PATH */
        ,
        { 0, 0 } /* UF_QUERY */
        ,
        { 0, 0 } /* UF_FRAGMENT */
        ,
        { 0, 0 } /* UF_USERINFO */
      } },
    0 }

  ,
  { "proxy ipv6 request",
    "http://[1:2::3:4]/",
    0,
    { (1 << UF_SCHEMA) | (1 << UF_HOST) | (1 << UF_PATH),
      0,
      {
        { 0, 4 } /* UF_SCHEMA */
        ,
        { 8, 8 } /* UF_HOST */
        ,
        { 0, 0 } /* UF_PORT */
        ,
        { 17, 1 } /* UF_PATH */
        ,
        { 0, 0 } /* UF_QUERY */
        ,
        { 0, 0 } /* UF_FRAGMENT */
        ,
        { 0, 0 } /* UF_USERINFO */
      } },
    0 }

  ,
  { "proxy ipv6 request with port",
    "http://[1:2::3:4]:67/",
    0,
    { (1 << UF_SCHEMA) | (1 << UF_HOST) | (1 << UF_PORT) | (1 << UF_PATH),
      67,
      {
        { 0, 4 } /* UF_SCHEMA */
        ,
        { 8, 8 } /* UF_HOST */
        ,
        { 18, 2 } /* UF_PORT */
        ,
        { 20, 1 } /* UF_PATH */
        ,
        { 0, 0 } /* UF_QUERY */
        ,
        { 0, 0 } /* UF_FRAGMENT */
        ,
        { 0, 0 } /* UF_USERINFO */
      } },
    0 }

  ,
  { "CONNECT ipv6 address",
    "[1:2::3:4]:443",
    1,
    { (1 << UF_HOST) | (1 << UF_PORT),
      443,
      {
        { 0, 0 } /* UF_SCHEMA */
        ,
        { 1, 8 } /* UF_HOST */
        ,
        { 11, 3 } /* UF_PORT */
        ,
        { 0, 0 } /* UF_PATH */
        ,
        { 0, 0 } /* UF_QUERY */
        ,
        { 0, 0 } /* UF_FRAGMENT */
        ,
        { 0, 0 } /* UF_USERINFO */
      } },
    0 }

  ,
  { "ipv4 in ipv6 address",
    "http://[2001:0000:0000:0000:0000:0000:1.9.1.1]/",
    0,
    { (1 << UF_SCHEMA) | (1 << UF_HOST) | (1 << UF_PATH),
      0,
      {
        { 0, 4 } /* UF_SCHEMA */
        ,
        { 8, 37 } /* UF_HOST */
        ,
        { 0, 0 } /* UF_PORT */
        ,
        { 46, 1 } /* UF_PATH */
        ,
        { 0, 0 } /* UF_QUERY */
        ,
        { 0, 0 } /* UF_FRAGMENT */
        ,
        { 0, 0 } /* UF_USERINFO */
      } },
    0 }

  ,
  { "extra ? in query string",
    "http://a.tbcdn.cn/p/fp/2010c/??fp-header-min.css,fp-base-min.css,"
    "fp-channel-min.css,fp-product-min.css,fp-mall-min.css,fp-category-min.css,"
    "fp-sub-min.css,fp-gdp4p-min.css,fp-css3-min.css,fp-misc-min.css?t="
    "20101022.css",
    0,
    { (1 << UF_SCHEMA) | (1 << UF_HOST) | (1 << UF_PATH) | (1 << UF_QUERY),
      0,
      {
        { 0, 4 } /* UF_SCHEMA */
        ,
        { 7, 10 } /* UF_HOST */
        ,
        { 0, 0 } /* UF_PORT */
        ,
        { 17, 12 } /* UF_PATH */
        ,
        { 30, 187 } /* UF_QUERY */
        ,
        { 0, 0 } /* UF_FRAGMENT */
        ,
        { 0, 0 } /* UF_USERINFO */
      } },
    0 }

};
void
dump_url(const char* url, const struct http_parser_url* u)
{
  unsigned int i;

  printf("\tfield_set: 0x%x, port: %u\n", u->field_set, u->port);
  for (i = 0; i < UF_MAX; i++) {
    if ((u->field_set & (1 << i)) == 0) {
      printf("\tfield_data[%u]: unset\n", i);
      continue;
    }

    printf("\tfield_data[%u]: off: %u len: %u part: \"%.*s\n\"",
           i,
           u->field_data[i].off,
           u->field_data[i].len,
           u->field_data[i].len,
           url + u->field_data[i].off);
  }
}
/*
struct http_parser_url {
  uint16_t field_set;            Bitmask of (1 << UF_*) values
  uint16_t port;                 Converted UF_PORT string

  struct {
    uint16_t off;                Offset into buffer in which field starts
    uint16_t len;                Length of run in buffer
  } field_data[UF_MAX];
};
enum http_parser_url_fields
  { UF_SCHEMA           = 0
  , UF_HOST             = 1
  , UF_PORT             = 2
  , UF_PATH             = 3
  , UF_QUERY            = 4
  , UF_FRAGMENT         = 5
  , UF_USERINFO         = 6
  , UF_MAX              = 7
  };
*/
const char*
str_http_url_field(uint16_t field)
{
  static char ret[256];
  ret[0] = '\0';
  if (field & UF_SCHEMA)
    strcat((char*)&ret, "schema|");
  if (field & UF_HOST)
    strcat((char*)&ret, "host|");
  if (field & UF_PORT)
    strcat((char*)&ret, "port|");
  if (field & UF_PATH)
    strcat((char*)&ret, "path|");
  if (field & UF_QUERY)
    strcat((char*)&ret, "query|");
  if (field & UF_FRAGMENT)
    strcat((char*)&ret, "fragment|");
  if (field & UF_USERINFO)
    strcat((char*)&ret, "user info|");
  if (field & UF_MAX)
    strcat((char*)&ret, "max|");
  return (const char*)&ret;
}
void
print_url(const char* url, struct http_parser_url* u)
{
  unsigned int i;
  /*printf("\tfield_set: 0x%x, port: %u\n", u->field_set, u->port);
  printf("\tfield_set: 0x%x, port: %u\n", u->field_set, u->port);
  printf("\t%s\n", str_http_url_field(u->field_set));
  for (i = 0; i < UF_MAX; i++) {
    if ((u->field_set & (1 << i)) == 0) {
      printf("\tfield_data[%u]: unset\n", i);
      continue;
    }

    printf("\tfield_data[%u]: off: %u len: %u part: \"%.*s\n\"",
           i,
           u->field_data[i].off,
           u->field_data[i].len,
           u->field_data[i].len,
           url + u->field_data[i].off);
  }*/
  // if(u->field_set & UF_SCHEMA)
  printf("\tschema field_data[%u]: off: %u len: %u part: \"%.*s\"\n",
         0,
         u->field_data[0].off,
         u->field_data[0].len,
         u->field_data[0].len,
         url + u->field_data[0].off);
  if (u->field_set & UF_HOST)
    printf("\thost field_data[%u]: off: %u len: %u part: \"%.*s\"\n",
           1,
           u->field_data[1].off,
           u->field_data[1].len,
           u->field_data[1].len,
           url + u->field_data[1].off);
  if (u->field_set & UF_PORT)
    printf("\tport field_data[%u]: off: %u len: %u part: \"%.*s\"\n",
           2,
           u->field_data[2].off,
           u->field_data[2].len,
           u->field_data[2].len,
           url + u->field_data[2].off);
  if (u->field_set & UF_PATH)
    printf("\tpath field_data[%u]: off: %u len: %u part: \"%.*s\"\n",
           3,
           u->field_data[3].off,
           u->field_data[3].len,
           u->field_data[3].len,
           url + u->field_data[3].off);
  if (u->field_set & UF_QUERY)
    printf("\tquery field_data[%u]: off: %u len: %u part: \"%.*s\"\n",
           4,
           u->field_data[4].off,
           u->field_data[4].len,
           u->field_data[4].len,
           url + u->field_data[4].off);
  if (u->field_set & UF_FRAGMENT)
    printf("\tfragment field_data[%u]: off: %u len: %u part: \"%.*s\"\n",
           5,
           u->field_data[5].off,
           u->field_data[5].len,
           u->field_data[5].len,
           url + u->field_data[5].off);
  if (u->field_set & UF_USERINFO)
    printf("\tuser info field_data[%u]: off: %u len: %u part: \"%.*s\"\n",
           6,
           u->field_data[6].off,
           u->field_data[6].len,
           u->field_data[6].len,
           url + u->field_data[6].off);
  /*if(u->field_set & UF_MAX)
   printf("\tmax field_data[%u]: off: %u len: %u part: \"%.*s\n\"",
          7,
          u->field_data[7].off,
          u->field_data[7].len,
          u->field_data[7].len,
          url + u->field_data[7].off);*/
}
void
test_parse_url(void)
{
  struct http_parser_url u;
  const struct url_test* test;
  unsigned int i;
  int rv;

  for (i = 0; i < (sizeof(url_tests) / sizeof(url_tests[0])); i++) {
    test = &url_tests[i];
    memset(&u, 0, sizeof(u));

    rv =
      http_parser_parse_url(test->url, strlen(test->url), test->is_connect, &u);
    print_url(test->url, &u);
    if (test->rv == 0) {
      if (rv != 0) {
        printf("\n*** http_parser_parse_url(\"%s\") \"%s\" test failed, "
               "unexpected rv %d ***\n\n",
               test->url,
               test->name,
               rv);
        abort();
      }

      if (memcmp(&u, &test->u, sizeof(u)) != 0) {
        printf("\n*** http_parser_parse_url(\"%s\") \"%s\" failed ***\n",
               test->url,
               test->name);

        printf("target http_parser_url:\n");
        dump_url(test->url, &test->u);
        printf("result http_parser_url:\n");
        dump_url(test->url, &u);

        abort();
      }
    } else {
      /* test->rv != 0 */
      if (rv == 0) {
        printf("\n*** http_parser_parse_url(\"%s\") \"%s\" test failed, "
               "unexpected rv %d ***\n\n",
               test->url,
               test->name,
               rv);
        abort();
      }
    }
  }
}

void
test_simple(const char* buf, enum http_errno err_expected)
{
  parser_init(HTTP_REQUEST);

  enum http_errno err;

  parse(buf, strlen(buf));
  err = HTTP_PARSER_ERRNO(parser);
  parse(NULL, 0);

  parser_free();

  /* In strict mode, allow us to pass with an unexpected HPE_STRICT as
   * long as the caller isn't expecting success.
   */
#if HTTP_PARSER_STRICT
  if (err_expected != err && err_expected != HPE_OK && err != HPE_STRICT) {
#else
  if (err_expected != err) {
#endif
    fprintf(stderr,
            "\n*** test_simple expected %s, but saw %s ***\n\n%s\n",
            http_errno_name(err_expected),
            http_errno_name(err),
            buf);
    abort();
  }
}
void
test_method_str(void)
{
  assert(0 == strcmp("GET", http_method_str(HTTP_GET)));
  assert(0 == strcmp("<unknown>", http_method_str(1337)));
}

int
main(void)
{
  parser = NULL;
  int i, j, k;
  int request_count;
  int response_count;
  unsigned long version;
  unsigned major;
  unsigned minor;
  unsigned patch;

  version = http_parser_version();
  major = (version >> 16) & 255;
  minor = (version >> 8) & 255;
  patch = version & 255;
  printf("http_parser v%u.%u.%u (0x%06lx)\n", major, minor, patch, version);

  printf("sizeof(http_parser) = %u\n", (unsigned int)sizeof(http_parser));

  //// API
  test_parse_url();
  test_method_str();

  /// REQUESTS

  test_simple("GET / HTP/1.1\r\n\r\n", HPE_INVALID_VERSION);

  // Well-formed but incomplete
  test_simple("GET / HTTP/1.1\r\n"
              "Content-Type: text/plain\r\n"
              "Content-Length: 6\r\n"
              "\r\n"
              "fooba",
              HPE_OK);

  static const char* all_methods[] = {
    "DELETE",
    "GET",
    "HEAD",
    "POST",
    "PUT",
    //"CONNECT", //CONNECT can't be tested like other methods, it's a tunnel
    "OPTIONS",
    "TRACE",
    "COPY",
    "LOCK",
    "MKCOL",
    "MOVE",
    "PROPFIND",
    "PROPPATCH",
    "SEARCH",
    "UNLOCK",
    "BIND",
    "REBIND",
    "UNBIND",
    "ACL",
    "REPORT",
    "MKACTIVITY",
    "CHECKOUT",
    "MERGE",
    "M-SEARCH",
    "NOTIFY",
    "SUBSCRIBE",
    "UNSUBSCRIBE",
    "PATCH",
    "PURGE",
    "MKCALENDAR",
    "LINK",
    "UNLINK",
    0
  };
  const char** this_method;
  for (this_method = all_methods; *this_method; this_method++) {
    char buf[200];
    sprintf(buf, "%s / HTTP/1.1\r\n\r\n", *this_method);
    test_simple(buf, HPE_OK);
  }

  static const char* bad_methods[] = {
    "ASDF",       "C******", "COLA", "GEM", "GETA",        "M****", "MKCOLA",
    "PROPPATCHA", "PUN",     "PX",   "SA",  "hello world", 0
  };
  for (this_method = bad_methods; *this_method; this_method++) {
    char buf[200];
    sprintf(buf, "%s / HTTP/1.1\r\n\r\n", *this_method);
    test_simple(buf, HPE_INVALID_METHOD);
  }

  // illegal header field name line folding
  test_simple("GET / HTTP/1.1\r\n"
              "name\r\n"
              " : value\r\n"
              "\r\n",
              HPE_INVALID_HEADER_TOKEN);

  const char* dumbfuck2 =
    "GET / HTTP/1.1\r\n"
    "X-SSL-Bullshit:   -----BEGIN CERTIFICATE-----\r\n"
    "\tMIIFbTCCBFWgAwIBAgICH4cwDQYJKoZIhvcNAQEFBQAwcDELMAkGA1UEBhMCVUsx\r\n"
    "\tETAPBgNVBAoTCGVTY2llbmNlMRIwEAYDVQQLEwlBdXRob3JpdHkxCzAJBgNVBAMT\r\n"
    "\tAkNBMS0wKwYJKoZIhvcNAQkBFh5jYS1vcGVyYXRvckBncmlkLXN1cHBvcnQuYWMu\r\n"
    "\tdWswHhcNMDYwNzI3MTQxMzI4WhcNMDcwNzI3MTQxMzI4WjBbMQswCQYDVQQGEwJV\r\n"
    "\tSzERMA8GA1UEChMIZVNjaWVuY2UxEzARBgNVBAsTCk1hbmNoZXN0ZXIxCzAJBgNV\r\n"
    "\tBAcTmrsogriqMWLAk1DMRcwFQYDVQQDEw5taWNoYWVsIHBhcmQYJKoZIhvcNAQEB\r\n"
    "\tBQADggEPADCCAQoCggEBANPEQBgl1IaKdSS1TbhF3hEXSl72G9J+WC/1R64fAcEF\r\n"
    "\tW51rEyFYiIeZGx/BVzwXbeBoNUK41OK65sxGuflMo5gLflbwJtHBRIEKAfVVp3YR\r\n"
    "\tgW7cMA/s/XKgL1GEC7rQw8lIZT8RApukCGqOVHSi/F1SiFlPDxuDfmdiNzL31+sL\r\n"
    "\t0iwHDdNkGjy5pyBSB8Y79dsSJtCW/iaLB0/n8Sj7HgvvZJ7x0fr+RQjYOUUfrePP\r\n"
    "\tu2MSpFyf+9BbC/aXgaZuiCvSR+8Snv3xApQY+fULK/xY8h8Ua51iXoQ5jrgu2SqR\r\n"
    "\twgA7BUi3G8LFzMBl8FRCDYGUDy7M6QaHXx1ZWIPWNKsCAwEAAaOCAiQwggIgMAwG\r\n"
    "\tA1UdEwEB/wQCMAAwEQYJYIZIAYb4QgHTTPAQDAgWgMA4GA1UdDwEB/wQEAwID6DAs\r\n"
    "\tBglghkgBhvhCAQ0EHxYdVUsgZS1TY2llbmNlIFVzZXIgQ2VydGlmaWNhdGUwHQYD\r\n"
    "\tVR0OBBYEFDTt/sf9PeMaZDHkUIldrDYMNTBZMIGaBgNVHSMEgZIwgY+AFAI4qxGj\r\n"
    "\tloCLDdMVKwiljjDastqooXSkcjBwMQswCQYDVQQGEwJVSzERMA8GA1UEChMIZVNj\r\n"
    "\taWVuY2UxEjAQBgNVBAsTCUF1dGhvcml0eTELMAkGA1UEAxMCQ0ExLTArBgkqhkiG\r\n"
    "\t9w0BCQEWHmNhLW9wZXJhdG9yQGdyaWQtc3VwcG9ydC5hYy51a4IBADApBgNVHRIE\r\n"
    "\tIjAggR5jYS1vcGVyYXRvckBncmlkLXN1cHBvcnQuYWMudWswGQYDVR0gBBIwEDAO\r\n"
    "\tBgwrBgEEAdkvAQEBAQYwPQYJYIZIAYb4QgEEBDAWLmh0dHA6Ly9jYS5ncmlkLXN1\r\n"
    "\tcHBvcnQuYWMudmT4sopwqlBWsvcHViL2NybC9jYWNybC5jcmwwPQYJYIZIAYb4QgEDBDAWLm"
    "h0\r\n"
    "\tdHA6Ly9jYS5ncmlkLXN1cHBvcnQuYWMudWsvcHViL2NybC9jYWNybC5jcmwwPwYD\r\n"
    "\tVR0fBDgwNjA0oDKgMIYuaHR0cDovL2NhLmdyaWQt5hYy51ay9wdWIv\r\n"
    "\tY3JsL2NhY3JsLmNybDANBgkqhkiG9w0BAQUFAAOCAQEAS/U4iiooBENGW/Hwmmd3\r\n"
    "\tXCy6Zrt08YjKCzGNjorT98g8uGsqYjSxv/hmi0qlnlHs+k/3Iobc3LjS5AMYr5L8\r\n"
    "\tUO7OSkgFFlLHQyC9JzPfmLCAugvzEbyv4Olnsr8hbxF1MbKZoQxUZtMVu29wjfXk\r\n"
    "\thTeApBv7eaKCWpSp7MCbvgzm74izKhu3vlDk9w6qVrxePfGgpKPqfHiOoGhFnbTK\r\n"
    "\twTC6o2xq5y0qZ03JonF7OJspEd3I5zKY3E+ov7/ZhW6DqT8UFvsAdjvQbXyhV8Eu\r\n"
    "\tYhixw1aKEPzNjNowuIseVogKOLXxWI5vAi5HgXdS0/ES5gDGsABo4fqovUKlgop3\r\n"
    "\tRA==\r\n"
    "\t-----END CERTIFICATE-----\r\n"
    "\r\n";
  test_simple(dumbfuck2, HPE_OK);

  const char* corrupted_connection = "GET / HTTP/1.1\r\n"
                                     "Host: www.example.com\r\n"
                                     "Connection\r\033\065\325eep-Alive\r\n"
                                     "Accept-Encoding: gzip\r\n"
                                     "\r\n";
  test_simple(corrupted_connection, HPE_INVALID_HEADER_TOKEN);

  const char* corrupted_header_name = "GET / HTTP/1.1\r\n"
                                      "Host: www.example.com\r\n"
                                      "X-Some-Header\r\033\065\325eep-Alive\r\n"
                                      "Accept-Encoding: gzip\r\n"
                                      "\r\n";
  test_simple(corrupted_header_name, HPE_INVALID_HEADER_TOKEN);

  struct http_parser_url u;
  memset(&u, 0, sizeof(struct http_parser_url));
  http_parser_parse_url(
    "abc://username:password@example.com:123/path/data?key=value#fragid1",
    strlen(
      "abc://username:password@example.com:123/path/data?key=value#fragid1"),
    0,
    &u);
  print_url(
    "abc://username:password@example.com:123/path/data?key=value#fragid1", &u);

  return 0;
}
