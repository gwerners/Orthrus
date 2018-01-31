#ifndef library_h
#define library_h
#ifdef __cplusplus

//#define USE_UV_PLUGIN

#ifndef USE_UV_PLUGIN
#include <dlfcn.h>
#else
#include <uv.h>
#endif


#define SCAN_ERROR_MISSING_METHOD    -1
#define SCAN_ERROR_MISSING_URI       -2
#define SCAN_ERROR_MISSING_VERSION   -3
#define SCAN_ERROR_MISSING_PARAMETER -4
#define SCAN_ERROR_MISSING_BODY      -5

#define string_missing_method    "metodo nao encontrado!"
#define string_missing_uri       "uri nao encontrado!"
#define string_missing_version   "version nao encontrado!"
#define string_missing_parameter "parametro nao encontrado!"
#define string_missing_body      "body nao encontrado!"

extern "C" {

typedef struct {
   const char*begin;
   const char*end;
}Position;

struct ScannerPlugin
{
#ifndef USE_UV_PLUGIN
   void* handle;
#else
   uv_lib_t *handle;
#endif
   const char* (*p_StrScanErr)(int error);
   int (*p_Scanner)(const char * input,const char*limit,char**path,char **output, int olen );

};
#endif

const char*StrScanErr(int error);
int Scanner(const char * input,const char*limit,char **output, int olen );


#ifdef __cplusplus
}
#endif
#endif

