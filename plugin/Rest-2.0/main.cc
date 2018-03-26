/*
Copyright 2018 Gabriel Wernersbach Farinas

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "http_parser.h"
#include "libRest-2.0.h"
#include "logger.h"
#include "request.pb.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

struct ScannerPlugin scanner_plugin;

void
ListRequests(const interfaces::RequestBlock& msg)
{
  for (int i = 0; i < msg.request_size(); i++) {
    const interfaces::Request& request = msg.request(i);
    log_debug("###########################################");
    log_debug(" scheme:%s ", request.scheme().c_str());
    log_debug(" name: %s", request.name().c_str());
    log_debug(" raw: %s", request.raw().c_str());
    log_debug(" type: %d", request.type());
    log_debug(" method: %d", request.method());
    log_debug(" status_code: %d", request.status_code());
    log_debug(" response_status: %s", request.response_status().c_str());
    log_debug(" request_path: %s", request.request_path().c_str());
    log_debug(" request_url: %s", request.request_url().c_str());
    log_debug(" fragment: %s", request.fragment().c_str());
    log_debug(" query_string: %s", request.query_string().c_str());
    log_debug(" body: %s", request.body().c_str());
    log_debug(" body_size: %llu", request.body_size());
    log_debug(" host: %s", request.host().c_str());
    log_debug(" userinfo: %s", request.userinfo().c_str());
    log_debug(" port: %d", request.port());
    log_debug(" num_headers: %d", request.num_headers());
    log_debug(" last_header_element: %d", request.last_header_element());
    log_debug(" should_keep_alive: %d", request.should_keep_alive());
    /*
       name
       raw
       type
       method
       status_code
       response_status
       request_path
       request_url
       fragment
       query_string
       body
       body_size
       host
       userinfo
       port
       num_headers
       last_header_element
       should_keep_alive
    */
    if (request.chunks_size() > 0) {
      log_debug(" Chunks : ");
      for (int j = 0; j < request.chunks_size(); j++) {
        const interfaces::Request::Chunk& chunk = request.chunks(j);
        log_debug("  %d", chunk.length());
      }
    }
    /*
    upgrade
    http_major
    http_minor
    */
    log_debug(" upgrade: %s", request.upgrade().c_str());
    log_debug(" http_major: %d", request.http_major());
    log_debug(" http_minor: %d", request.http_minor());

    if (request.headers_size() > 0) {
      log_debug(" Headers : ");
      for (int j = 0; j < request.headers_size(); j++) {
        const interfaces::Request::Header& header = request.headers(j);
        log_debug("  %s: %s", header.key().c_str(), header.value().c_str());
      }
    }
    log_debug(" session: %s", request.session().c_str());
  }
}
/*
   unsigned long (*p_version)(void);
   void* (*p_init)(int type);
   void (*p_destroy)(void *p);
   int (*p_scan)(const char * input,size_t ilen,char **output, size_t olen );
   const char*(*p_str_err)(int err);
*/
unsigned long
version_dummy(void)
{
  return 0;
}
void*
init_dummy(int type)
{
  return NULL;
}
void
destroy_dummy(void* p)
{
  return;
}
int
scan_dummy(void* p,
           const char* name,
           const char* input,
           size_t ilen,
           char** path,
           char** output,
           size_t olen)
{
  if (output && *output)
    *output = NULL;
  if (path && *path)
    *path = NULL;
  return 0;
}
const char*
str_err_dummy(int err)
{
  return "error dummy";
}

void
LoadScannerPluginDummy()
{
  scanner_plugin.handle = NULL;

  scanner_plugin.p_version = (unsigned long (*)(void))version_dummy;
  scanner_plugin.p_init = (void* (*)(int))init_dummy;
  scanner_plugin.p_destroy = (void (*)(void*))destroy_dummy;
  scanner_plugin.p_scan = (int (*)(
    void*, const char*, const char*, size_t, char**, char**, size_t))scan_dummy;
  scanner_plugin.p_str_err = (const char* (*)(int))str_err_dummy;
}

