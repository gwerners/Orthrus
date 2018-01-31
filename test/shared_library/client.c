
//#define USE_UV_PLUGIN
#include <stdio.h>
#ifndef USE_UV_PLUGIN
#include <dlfcn.h>
#endif
#include <uv.h>

#include "library.h"
int main(int argc,char**argv)
{
   void (*p_init_sample)(const char*);
   void (*p_print_sample)();
   void (*p_remove_sample)();
#ifndef USE_UV_PLUGIN

   void* handle = dlopen("./library.so", RTLD_LAZY);

   p_init_sample=(void (*)(const char*))dlsym(handle, "init_sample");
   p_print_sample=(void (*)())dlsym(handle, "print_sample");
   p_remove_sample=(void (*)())dlsym(handle, "remove_sample");
#else
   uv_lib_t *lib = (uv_lib_t*) malloc(sizeof(uv_lib_t));

     printf("Loading %s\n", "./library.so");
     if (uv_dlopen("./library.so", lib)) {
         fprintf(stderr, "Error: %s\n", uv_dlerror(lib));
         return 0;
     }

     if (uv_dlsym(lib, "init_sample", (void **) &p_init_sample)) {
         fprintf(stderr, "dlsym error: %s\n", uv_dlerror(lib));
         return 0;
     }
     if (uv_dlsym(lib, "print_sample", (void **) &p_print_sample)) {
         fprintf(stderr, "dlsym error: %s\n", uv_dlerror(lib));
         return 0;
     }
     if (uv_dlsym(lib, "remove_sample", (void **) &p_remove_sample)) {
         fprintf(stderr, "dlsym error: %s\n", uv_dlerror(lib));
         return 0;
     }
#endif
   p_init_sample("xx contenha algo!");
   p_print_sample();
   p_remove_sample();

   printf("all samples are ok\n");
   return 0;
}

