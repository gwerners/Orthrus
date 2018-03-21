#ifndef library_h
#define library_h

#define USE_UV_PLUGIN

#ifndef USE_UV_PLUGIN
#include <dlfcn.h>
#else
#include <uv.h>
#endif

// type!
#define HTTP_REQUEST 0
#define HTTP_RESPONSE 1
#define HTTP_BOTH 2

#ifdef __cplusplus
extern "C" {
#endif

#undef TRUE
#define TRUE 1
#undef FALSE
#define FALSE 0

#define HTTP_MAX_HEADERS 13
#define HTTP_MAX_ELEMENT_SIZE 2048
#define HTTP_MAX_CHUNKS 16

#define HTTP_NONE 0
#define HTTP_FIELD 1
#define HTTP_VALUE 2
// enum http_parser_type
#define HTTP_REQUEST 0
#define HTTP_RESPONSE 1
#define HTTP_BOTH 2
struct http_message
{
  const char* name; // for debugging purposes
  const char* raw;
  int type;
  unsigned int method;
  int status_code;
  char response_status[HTTP_MAX_ELEMENT_SIZE];
  char request_path[HTTP_MAX_ELEMENT_SIZE];
  char request_url[HTTP_MAX_ELEMENT_SIZE];
  char fragment[HTTP_MAX_ELEMENT_SIZE];
  char query_string[HTTP_MAX_ELEMENT_SIZE];
  char body[HTTP_MAX_ELEMENT_SIZE];
  size_t body_size;
  const char* host;
  const char* userinfo;
  uint16_t port;
  int num_headers;
  int last_header_element; // HTTP_NONE | HTTP_FIELD | HTTP_VALUE
  char headers[HTTP_MAX_HEADERS][2][HTTP_MAX_ELEMENT_SIZE];
  int should_keep_alive;

  int num_chunks;
  int num_chunks_complete;
  int chunk_lengths[HTTP_MAX_CHUNKS];

  const char* upgrade;

  unsigned short http_major;
  unsigned short http_minor;

  int message_begin_cb_called;
  int headers_complete_cb_called;
  int message_complete_cb_called;
  int message_complete_on_eof;
  int body_is_final;
};

struct ScannerPlugin
{
#ifndef USE_UV_PLUGIN
  void* handle;
#else
  uv_lib_t* handle;
#endif
  unsigned long (*p_version)(void);
  void* (*p_init)(int type);
  void (*p_destroy)(void* p);
  int (*p_scan)(void* p,
                const char* input,
                size_t ilen,
                char** output,
                size_t olen);
  const char* (*p_str_err)(int err);
};

unsigned long
version(void);
void*
init(int type);
void
destroy(void* p);
int
scan(void* p, const char* input, size_t ilen, char** output, size_t olen);
const char*
str_err(int err);

#ifdef __cplusplus
}
#endif
#endif
