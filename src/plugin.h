#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#define USE_UV_PLUGIN

#ifndef USE_UV_PLUGIN
#include <dlfcn.h>
#else
#include <uv.h>
#endif

extern "C" {

struct ScannerPlugin
{
#ifndef USE_UV_PLUGIN
   void* handle;
#else
   uv_lib_t *handle;
#endif
   unsigned long (*p_version)(void);
   void* (*p_init)(int type);
   void (*p_destroy)(void *p);
   int (*p_scan)(void *p,const char*name,const char * input,size_t ilen,char **path,char **output, size_t olen );
   const char*(*p_str_err)(int err);
};

extern struct ScannerPlugin scanner_plugin;
}
unsigned long version_dummy(void);
void* init_dummy(int type);
void destroy_dummy(void *p);
int scan_dummy(void *p,const char*name,const char * input,size_t ilen,char **path,char **output, size_t olen );
const char* str_err_dummy(int err);

void LoadScannerPluginDummy();
void LoadScannerPlugin(const char*name);

int LibraryExists(const char*filename);
int LoadPlugin(const char*libname,void(*Load)(const char*),void(*LoadDummy)());

#endif
