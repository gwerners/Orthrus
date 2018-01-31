
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include <iostream>
#include <fstream>
#include <string>
#include "request.pb.h"
#include "libM4UI-Rest-1.0.h"
#include "http_parser.h"

#define COUT_ME(name) cout << " " << #name << ": " << request.name() << endl;


using namespace std;

struct ScannerPlugin scanner_plugin;

void ListRequests(const m4u_interfaces::MessageBlock& msg)
{
/*
   string name            = 1;
   string raw             = 2;
   int32  type            = 3;
   uint32 method          = 4;
   int32  status_code     = 5;
   string response_status = 6;
   string request_path    = 7;
   string request_url     = 8;
   string fragment        = 9;
   string query_string    = 10;
   string body            = 11;
   uint64 body_size       = 12;
   string host            = 13;
   string userinfo        = 14;
   uint32 port            = 15;
   int32  num_headers     = 16;
   int32  last_header_element = 17;
   int32   should_keep_alive  = 18;

  message Chunk {
      int32 complete = 1;
      int32 length   = 2;
  }
  repeated Chunk chunks = 19;

  string upgrade        = 20;
  uint32 http_major     = 21;
  uint32 http_minor     = 22;

   message Headers {
      string key = 1;
      string value = 2;
   }
   repeated Headers headers = 23;
*/
   for (int i = 0; i < msg.request_size(); i++) {
      const m4u_interfaces::Request& request = msg.request(i);
      cout << "###########################################" << endl;
      cout << " name: " << request.name() << endl;
      COUT_ME(raw)
      COUT_ME(type)
      COUT_ME(method)
      COUT_ME(status_code)
      COUT_ME(response_status)
      COUT_ME(request_path)
      COUT_ME(request_url)
      COUT_ME(fragment)
      COUT_ME(query_string)
      COUT_ME(body)
      COUT_ME(body_size)
      COUT_ME(host)
      COUT_ME(userinfo)
      COUT_ME(port)
      COUT_ME(num_headers)
      COUT_ME(last_header_element)
      COUT_ME(should_keep_alive)
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
      if(request.chunks_size()>0)
      {
         cout << " Chunks : "<<endl;
         for (int j = 0; j < request.chunks_size(); j++) {
            const m4u_interfaces::Request::Chunk& chunk = request.chunks(j);
            cout << "  " <<chunk.length() << endl;
         }
      }
/*
upgrade
http_major
http_minor
*/
      COUT_ME(upgrade)
      COUT_ME(http_major)
      COUT_ME(http_minor)

      if(request.headers_size()>0)
      {
         cout << " Headers : "<<endl;
         for (int j = 0; j < request.headers_size(); j++) {
            const m4u_interfaces::Request::Header& header = request.headers(j);

            cout << "  " << header.key() << ":" <<header.value() << endl;
         }
      }
   }
}
/*
   unsigned long (*p_version)(void);
   void* (*p_init)(int type);
   void (*p_destroy)(void *p);
   int (*p_scan)(const char * input,size_t ilen,char **output, size_t olen );
   const char*(*p_str_err)(int err);
*/
unsigned long version_dummy(void)
{
   return 0;
}
void* init_dummy(int type)
{
   return NULL;
}
void destroy_dummy(void *p)
{
   return;
}
int scan_dummy(void *p,const char * input,size_t ilen,char **output, size_t olen )
{
   if(output && *output)
      *output=NULL;
   return 0;
}
const char* str_err_dummy(int err)
{
   return "error dummy";
}

void LoadScannerPluginDummy()
{
   scanner_plugin.handle = NULL;

   scanner_plugin.p_version=(unsigned long (*)(void))version_dummy;
   scanner_plugin.p_init=(void* (*)(int))init_dummy;
   scanner_plugin.p_destroy=(void (*)(void *))destroy_dummy;
   scanner_plugin.p_scan=(int (*)(void *,const char *,size_t,char **, size_t))scan_dummy;
   scanner_plugin.p_str_err=(const char*(*)(int))str_err_dummy;
}