void
LoadScannerPlugin(const char* name)
{
  memset(&scanner_plugin, 0, sizeof(scanner_plugin));
#ifndef USE_UV_PLUGIN
  log_debug("loading [%s]", name);
  scanner_plugin.handle = dlopen(name, RTLD_LAZY);
  if (!scanner_plugin.handle) {
    log_debug("unable to load [%s] due to [%s]", name, dlerror());
  }
  scanner_plugin.p_version =
    (unsigned long (*)(void))dlsym(scanner_plugin.handle, "version");
  scanner_plugin.p_init = (void* (*)(int))dlsym(scanner_plugin.handle, "init");
  scanner_plugin.p_destroy =
    (void (*)(void*))dlsym(scanner_plugin.handle, "destroy");
  scanner_plugin.p_scan =
    (int (*)(void*, const char*, const char*, size_t, char**, char**, size_t))
      dlsym(scanner_plugin.handle, "scan");
  scanner_plugin.p_str_err =
    (const char* (*)(int))dlsym(scanner_plugin.handle, "str_err");

  log_debug("scanner_plugin.handle[%p]", scanner_plugin.handle);
  log_debug("scanner_plugin.version[%p]", scanner_plugin.p_version);
  fflush(stdout);
#else
  scanner_plugin.handle = (uv_lib_t*)malloc(sizeof(uv_lib_t));

  log_debug("Loading %s", name);
  if (uv_dlopen(name, scanner_plugin.handle)) {
    log_error("Error: %s", uv_dlerror(scanner_plugin.handle));
    return;
  }
  if (uv_dlsym(
        scanner_plugin.handle, "version", (void**)&scanner_plugin.p_version)) {
    log_error("dlsym error: %s", uv_dlerror(scanner_plugin.handle));
    return;
  }
  if (uv_dlsym(scanner_plugin.handle, "init", (void**)&scanner_plugin.p_init)) {
    log_error("dlsym error: %s", uv_dlerror(scanner_plugin.handle));
    return;
  }
  if (uv_dlsym(
        scanner_plugin.handle, "destroy", (void**)&scanner_plugin.p_destroy)) {
    log_error("dlsym error: %s", uv_dlerror(scanner_plugin.handle));
    return;
  }
  if (uv_dlsym(scanner_plugin.handle, "scan", (void**)&scanner_plugin.p_scan)) {
    log_error("dlsym error: %s", uv_dlerror(scanner_plugin.handle));
    return;
  }
  if (uv_dlsym(
        scanner_plugin.handle, "str_err", (void**)&scanner_plugin.p_str_err)) {
    log_error("dlsym error: %s", uv_dlerror(scanner_plugin.handle));
    return;
  }
#endif
}
int
LibraryExists(const char* filename)
{
  int ret;
  struct stat buf;
  ret = stat(filename, &buf);
  if (ret == -1)
    return 0;
  if (buf.st_mode & S_IXUSR) // execute permission!
    return 1;
  return 0;
}
void
LoadPlugin(const char* libname)
{
  int ok;
  ok = LibraryExists(libname);
  if (ok) {
    log_debug("library exists");
#ifndef USE_UV_PLUGIN
    if (scanner_plugin.handle)
      dlclose(scanner_plugin.handle);
#else
    if (scanner_plugin.handle)
      uv_dlclose(scanner_plugin.handle);
#endif
    LoadScannerPlugin(libname);
  } else {
    log_error("loading new library[%s] FAILED", libname);
    LoadScannerPluginDummy();
  }
}

int
main()
{
  int ret;
  char *str, *output, *path;
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  interfaces::RequestBlock msg;

  InitializeLog();
  // load plugin
  LoadPlugin("libRest-2.0.so");
  void* scanner = scanner_plugin.p_init(HTTP_REQUEST);

  str = (char*)malloc(1024);
  /*strcpy(str,
     "GET /path/cgi.script HTTP/1.1\n"
     "Host: 127.0.0.1:7000\n"
     "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:42.0)
  Gecko/20100101 Firefox/42.0\n" "Accept:
  text/html,application/xhtml+xml,application/xml;q=0.9,* /*;q=0.8\n"
     "Accept-Language: en-US,en;q=0.5\n"
     "Accept-Encoding: gzip, deflate\n"
     "Connection: keep-alive\n"
     "Content-Type:  application/json\n"
     "Cache-Control: max-age=0\r\n"
     "{"
     "    \"id\": 1,\n"
     "    \"name\": \"A green door\",\n"
     "    \"price\": 12.50,\n"
     "    \"tags\": [\"home\", \"green\"]\n"
     "}"
  );*/
  strcpy(str,
         "GET scheme://user:password@host:5564/path?query#fragment HTTP/1.1\r\n"
         //"GET /tefmock/comanda HTTP/1.1\r\n"
         //"POST /tefmock/comanda HTTP/1.1\r\n"
         "Host: 127.0.0.1:8085\r\n"
         "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:42.0) "
         "Gecko/20100101 Firefox/42.0\r\n"
         "Accept: "
         "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
         "Accept-Language: en-US,en;q=0.5\r\n"
         "Accept-Encoding: gzip, deflate\r\n"
         "Content-Length: 23\r\n"
         "SessionID: "
         "f4202e3c5852f9182a0430fd8144f0a74b95e7417ecae17db0f8cfeed0e3e66e\r\n"
         "Connection: keep-alive\r\n\r\n<html>body legal</html>");
  log_debug("Scanner version = %lu", scanner_plugin.p_version());

  log_debug("input:%s", str);

  output = NULL;
  ret = scanner_plugin.p_scan(
    scanner, "servico SMS", str, strlen(str), &path, &output, 0);

  log_debug("scanner [%d:%s]", ret, scanner_plugin.p_str_err(ret));
  if (ret == HPE_OK) {
    if (output)
      if (msg.ParseFromString(output))
        ListRequests(msg);
  } else
    log_info("parsing failure");
  if (path)
    log_info("found path[%s]", path);

  scanner_plugin.p_destroy(scanner);
  free(str);
  if (output)
    free(output);
  if (path)
    free(path);

  log_debug("finalized");
  google::protobuf::ShutdownProtobufLibrary();
  return 0;
}
