

#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
#include <inttypes.h>
#include <stdlib.h>
#include "request.pb.h"
#include "libM4UI-Rest-2.0.h"
#include "logger.h"

#define YYCTYPE        char
#define YYPEEK()       (cursor >= limit ? 0 : *cursor)
#define YYSKIP()       ++cursor
#define YYBACKUP()     marker = cursor
#define YYBACKUPCTX()  ctxmarker = cursor
#define YYRESTORE()    cursor = marker
#define YYRESTORECTX() cursor = ctxmarker

int session_found=0;
static struct http_message message;

//  parser_init(HTTP_REQUEST);

static size_t
http_strnlen(const char *s, size_t maxlen)
{
  const char *p;

  p = (const char*)memchr(s, '\0', maxlen);
  if (p == NULL)
    return maxlen;

  return p - s;
}
static size_t
http_strlncat(char *dst, size_t len, const char *src, size_t n)
{
  size_t slen;
  size_t dlen;
  size_t rlen;
  size_t ncpy;

  slen = http_strnlen(src, n);
  dlen = http_strnlen(dst, len);

  if (dlen < len) {
    rlen = len - dlen;
    ncpy = slen < rlen ? slen : (rlen - 1);
    memcpy(dst + dlen, src, ncpy);
    dst[dlen + ncpy] = '\0';
  }
  assert(len > slen + dlen);
  return slen + dlen;
}

static size_t
http_strlcat(char *dst, const char *src, size_t len)
{
  return http_strlncat(dst, len, src, (size_t) -1);
}

static size_t
http_strlncpy(char *dst, size_t len, const char *src, size_t n)
{
  size_t slen;
  size_t ncpy;

  slen = http_strnlen(src, n);

  if (len > 0) {
    ncpy = slen < len ? slen : (len - 1);
    memcpy(dst, src, ncpy);
    dst[ncpy] = '\0';
  }

  assert(len > slen);
  return slen;
}

static size_t
http_strlcpy(char *dst, const char *src, size_t len)
{
  return http_strlncpy(dst, len, src, (size_t) -1);
}
unsigned long version(void)
{
   return  http_parser_version();
}
void *
init (int type)
{
   http_parser *parser;
   parser = (http_parser*)malloc(sizeof(http_parser));
   http_parser_init(parser, type);
   memset(&message, 0, sizeof message);
   return (void*)parser;
}

void
destroy (void *p)
{
  free(p);
}

static int
message_begin_cb (http_parser *p)
{
  message.message_begin_cb_called = TRUE;
  return 0;
}

