
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

using namespace std;

struct ScannerPlugin scanner_plugin;

void ListRequests(const m4u_interfaces::MessageBlock& msg)
{
   for (int i = 0; i < msg.request_size(); i++) {
      const m4u_interfaces::Request& request = msg.request(i);
      cout << "###########################################" << endl;
      cout << " path: " << request.path() << endl;

      cout << " Method : ";
      switch (request.type()) {
         case m4u_interfaces::Request::GET:
            cout << "GET" << endl;
            break;
         case m4u_interfaces::Request::HEAD:
            cout << "HEAD" << endl;
            break;
         case m4u_interfaces::Request::POST:
            cout << "POST" << endl;
            break;
         case m4u_interfaces::Request::PUT:
            cout << "PUT" << endl;
            break;
         case m4u_interfaces::Request::DELETE:
            cout << "DELETE" << endl;
            break;
         case m4u_interfaces::Request::CONNECT:
            cout << "CONNECT" << endl;
            break;
         case m4u_interfaces::Request::OPTIONS:
            cout << "OPTIONS" << endl;
            break;
         case m4u_interfaces::Request::TRACE:
            cout << "TRACE" << endl;
            break;
      }

      if(request.params_size()>0)
      {
         cout << " Parameters : "<<endl;
         for (int j = 0; j < request.params_size(); j++) {
            const m4u_interfaces::Request::Parameters& parameter = request.params(j);

            cout << "  " << parameter.key() << ":" <<parameter.value() << endl;
         }
      }
      cout << " body: " << request.body() << endl;
   }
}
const char* StrScanErrDummy(int error)
{
   return "";
}
int ScannerDummy(const char * input,const char* limit,char **output, int olen )
{
   *output=strdup("");
   return 0;
}
void LoadScannerPluginDummy()
{
   scanner_plugin.handle = NULL;
   scanner_plugin.p_Scanner=(int (*)(const char *,const char*,char **,int))ScannerDummy;
   scanner_plugin.p_StrScanErr=(const char* (*)(int ))StrScanErrDummy;
}
void LoadScannerPlugin(const char*name)
{
   memset(&scanner_plugin,0,sizeof(scanner_plugin));
#ifndef USE_UV_PLUGIN
   //printf("loading [%s]\n",name);
   scanner_plugin.handle = dlopen(name, RTLD_LAZY);
   /*if(!scanner_plugin.handle)
   {
      printf("unable to load [%s] due to [%s]\n",name,dlerror());
   }*/
   //printf("scanner_plugin.handle[%p]\n",scanner_plugin.handle);
   scanner_plugin.p_Scanner=(int (*)(const char *,const char*,char **,int))dlsym(scanner_plugin.handle, "Scanner");
   //printf("scanner_plugin.Scanner[%p]\n",scanner_plugin.p_Scanner);
   scanner_plugin.p_StrScanErr=(const char* (*)(int ))dlsym(scanner_plugin.handle, "StrScanErr");
   //printf("scanner_plugin.StrScanErr[%p]\n",scanner_plugin.p_StrScanErr);fflush(stdout);
#else
   scanner_plugin.handle = (uv_lib_t*) malloc(sizeof(uv_lib_t));

   printf("Loading %s\n", name);
   if (uv_dlopen(name,scanner_plugin.handle)) {
      fprintf(stderr, "Error: %s\n", uv_dlerror(scanner_plugin.handle));
      return ;
   }

   if (uv_dlsym(scanner_plugin.handle, "Scanner", (void **) &scanner_plugin.p_Scanner)) {
      fprintf(stderr, "dlsym error: %s\n", uv_dlerror(scanner_plugin.handle));
      return ;
   }
   if (uv_dlsym(scanner_plugin.handle, "StrScanErr", (void **) &scanner_plugin.p_StrScanErr)) {
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
int Scanner(const char * input,const char*limit,char **output, int olen );
int main ()
{
   int ret;
   char *str,*output;
   GOOGLE_PROTOBUF_VERIFY_VERSION;
   m4u_interfaces::MessageBlock msg;

   //load plugin
   LoadPlugin("libM4UI-Rest-1.0.so");


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

   printf("input:\n%s\n",str);

   ret=scanner_plugin.p_Scanner(str,str+strlen(str),&output,0);

   printf("scanner [%d:%s]\n",ret,scanner_plugin.p_StrScanErr(ret));
   if(ret)
   {
      if(msg.ParseFromString(output))
         ListRequests(msg);
   }else
      printf("parsing failure\n");

   free(str);free(output);
   google::protobuf::ShutdownProtobufLibrary();
   return 0;
}


