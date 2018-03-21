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
#ifndef library_h
#define library_h
#ifdef __cplusplus

//#define USE_UV_PLUGIN

#ifndef USE_UV_PLUGIN
#include <dlfcn.h>
#else
#include <uv.h>
#endif

#define SCAN_ERROR_MISSING_METHOD -1
#define SCAN_ERROR_MISSING_URI -2
#define SCAN_ERROR_MISSING_VERSION -3
#define SCAN_ERROR_MISSING_PARAMETER -4
#define SCAN_ERROR_MISSING_BODY -5

#define string_missing_method "metodo nao encontrado!"
#define string_missing_uri "uri nao encontrado!"
#define string_missing_version "version nao encontrado!"
#define string_missing_parameter "parametro nao encontrado!"
#define string_missing_body "body nao encontrado!"

extern "C" {

typedef struct
{
  const char* begin;
  const char* end;
} Position;

struct ScannerPlugin
{
#ifndef USE_UV_PLUGIN
  void* handle;
#else
  uv_lib_t* handle;
#endif
  const char* (*p_StrScanErr)(int error);
  int (*p_Scanner)(const char* input,
                   const char* limit,
                   char** path,
                   char** output,
                   int olen);
};
#endif

const char*
StrScanErr(int error);
int
Scanner(const char* input, const char* limit, char** output, int olen);

#ifdef __cplusplus
}
#endif
#endif