static int
request_url_cb(http_parser *p, const char *buf, size_t len)
{
   char *str;
   const char *url=NULL;
   struct http_parser_url u;
   unsigned int i,is_connect;
   int rv;
   http_strlncat(message.request_url,
      sizeof(message.request_url),
      buf,
      len);
   log_debug("request url[%s]",message.request_url);
   is_connect = (p->method == HTTP_CONNECT)?1:0;
   rv = http_parser_parse_url(message.request_url,
                               strlen(message.request_url),
                               is_connect,
                               &u);
   if (rv != 0) {
      log_error("http_parser_parse_url(\"%s\") \"%s\" test failed, "
      "unexpected rv %d", message.request_url, message.name, rv);
      //abort();
      return rv;
   }
   url=message.request_url;
   if(u.field_set & 1<<0){
       log_debug("\tscheme field_data[%u]: off: %u len: %u part: \"%.*s\"",
              0,
              u.field_data[0].off,
              u.field_data[0].len,
              u.field_data[0].len,
              url + u.field_data[0].off);fflush(stdout);
              str=(char*)malloc(u.field_data[0].len+1);
              str[0]='\0';
              http_strlncat(str,
                 u.field_data[0].len+1,
                 url + u.field_data[0].off,
                 u.field_data[0].len);

              message.scheme=str;
           }
      if(u.field_set & 1<<1){
          log_debug("\thost field_data[%u]: off: %u len: %u part: \"%.*s\"",
              1,
              u.field_data[1].off,
              u.field_data[1].len,
              u.field_data[1].len,
              url + u.field_data[1].off);fflush(stdout);
              str=(char*)malloc(u.field_data[1].len+1);
              str[0]='\0';
              http_strlncat(str,
                 u.field_data[1].len+1,
                 url + u.field_data[1].off,
                 u.field_data[1].len);

              message.host=str;
           }
      if(u.field_set & 1<<2){
       log_debug("\tport field_data[%u]: off: %u len: %u part: \"%.*s\"",
              2,
              u.field_data[2].off,
              u.field_data[2].len,
              u.field_data[2].len,
              url + u.field_data[2].off);fflush(stdout);
              str=(char*)malloc(u.field_data[2].len+1);
              str[0]='\0';
              http_strlncat(str,
                 u.field_data[2].len+1,
                 url + u.field_data[2].off,
                 u.field_data[2].len);
                message.port=atoi(str);
               free(str);
      }
      if(u.field_set & 1<<3){
       log_debug("\tpath field_data[%u]: off: %u len: %u part: \"%.*s\"",
              3,
              u.field_data[3].off,
              u.field_data[3].len,
              u.field_data[3].len,
              url + u.field_data[3].off);fflush(stdout);
              http_strlncat(message.request_path,
                 sizeof(message.request_path),
                 url + u.field_data[3].off,
                 u.field_data[3].len);
           }
      if(u.field_set & 1<<4){
       log_debug("\tquery field_data[%u]: off: %u len: %u part: \"%.*s\"",
              4,
              u.field_data[4].off,
              u.field_data[4].len,
              u.field_data[4].len,
              url + u.field_data[4].off);fflush(stdout);
              http_strlncat(message.query_string,
                 sizeof(message.query_string),
                 url + u.field_data[4].off,
                 u.field_data[4].len);
           }
      if(u.field_set & 1<<5){
       log_debug("\tfragment field_data[%u]: off: %u len: %u part: \"%.*s\"",
              5,
              u.field_data[5].off,
              u.field_data[5].len,
              u.field_data[5].len,
              url + u.field_data[5].off);fflush(stdout);
              http_strlncat(message.fragment,
                 sizeof(message.fragment),
                 url + u.field_data[5].off,
                 u.field_data[5].len);
           }
      if(u.field_set & 1<<6){
       log_debug("\tuser info field_data[%u]: off: %u len: %u part: \"%.*s\"",
              6,
              u.field_data[6].off,
              u.field_data[6].len,
              u.field_data[6].len,
              url + u.field_data[6].off);fflush(stdout);
              str=(char*)malloc(u.field_data[6].len+1);
              str[0]='\0';
              http_strlncat(str,
                 u.field_data[6].len+1,
                 url + u.field_data[6].off,
                 u.field_data[6].len);

              message.userinfo=str;
           }
  return 0;
}
static int
response_status_cb (http_parser *p, const char *buf, size_t len)
{
  http_strlncat(message.response_status,
           sizeof(message.response_status),
           buf,
           len);
  return 0;
}
static int
scanner(const char * cursor, const char * const limit)
{
   const char * marker;
   const char * ctxmarker;
   /*!re2c
   re2c:yyfill:enable = 0;
   ""{ return 0;}
   "M4U_SessionID"{ session_found=1;return 1;}
   */
   return 0;
}
static int
header_field_cb (http_parser *p, const char *buf, size_t len)
{
  struct http_message *m = &message;
  int ret;
  if (m->last_header_element != HTTP_FIELD)
    m->num_headers++;

  http_strlncat(m->headers[m->num_headers-1][0],
           sizeof(m->headers[m->num_headers-1][0]),
           buf,
           len);

  m->last_header_element = HTTP_FIELD;

  ret=scanner(buf,buf+len);
  if(ret)
  {
     log_debug("M4U_SessionID:[%s]",m->headers[m->num_headers-1][0]);
  }
  return 0;
}
static int
header_value_cb (http_parser *p, const char *buf, size_t len)
{
  struct http_message *m = &message;

  http_strlncat(m->headers[m->num_headers-1][1],
           sizeof(m->headers[m->num_headers-1][1]),
           buf,
           len);

  m->last_header_element = HTTP_VALUE;
  if(session_found)
  {
     log_debug("session value:[%s]",m->headers[m->num_headers-1][1]);
     m->session[0]='\0';
     http_strlncat(m->session,
              65,
              buf,
              len);
    session_found=0;
   }
  return 0;
}
static int
headers_complete_cb (http_parser *p)
{
  message.method = p->method;
  message.status_code = p->status_code;
  message.http_major = p->http_major;
  message.http_minor = p->http_minor;
  message.headers_complete_cb_called = TRUE;
  message.should_keep_alive = http_should_keep_alive(p);
  return 0;
}
static void
check_body_is_final (const http_parser *p)
{
  if (message.body_is_final) {
    log_error("*** Error http_body_is_final() should return 1 "
                    "on last on_body callback call "
                    "but it doesn't! ***");
    //abort();
    return;
  }
  message.body_is_final = http_body_is_final(p);
}
static int
body_cb (http_parser *p, const char *buf, size_t len)
{
  http_strlncat(message.body,
           sizeof(message.body),
           buf,
           len);
  message.body_size += len;
  check_body_is_final(p);
  return 0;
}
static int
message_complete_cb (http_parser *p)
{
  if (message.should_keep_alive != http_should_keep_alive(p))
  {
    log_error("*** Error http_should_keep_alive() should have same "
                    "value in both on_message_complete and on_headers_complete "
                    "but it doesn't! ***");
    //abort();
    return 0;
  }

  if (message.body_size &&
      http_body_is_final(p) &&
      !message.body_is_final)
  {
    log_error("*** Error http_body_is_final() should return 1 "
                    "on last on_body callback call "
                    "but it doesn't! ***");
    //abort();
    return 0;
  }

  message.message_complete_cb_called = TRUE;

  message.message_complete_on_eof = p->eof;

  return 0;
}
static int
chunk_header_cb (http_parser *p)
{
  int chunk_idx = message.num_chunks;
  message.num_chunks++;
  if (chunk_idx < HTTP_MAX_CHUNKS) {
    message.chunk_lengths[chunk_idx] = p->content_length;
  }
  return 0;
}
int
chunk_complete_cb (http_parser *p)
{
  message.num_chunks_complete++;
  return 0;
}
static http_parser_settings settings =
  {/*.on_message_begin = */message_begin_cb
  ,/*.on_url = */request_url_cb
  ,/*.on_status = */response_status_cb
  ,/*.on_header_field = */header_field_cb
  ,/*.on_header_value = */header_value_cb
  ,/*.on_headers_complete = */headers_complete_cb
  ,/*.on_body = */body_cb
  ,/*.on_message_complete = */message_complete_cb
  ,/*.on_chunk_header = */chunk_header_cb
  ,/*.on_chunk_complete = */chunk_complete_cb
  };
