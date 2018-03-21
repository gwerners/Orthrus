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

#include <fstream>
#include <iostream>
#include <string>

#include "logger.h"
#include "plugin.h"

extern "C" {
struct ScannerPlugin scanner_plugin;
}

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
  log_debug("loading [%s]\n", name);
  scanner_plugin.handle = dlopen(name, RTLD_LAZY);
  if (!scanner_plugin.handle) {
    log_debug("unable to load [%s] due to [%s]\n", name, dlerror());
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

  log_debug("scanner_plugin.handle[%p]\n", scanner_plugin.handle);
  log_debug("scanner_plugin.version[%p]\n", scanner_plugin.p_version);
  fflush(stdout);
#else
  scanner_plugin.handle = (uv_lib_t*)malloc(sizeof(uv_lib_t));

  log_debug("Loading %s\n", name);
  if (uv_dlopen(name, scanner_plugin.handle)) {
    fprintf(stderr, "Error: %s\n", uv_dlerror(scanner_plugin.handle));
    return;
  }
  if (uv_dlsym(
        scanner_plugin.handle, "version", (void**)&scanner_plugin.p_version)) {
    fprintf(stderr, "dlsym error: %s\n", uv_dlerror(scanner_plugin.handle));
    return;
  }
  if (uv_dlsym(scanner_plugin.handle, "init", (void**)&scanner_plugin.p_init)) {
    fprintf(stderr, "dlsym error: %s\n", uv_dlerror(scanner_plugin.handle));
    return;
  }
  if (uv_dlsym(
        scanner_plugin.handle, "destroy", (void**)&scanner_plugin.p_destroy)) {
    fprintf(stderr, "dlsym error: %s\n", uv_dlerror(scanner_plugin.handle));
    return;
  }
  if (uv_dlsym(scanner_plugin.handle, "scan", (void**)&scanner_plugin.p_scan)) {
    fprintf(stderr, "dlsym error: %s\n", uv_dlerror(scanner_plugin.handle));
    return;
  }
  if (uv_dlsym(
        scanner_plugin.handle, "str_err", (void**)&scanner_plugin.p_str_err)) {
    fprintf(stderr, "dlsym error: %s\n", uv_dlerror(scanner_plugin.handle));
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
  if (buf.st_mode & S_IXUSR) // check execute permission!
    return 1;
  return 0;
}

int
LoadPlugin(const char* libname, void (*Load)(const char*), void (*LoadDummy)())
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
    Load(libname);
    return 1;
  } else {
    log_error("loading library[%s] FAILED", libname);
    LoadDummy();
    return 0;
  }
}