void LoadScannerPlugin(const char*name)
{
   memset(&scanner_plugin,0,sizeof(scanner_plugin));
#ifndef USE_UV_PLUGIN
   printf("loading [%s]\n",name);
   scanner_plugin.handle = dlopen(name, RTLD_LAZY);
   if(!scanner_plugin.handle)
   {
      printf("unable to load [%s] due to [%s]\n",name,dlerror());
   }
   scanner_plugin.p_version=(unsigned long (*)(void)) dlsym(scanner_plugin.handle,"version");
   scanner_plugin.p_init=(void* (*)(int)) dlsym(scanner_plugin.handle, "init");
   scanner_plugin.p_destroy=(void (*)(void *)) dlsym(scanner_plugin.handle,"destroy");
   scanner_plugin.p_scan=(int (*)(void *,const char *,size_t,char **, size_t)) dlsym(scanner_plugin.handle, "scan");
   scanner_plugin.p_str_err=(const char*(*)(int)) dlsym(scanner_plugin.handle, "str_err");

   printf("scanner_plugin.handle[%p]\n",scanner_plugin.handle);
   printf("scanner_plugin.version[%p]\n",scanner_plugin.p_version);fflush(stdout);
#else
   scanner_plugin.handle = (uv_lib_t*) malloc(sizeof(uv_lib_t));

   printf("Loading %s\n", name);
   if (uv_dlopen(name,scanner_plugin.handle)) {
      fprintf(stderr, "Error: %s\n", uv_dlerror(scanner_plugin.handle));
      return ;
   }
   if (uv_dlsym(scanner_plugin.handle, "version", (void **) &scanner_plugin.p_version)) {
      fprintf(stderr, "dlsym error: %s\n", uv_dlerror(scanner_plugin.handle));
      return ;
   }
   if (uv_dlsym(scanner_plugin.handle, "init", (void **) &scanner_plugin.p_init)) {
      fprintf(stderr, "dlsym error: %s\n", uv_dlerror(scanner_plugin.handle));
      return ;
   }
   if (uv_dlsym(scanner_plugin.handle, "destroy", (void **) &scanner_plugin.p_destroy)) {
      fprintf(stderr, "dlsym error: %s\n", uv_dlerror(scanner_plugin.handle));
      return ;
   }
   if (uv_dlsym(scanner_plugin.handle, "scan", (void **) &scanner_plugin.p_scan)) {
      fprintf(stderr, "dlsym error: %s\n", uv_dlerror(scanner_plugin.handle));
      return ;
   }
   if (uv_dlsym(scanner_plugin.handle, "str_err", (void **) &scanner_plugin.p_str_err)) {
      fprintf(stderr, "dlsym error: %s\n", uv_dlerror(scanner_plugin.handle));
      return ;
   }
#endif
}
int LibraryExists(const char*filename)
{
   int ret;
   struct stat buf;
   ret=stat(filename, &buf);
   if(ret==-1)
      return 0;
   if(buf.st_mode & S_IXUSR) //execute permission!
      return 1;
   return 0;
}
void LoadPlugin(const char*libname)
{
   int ok;
   ok=LibraryExists(libname);
   if(ok)
   {
      printf("library exists\n");
#ifndef USE_UV_PLUGIN
      if(scanner_plugin.handle)
         dlclose(scanner_plugin.handle);
#else
      if(scanner_plugin.handle)
         uv_dlclose(scanner_plugin.handle);
#endif
      LoadScannerPlugin(libname);
   }else{
      printf("loading new library[%s] FAILED\n",libname);
      LoadScannerPluginDummy();
   }
}

int main ()
{
   int ret;
   char *str,*output;
   GOOGLE_PROTOBUF_VERIFY_VERSION;
   m4u_interfaces::MessageBlock msg;

   //load plugin
   LoadPlugin("libM4UI-Rest-1.0.so");
   void *scanner = scanner_plugin.p_init(HTTP_REQUEST);

   str=(char*)malloc(1024);
   /*strcpy(str,
      "GET /path/cgi.script HTTP/1.1\n"
      "Host: 127.0.0.1:7000\n"
      "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:42.0) Gecko/20100101 Firefox/42.0\n"
      "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,* /*;q=0.8\n"
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
      "GET /tefmock/comanda HTTP/1.1\r\n"
      "Host: 127.0.0.1:8085\r\n"
      "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:42.0) Gecko/20100101 Firefox/42.0\r\n"
      "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
      "Accept-Language: en-US,en;q=0.5\r\n"
      "Accept-Encoding: gzip, deflate\r\n"
      "Connection: keep-alive\r\n\r\n"
   );
   cout << "Scanner version = "<< scanner_plugin.p_version() << endl;

   printf("input:\n%s\n",str);

   output=NULL;
   ret=scanner_plugin.p_scan(scanner,str,strlen(str),&output,0);

   printf("scanner [%d:%s]\n",ret,scanner_plugin.p_str_err(ret));
   if(ret==HPE_OK)
   {
      if(msg.ParseFromString(output))
         ListRequests(msg);
   }else
      printf("parsing failure\n");

   scanner_plugin.p_destroy(scanner);
   free(str);
   if(output)free(output);

   cout << "finalized" << endl;
   google::protobuf::ShutdownProtobufLibrary();
   return 0;
}


