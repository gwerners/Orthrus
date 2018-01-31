
//#define USE_UV_PLUGIN
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef USE_UV_PLUGIN
#include <dlfcn.h>
#endif
#include <uv.h>

#include "library.h"
struct ScreenPlugin
{
#ifndef USE_UV_PLUGIN
   void* handle;
#else
   uv_lib_t *handle;
#endif
   void (*p_init_sample)(const char*);
   void (*p_print_sample)();
   void (*p_remove_sample)();
};
struct ScreenPlugin gsplugin;
//"./library.so"
void load_plugin(const char*name)
{
#ifndef USE_UV_PLUGIN
   gsplugin.handle = dlopen(name, RTLD_LAZY);
   gsplugin.p_init_sample=(void (*)(const char*))dlsym(gsplugin.handle, "init_sample");
   gsplugin.p_print_sample=(void (*)())dlsym(gsplugin.handle, "print_sample");
   gsplugin.p_remove_sample=(void (*)())dlsym(gsplugin.handle, "remove_sample");
#else
   gsplugin.handle = (uv_lib_t*) malloc(sizeof(uv_lib_t));

     printf("Loading %s\n", name);
     if (uv_dlopen(name, handle)) {
         fprintf(stderr, "Error: %s\n", uv_dlerror(handle));
         return 0;
     }

     if (uv_dlsym(handle, "init_sample", (void **) &gsplugin.p_init_sample)) {
         fprintf(stderr, "dlsym error: %s\n", uv_dlerror(handle));
         return 0;
     }
     if (uv_dlsym(handle, "print_sample", (void **) &gsplugin.p_print_sample)) {
         fprintf(stderr, "dlsym error: %s\n", uv_dlerror(handle));
         return 0;
     }
     if (uv_dlsym(handle, "remove_sample", (void **) &gsplugin.p_remove_sample)) {
         fprintf(stderr, "dlsym error: %s\n", uv_dlerror(handle));
         return 0;
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
void read_input()
{
   int fim=0,ok;
   char *buffer;//basic buffer
   ssize_t read;
   size_t len;
   do{
      buffer = NULL;
      read = getline(&buffer, &len, stdin);
      if (-1 != read){
         len=strlen(buffer);
         buffer[len-1]='\0';
         printf("readed[%s]\n",buffer);
         if(buffer[0]=='q')fim=1;
         if(buffer[0]=='L')
         {
            printf("loading new library[%s]\n",(char*)&buffer[1]);
            ok=LibraryExists((char*)&buffer[1]);
            if(ok)
            {
#ifndef USE_UV_PLUGIN
            dlclose(gsplugin.handle);
#else
            uv_dlclose(gsplugin.handle);
#endif
            load_plugin((char*)&buffer[1]);
            }else
               printf("loading new library[%s] FAILED\n",(char*)&buffer[1]);
         }

         gsplugin.p_init_sample("xx contenha algo!");
         gsplugin.p_print_sample();
         gsplugin.p_remove_sample();
      }else
         printf("No line read...\n");

      printf("Size read: %lu\n Len: %lu\n", read, len);

   }while(!fim);
   free(buffer);

}
int main(int argc,char**argv)
{
   load_plugin("./library_A.so");//load default plugin
   read_input();


   printf("all samples are ok\n");
   return 0;
}

