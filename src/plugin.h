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
  uv_lib_t* handle;
#endif
  unsigned long (*p_version)(void);
  void* (*p_init)(int type);
  void (*p_destroy)(void* p);
  int (*p_scan)(void* p,
                const char* name,
                const char* input,
                size_t ilen,
                char** path,
                char** output,
                size_t olen);
  const char* (*p_str_err)(int err);
};

extern struct ScannerPlugin scanner_plugin;
}
unsigned long
version_dummy(void);
void*
init_dummy(int type);
void
destroy_dummy(void* p);
int
scan_dummy(void* p,
           const char* name,
           const char* input,
           size_t ilen,
           char** path,
           char** output,
           size_t olen);
const char*
str_err_dummy(int err);

void
LoadScannerPluginDummy();
void
LoadScannerPlugin(const char* name);

int
LibraryExists(const char* filename);
int
LoadPlugin(const char* libname, void (*Load)(const char*), void (*LoadDummy)());

#endif