const char*str_http_type(unsigned int type)
{
   switch(type)
   {
      case HTTP_REQUEST:return "request";
      case HTTP_RESPONSE:return "response";
      case HTTP_BOTH:return "request_response";
      default: return "unknown";
   }
}
const char*str_http_flags(unsigned int flags)
{
   switch(flags)
   {
      case 0:return "no flags";
      case F_CHUNKED:return "chunked";
      case F_CONNECTION_KEEP_ALIVE:return "connection keep alive";
      case F_CONNECTION_CLOSE:return"connection close";
      case F_CONNECTION_UPGRADE:return "connection upgrade";
      case F_TRAILING: return "trailing";
      case F_UPGRADE:return "upgrade";
      case F_SKIPBODY:return "skipbody";
      default:return "<unknown>";
  }
}
#define MAKE_CASE_STR(index,vlr) case index: return #vlr;

const char*str_http_state(unsigned int state)
{
   switch(state)
   {
      MAKE_CASE_STR(1,s_dead)
      MAKE_CASE_STR(2,s_start_req_or_res)
      MAKE_CASE_STR(3,s_res_or_resp_H)
      MAKE_CASE_STR(4,s_start_res)
      MAKE_CASE_STR(5,s_res_H)
      MAKE_CASE_STR(6,s_res_HT)
      MAKE_CASE_STR(7,s_res_HTT)
      MAKE_CASE_STR(8,s_res_HTTP)
      MAKE_CASE_STR(9,s_res_first_http_major)
      MAKE_CASE_STR(10,s_res_http_major)
      MAKE_CASE_STR(11,s_res_first_http_minor)
      MAKE_CASE_STR(12,s_res_http_minor)
      MAKE_CASE_STR(13,s_res_first_status_code)
      MAKE_CASE_STR(14,s_res_status_code)
      MAKE_CASE_STR(15,s_res_status_start)
      MAKE_CASE_STR(16,s_res_status)
      MAKE_CASE_STR(17,s_res_line_almost_done)
      MAKE_CASE_STR(18,s_start_req)
      MAKE_CASE_STR(19,s_req_method)
      MAKE_CASE_STR(20,s_req_spaces_before_url)
      MAKE_CASE_STR(21,s_req_schema)
      MAKE_CASE_STR(22,s_req_schema_slash)
      MAKE_CASE_STR(23,s_req_schema_slash_slash)
      MAKE_CASE_STR(24,s_req_server_start)
      MAKE_CASE_STR(25,s_req_server)
      MAKE_CASE_STR(26,s_req_server_with_at)
      MAKE_CASE_STR(27,s_req_path)
      MAKE_CASE_STR(28,s_req_query_string_start)
      MAKE_CASE_STR(29,s_req_query_string)
      MAKE_CASE_STR(30,s_req_fragment_start)
      MAKE_CASE_STR(31,s_req_fragment)
      MAKE_CASE_STR(32,s_req_http_start)
      MAKE_CASE_STR(33,s_req_http_H)
      MAKE_CASE_STR(34,s_req_http_HT)
      MAKE_CASE_STR(35,s_req_http_HTT)
      MAKE_CASE_STR(36,s_req_http_HTTP)
      MAKE_CASE_STR(37,s_req_first_http_major)
      MAKE_CASE_STR(38,s_req_http_major)
      MAKE_CASE_STR(39,s_req_first_http_minor)
      MAKE_CASE_STR(40,s_req_http_minor)
      MAKE_CASE_STR(41,s_req_line_almost_done)
      MAKE_CASE_STR(42,s_header_field_start)
      MAKE_CASE_STR(43,s_header_field)
      MAKE_CASE_STR(44,s_header_value_discard_ws)
      MAKE_CASE_STR(45,s_header_value_discard_ws_almost_done)
      MAKE_CASE_STR(46,s_header_value_discard_lws)
      MAKE_CASE_STR(47,s_header_value_start)
      MAKE_CASE_STR(48,s_header_value)
      MAKE_CASE_STR(49,s_header_value_lws)
      MAKE_CASE_STR(50,s_header_almost_done)
      MAKE_CASE_STR(51,s_chunk_size_start)
      MAKE_CASE_STR(52,s_chunk_size)
      MAKE_CASE_STR(53,s_chunk_parameters)
      MAKE_CASE_STR(54,s_chunk_size_almost_done)
      MAKE_CASE_STR(55,s_headers_almost_done)
      MAKE_CASE_STR(56,s_headers_done)
      MAKE_CASE_STR(57,s_chunk_data)
      MAKE_CASE_STR(58,s_chunk_data_almost_done)
      MAKE_CASE_STR(59,s_chunk_data_done)
      MAKE_CASE_STR(60,s_body_identity)
      MAKE_CASE_STR(61,s_body_identity_eof)
      MAKE_CASE_STR(62,s_message_done)
      default: return "unknown";
   }
}
const char*str_http_header_states(unsigned int hstate)
{
   switch(hstate)
   {

      MAKE_CASE_STR(0,h_general)
      MAKE_CASE_STR(1,h_C)
      MAKE_CASE_STR(2,h_CO)
      MAKE_CASE_STR(3,h_CON)
      MAKE_CASE_STR(4,h_matching_connection)
      MAKE_CASE_STR(5,h_matching_proxy_connection)
      MAKE_CASE_STR(6,h_matching_content_length)
      MAKE_CASE_STR(7,h_matching_transfer_encoding)
      MAKE_CASE_STR(8,h_matching_upgrade)
      MAKE_CASE_STR(9,h_connection)
      MAKE_CASE_STR(10,h_content_length)
      MAKE_CASE_STR(11,h_transfer_encoding)
      MAKE_CASE_STR(12,h_upgrade)
      MAKE_CASE_STR(13,h_matching_transfer_encoding_chunked)
      MAKE_CASE_STR(14,h_matching_connection_token_start)
      MAKE_CASE_STR(15,h_matching_connection_keep_alive)
      MAKE_CASE_STR(16,h_matching_connection_close)
      MAKE_CASE_STR(17,h_matching_connection_upgrade)
      MAKE_CASE_STR(18,h_matching_connection_token)
      MAKE_CASE_STR(19,h_transfer_encoding_chunked)
      MAKE_CASE_STR(20,h_connection_keep_alive)
      MAKE_CASE_STR(21,h_connection_close)
      MAKE_CASE_STR(22,h_connection_upgrade)
      default: return "unknown";
  }
}
const char*str_http_status_code(unsigned int scode)
{
   switch(scode)
   {
      MAKE_CASE_STR(0,unknown)
      MAKE_CASE_STR(100,continue)
      MAKE_CASE_STR(101,switching_protocols)
      MAKE_CASE_STR(200,ok)
      MAKE_CASE_STR(201,created)
      MAKE_CASE_STR(202,accepted)
      MAKE_CASE_STR(203,non-authoritative_information)
      MAKE_CASE_STR(204,no_content)
      MAKE_CASE_STR(205,reset_content)
      MAKE_CASE_STR(206,partial_content)
      MAKE_CASE_STR(300,multiple_choices)
      MAKE_CASE_STR(301,moved_permanently)
      MAKE_CASE_STR(302,found)
      MAKE_CASE_STR(303,see_other)
      MAKE_CASE_STR(304,not_modified)
      MAKE_CASE_STR(305,use_proxy)
      MAKE_CASE_STR(306,unused)
      MAKE_CASE_STR(307,temporary_redirect)
      MAKE_CASE_STR(400,bad_request)
      MAKE_CASE_STR(401,unauthorized)
      MAKE_CASE_STR(402,payment_required)
      MAKE_CASE_STR(403,forbidden)
      MAKE_CASE_STR(404,not_found)
      MAKE_CASE_STR(405,method_not_allowed)
      MAKE_CASE_STR(406,not_acceptable)
      MAKE_CASE_STR(407,proxy_authentication_required)
      MAKE_CASE_STR(408,request_timeout)
      MAKE_CASE_STR(409,conflict)
      MAKE_CASE_STR(410,gone)
      MAKE_CASE_STR(411,length_required)
      MAKE_CASE_STR(412,precondition_failed)
      MAKE_CASE_STR(413,request_entity_too_large)
      MAKE_CASE_STR(414,request-URI_too_long)
      MAKE_CASE_STR(415,unsupported_media_type)
      MAKE_CASE_STR(416,requested_range_not_satisfiable)
      MAKE_CASE_STR(417,expectation_failed)
      MAKE_CASE_STR(500,internal_server_error)
      MAKE_CASE_STR(501,not_implemented)
      MAKE_CASE_STR(502,bad_gateway)
      MAKE_CASE_STR(503,service_unavailable)
      MAKE_CASE_STR(504,gateway_timeout)
      MAKE_CASE_STR(505,HTTP_version_not_supported)
      default: return "unknown";
   }
}
static size_t parse(http_parser *parser,const char *buf, size_t len)
{
   size_t nparsed;
   parser->eof = (len == 0);
   nparsed = http_parser_execute(parser, &settings, buf, len);
   log_debug("HTTP->\n"
      "\ttype[%s]"
      "\tflags[%s]"
      "\tstate[%s]"
      "\theader_state[%s]"
      "\tindex[%u]"
      "\tnread[%u]"
      "\tcontent_length[%" PRId64 "]"
      "\thttp_major[%hd]"
      "\thttp_minor[%hd]"
      "\tstatus_code[%s]"
      "\tmethod[%s]"
      "\thttp_errno[%s]"
      "\tupgrade[%u]",
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
      parser->upgrade
   );
   return nparsed;
}
void convert2protobuffer(m4u_interfaces::Request* request,struct http_message *message)
{
   int i;
   std::string key;
   std::string value;
   m4u_interfaces::Request::Header* headers;
   m4u_interfaces::Request::Chunk* chunks;
   if(message->scheme)request->set_scheme(message->scheme);
   if(message->name)request->set_name(message->name);
   if(message->raw)request->set_raw(message->raw);
   request->set_type(message->type);
   request->set_method(message->method);
   request->set_status_code(message->status_code);
   if(message->response_status[0])request->set_response_status(message->response_status);
   if(message->request_path[0])request->set_request_path(message->request_path);
   if(message->request_url[0])request->set_request_url(message->request_url);
   if(message->fragment[0])request->set_fragment(message->fragment);
   if(message->query_string[0])request->set_query_string(message->query_string);
   if(message->body[0])request->set_body(message->body);
   request->set_body_size(message->body_size);
   if(message->host)request->set_host(message->host);
   if(message->userinfo)request->set_userinfo(message->userinfo);
   request->set_port(message->port);
   request->set_num_headers(message->num_headers);
   request->set_last_header_element(message->last_header_element);


   for(i=0;i<message->num_headers;i++)
   {
      headers = request->add_headers();
      key=(char*)&message->headers[i][0];
      value=(char*)&message->headers[i][1];
      headers->set_key(key);
      headers->set_value(value);
   }
   request->set_should_keep_alive(message->should_keep_alive);
   request->set_num_chunks(message->num_chunks);
   request->set_num_chunks_complete(message->num_chunks_complete);

   for(i=0;i<message->num_chunks;i++)
   {
      chunks = request->add_chunks();
      chunks->set_length(message->chunk_lengths[i]);
   }
   if(message->upgrade)request->set_upgrade(message->upgrade);
   request->set_http_major(message->http_major);
   request->set_http_minor(message->http_minor);
   request->set_session(message->session);
}
int scan(void *p,const char*name,const char * input,size_t ilen,char **path,char **output, size_t olen )
{
   static m4u_interfaces::Request request;
   std::string str_output;
   http_parser *parser=(http_parser *)p;
   enum http_errno err;

   if(!log_output)InitializeLog();

   session_found=0;
   memset(&message,0,sizeof(message));
   message.name=name;
   message.raw=input;
   parse(parser,input, ilen);
   err = HTTP_PARSER_ERRNO(parser);
   log_debug("name[%s] buf[%lu](%s)",name,ilen,input);
   if (HPE_OK != err) {
      log_error("*** test_simple error %s *** %s",
       http_errno_name(err), input);
      //abort();
      return err;
   }
   log_debug("convert");

   request.Clear();

   convert2protobuffer(&request,&message);

   if(request.SerializeToString(&str_output))
   {
      if(!olen)
      {
         *output=(char*)calloc(1,str_output.length()+1);
         memcpy(*output,str_output.c_str(),str_output.length());
      }
   }
   *path=strdup(message.request_path);
   if(message.scheme)free((void*)message.scheme);
   if(message.host)free((void*)message.host);
   if(message.userinfo)free((void*)message.userinfo);

   return HPE_OK;
}
const char* str_err(int err)
{
   return http_errno_name(err);
}
