

#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
#include "request.pb.h"
#include "libM4UI-Rest-1.0.h"

//DEFINES

#define _DEBUG_ME_

#define YYCTYPE        char
#define YYPEEK()       (cursor >= limit ? 0 : *cursor)
#define YYSKIP()       ++cursor
#define YYBACKUP()     marker = cursor
#define YYBACKUPCTX()  ctxmarker = cursor
#define YYRESTORE()    cursor = marker
#define YYRESTORECTX() cursor = ctxmarker

extern "C"
{

static int InternalScanner(
interfaces::Request* request,
const char * cursor, 
const char * const limit,
int *var_method,
int *var_uri,
int *var_version,
Position *uri,
Position *body )
{
   interfaces::Request::Parameters* parameter;
   std::string key,value;
   const char * marker;
   const char * ctxmarker;
   char buffer[256];
   Position variable;
   int var_request_header;
   /*!re2c
      re2c:yyfill:enable = 0;
      CR             = '\r';
      LF             = '\n';
      SP             = ' ';
      HT             = [\011];
      CRLF           = CR LF;
      digit          = [0-9];
      CTL            = [\000-\037] | [\177];
      SEP            = [()<>@,;:\\"/\[\]?={}] | SP | HT;
      ctoken         = [\000-\377] \(CTL | SEP);
      token          = ctoken+;
      Request_URI    = ('/'|'.'|[A-Z]|[a-z]|[0-9])+;
      HTTP_Version   = "HTTP" "/" digit+ "." digit+ ;
      variable_value = ([a-z]|[A-Z]|[0-9]|[+-=,.;* :/()_])*;
      Body           = (ctoken | SEP | CR | LF)*;

   */

   /*!re2c
		""{ *var_method=0;goto lbl_method;}
		"GET"{ *var_method=1;request->set_type(interfaces::Request::GET);goto lbl_method;}
		"HEAD"{ *var_method=2;request->set_type(interfaces::Request::HEAD);goto lbl_method;}
		"POST"{ *var_method=3;request->set_type(interfaces::Request::POST);goto lbl_method;}
		"PUT"{ *var_method=4;request->set_type(interfaces::Request::PUT);goto lbl_method;}
		"DELETE"{ *var_method=5;request->set_type(interfaces::Request::DELETE);goto lbl_method;}
		"CONNECT"{ *var_method=6;request->set_type(interfaces::Request::CONNECT);goto lbl_method;}
		"OPTIONS"{ *var_method=7;request->set_type(interfaces::Request::OPTIONS);goto lbl_method;}
		"TRACE"{ *var_method=8;request->set_type(interfaces::Request::TRACE);goto lbl_method;}
   */
lbl_method:
   if(!*var_method)
   {
      return SCAN_ERROR_MISSING_METHOD;
   }
   /*!re2c
      "" { return -1; }
      SP { goto lbl_space_uri;}
   */
lbl_space_uri:
   uri->begin=cursor;
   uri->end=0;
   /*!re2c
		""{ *var_uri=0;goto lbl_uri;}
		Request_URI{ *var_uri=1;goto lbl_uri;}
   */
lbl_uri:
   if(!*var_uri)
   {
      return SCAN_ERROR_MISSING_URI;
   }
   uri->end=cursor;
   value.assign((char*)uri->begin,uri->end-uri->begin);
   request->set_path(value);
   /*!re2c
      "" { return -2; }
      SP { goto lbl_space_version;}
   */
lbl_space_version:
   /*!re2c
		""{ *var_version=0;goto lbl_version;}
		HTTP_Version{ *var_version=4;goto lbl_version;}
   */
lbl_version:
   if(!*var_version)
   {
      return SCAN_ERROR_MISSING_VERSION;
   }
   /*!re2c
      "" { return -3; }
      CRLF { goto lbl_space_request_header;}
   */
lbl_space_request_header:
   variable.begin=cursor;
   variable.end=0;
   /*!re2c
      CRLF { goto lbl_expect_body;}
		""{ goto lbl_expect_body;}
		"Accept"{ var_request_header=1;key="Accept";goto lbl_request_header;}
		"Accept-Encoding"{ var_request_header=2;key="Accept-Encoding";goto lbl_request_header;}
		"Accept-Language"{ var_request_header=3;key="Accept-Language";goto lbl_request_header;}
		"Cache-Control"{ var_request_header=4;key="Cache-Control";goto lbl_request_header;}
		"Connection"{ var_request_header=5;key="Connection";goto lbl_request_header;}
		"From"{ var_request_header=6;key="From";goto lbl_request_header;}
		"Host"{ var_request_header=7;key="Host";goto lbl_request_header;}
		"User-Agent"{ var_request_header=8;key="User-Agent";goto lbl_request_header;}
		"Content-Type"{ var_request_header=9;key="Content-Type";goto lbl_request_header;}
   */
lbl_request_header:
   if(!var_request_header)
   {
      //return SCAN_ERROR_MISSING_PARAMETER;
      //parameter opcional, pular para body!!!
      goto lbl_expect_body;
   }
   parameter = request->add_params();
   variable.end=cursor;
   if(*cursor==':')++cursor;
#ifdef _DEBUG_ME_
   strncpy(buffer,variable.begin,variable.end-variable.begin);
   buffer[variable.end-variable.begin]='\0';
   printf("variable[%s]\n",buffer);
#endif
   variable.begin=cursor;
   variable.end=0;
   /*!re2c
      "" { printf("position[%s]\n",cursor);return -4; }
      variable_value { }
   */
   variable.end=cursor;
   if(*cursor==':')++cursor;
#ifdef _DEBUG_ME_
   strncpy(buffer,variable.begin,variable.end-variable.begin);
   buffer[variable.end-variable.begin]='\0';
   printf("value[%s]\n",buffer);
#endif
   value.assign((char*)variable.begin,variable.end-variable.begin);
#ifdef _DEBUG_ME_
   std::cout << "Param[" <<key <<"] with value["<<value<<"]"<<std::endl;
#endif
   parameter->set_key(key);
   parameter->set_value(value);
   /*!re2c
      "" { printf(".position[%s]\n",cursor);return 1; }
      CRLF { goto lbl_space_request_header;}
   */

lbl_expect_body:
   body->begin=cursor;
   body->end=0;

   while(*cursor!='\0')++cursor;
   body->end=cursor;

   if(body->begin==cursor)
   {
      body->begin=0;
      body->end=0;
      return SCAN_ERROR_MISSING_BODY;
   }
#ifdef _DEBUG_ME_
   strncpy(buffer,body->begin,body->end-body->begin);
   buffer[body->end-body->begin]='\0';
   printf("body[%s]\n",buffer);
#endif
   value.assign((char*)body->begin,body->end-body->begin);
   request->set_body(value);
   return 1;
}



const char*StrScanErr(int error)
{
   switch(error)
   {
      case 1: return "sucess";
      case SCAN_ERROR_MISSING_METHOD   : return string_missing_method;
      case SCAN_ERROR_MISSING_URI      : return string_missing_uri;
      case SCAN_ERROR_MISSING_VERSION  : return string_missing_version;
      case SCAN_ERROR_MISSING_PARAMETER: return string_missing_parameter;
      case SCAN_ERROR_MISSING_BODY     : return string_missing_body;
   }
   return "invalid error number";
}

int Scanner(const char * input,const char*limit,char **output, int olen )
{
   GOOGLE_PROTOBUF_VERIFY_VERSION;
   Position uri;
   Position body;
   std::string str_output;
   interfaces::MessageBlock msg;
   int ret,var_method,var_uri,var_version;
   ret=InternalScanner(msg.add_request(),input, limit,&var_method,&var_uri,&var_version,&uri,&body);

   if(msg.SerializeToString(&str_output))
   {
      if(!olen)
      {
         *output=(char*)malloc(str_output.length()+1);
         memcpy(*output,str_output.c_str(),str_output.length()+1);
      }
   }
   google::protobuf::ShutdownProtobufLibrary();
   return ret;
}



}
