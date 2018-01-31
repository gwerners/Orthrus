
#include <stdio.h>
#include <string.h>

int scan (const char * cursor, const char * const limit)
{
    const char * marker;
    const char * ctxmarker;
#   define YYCTYPE        char
#   define YYPEEK()       (cursor >= limit ? 0 : *cursor)
#   define YYSKIP()       ++cursor
#   define YYBACKUP()     marker = cursor
#   define YYBACKUPCTX()  ctxmarker = cursor
#   define YYRESTORE()    cursor = marker
#   define YYRESTORECTX() cursor = ctxmarker
    /*!re2c

      OCTET          = [\000-\377];
      CHAR           = [\000-\177];
      UPALPHA        = [A-Z];
      LOALPHA        = [a-z];
      ALPHA          = UPALPHA | LOALPHA;
      DIGIT          = [0-9];
      CTL            = [\000-\037] | [\177];
      CR             = [\015];
      LF             = [\012];
      SP             = [\040];
      HT             = [\011];
      DQUOTE         = [\042];

      CRLF           = CR LF;

      LWS            = CRLF? ( SP | HT )+;



      HEX            = [A-F] | [a-f] | DIGIT;

      SEPARATORS     = [()<>@,;:\\"/\[\]?={}] | SP | HT;

      TEXT           = [\015] | [\012] | [\011] | [\040-\176] | [\200-\377];

      HTTP_Version   = "HTTP" "/" DIGIT+ "." DIGIT+ ;
      re2c:yyfill:enable = 0;

      ""           { return 0; }
      HEX*         { return 1; }
      SEPARATORS*  { return 3; }
      HTTP_Version { return 4; }
      TEXT*        { return 2; }
      
    */
//      
//
}

/*

   +---------+-------------------------------------------------+-------+
   | Method  | Description                                     | Sec.  |
   +---------+-------------------------------------------------+-------+
   | GET     | Transfer a current representation of the target | 4.3.1 |
   |         | resource.                                       |       |
   | HEAD    | Same as GET, but only transfer the status line  | 4.3.2 |
   |         | and header section.                             |       |
   | POST    | Perform resource-specific processing on the     | 4.3.3 |
   |         | request payload.                                |       |
   | PUT     | Replace all current representations of the      | 4.3.4 |
   |         | target resource with the request payload.       |       |
   | DELETE  | Remove all current representations of the       | 4.3.5 |
   |         | target resource.                                |       |
   | CONNECT | Establish a tunnel to the server identified by  | 4.3.6 |
   |         | the target resource.                            |       |
   | OPTIONS | Describe the communication options for the      | 4.3.7 |
   |         | target resource.                                |       |
   | TRACE   | Perform a message loop-back test along the path | 4.3.8 |
   |         | to the target resource.                         |       |
   +---------+-------------------------------------------------+-------+
*/
int scan2(const char * cursor, const char * const limit)
{
    const char * marker;
    const char * ctxmarker;
    int method=0;
#   define YYCTYPE        char
#   define YYPEEK()       (cursor >= limit ? 0 : *cursor)
#   define YYSKIP()       ++cursor
#   define YYBACKUP()     marker = cursor
#   define YYBACKUPCTX()  ctxmarker = cursor
#   define YYRESTORE()    cursor = marker
#   define YYRESTORECTX() cursor = ctxmarker
    /*!re2c
      re2c:yyfill:enable = 0;
      ""        { method = 0;goto lbl_method_error;}
      "GET"     { method = 1;goto lbl_uri;}
      "HEAD"    { method = 2;goto lbl_uri;}
      "POST"    { method = 3;goto lbl_uri;}
      "PUT"     { method = 4;goto lbl_uri;}
      "DELETE"  { method = 5;goto lbl_uri;}
      "CONNECT" { method = 6;goto lbl_uri;}
      "OPTIONS" { method = 7;goto lbl_uri;}
      "TRACE"   { method = 8;goto lbl_uri;}

*/
lbl_method_error:
   if(!method)
   {
      printf("metodo nao encontrado!\n");
      return 0;
   }
lbl_uri:
//Request-URI    = "*" | absoluteURI | abs_path | authority
//example :
//OPTIONS * HTTP/1.1
//GET http://www.w3.org/pub/WWW/TheProject.html HTTP/1.1
   if(*cursor==' ')++cursor;
   else return -2;

   //parse uri
   while(*cursor!='\0' && *cursor!=' ') ++cursor;

   if(*cursor==' ')++cursor;
   else return -3;

    /*!re2c
      re2c:yyfill:enable = 0;
      ""           { return -1;}
      HTTP_Version { return  4;}
    */
   return -2;
}
/*

       request-header = Accept                   ; Section 14.1
                      | Accept-Charset           ; Section 14.2
                      | Accept-Encoding          ; Section 14.3
                      | Accept-Language          ; Section 14.4
                      | Authorization            ; Section 14.8
                      | Expect                   ; Section 14.20
                      | From                     ; Section 14.22
                      | Host                     ; Section 14.23
                      | If-Match                 ; Section 14.24
                      | If-Modified-Since        ; Section 14.25
                      | If-None-Match            ; Section 14.26
                      | If-Range                 ; Section 14.27
                      | If-Unmodified-Since      ; Section 14.28
                      | Max-Forwards             ; Section 14.31
                      | Proxy-Authorization      ; Section 14.34
                      | Range                    ; Section 14.35
                      | Referer                  ; Section 14.36
                      | TE                       ; Section 14.39
                      | User-Agent               ; Section 14.43


      Status-Code    =
            "100"  ; Section 10.1.1: Continue
          | "101"  ; Section 10.1.2: Switching Protocols
          | "200"  ; Section 10.2.1: OK
          | "201"  ; Section 10.2.2: Created
          | "202"  ; Section 10.2.3: Accepted
          | "203"  ; Section 10.2.4: Non-Authoritative Information
          | "204"  ; Section 10.2.5: No Content
          | "205"  ; Section 10.2.6: Reset Content
          | "206"  ; Section 10.2.7: Partial Content
          | "300"  ; Section 10.3.1: Multiple Choices
          | "301"  ; Section 10.3.2: Moved Permanently
          | "302"  ; Section 10.3.3: Found
          | "303"  ; Section 10.3.4: See Other
          | "304"  ; Section 10.3.5: Not Modified
          | "305"  ; Section 10.3.6: Use Proxy
          | "307"  ; Section 10.3.8: Temporary Redirect
          | "400"  ; Section 10.4.1: Bad Request
          | "401"  ; Section 10.4.2: Unauthorized
          | "402"  ; Section 10.4.3: Payment Required
          | "403"  ; Section 10.4.4: Forbidden
          | "404"  ; Section 10.4.5: Not Found
          | "405"  ; Section 10.4.6: Method Not Allowed
          | "406"  ; Section 10.4.7: Not Acceptable
          | "407"  ; Section 10.4.8: Proxy Authentication Required
          | "408"  ; Section 10.4.9: Request Time-out
          | "409"  ; Section 10.4.10: Conflict
          | "410"  ; Section 10.4.11: Gone
          | "411"  ; Section 10.4.12: Length Required
          | "412"  ; Section 10.4.13: Precondition Failed
          | "413"  ; Section 10.4.14: Request Entity Too Large
          | "414"  ; Section 10.4.15: Request-URI Too Large
          | "415"  ; Section 10.4.16: Unsupported Media Type
          | "416"  ; Section 10.4.17: Requested range not satisfiable
          | "417"  ; Section 10.4.18: Expectation Failed
          | "500"  ; Section 10.5.1: Internal Server Error
          | "501"  ; Section 10.5.2: Not Implemented
          | "502"  ; Section 10.5.3: Bad Gateway
          | "503"  ; Section 10.5.4: Service Unavailable
          | "504"  ; Section 10.5.5: Gateway Time-out
          | "505"  ; Section 10.5.6: HTTP Version not supported
          | extension-code

      extension-code = 3DIGIT
      Reason-Phrase  = *<TEXT, excluding CR, LF>

       response-header = Accept-Ranges           ; Section 14.5
                       | Age                     ; Section 14.6
                       | ETag                    ; Section 14.19
                       | Location                ; Section 14.30
                       | Proxy-Authenticate      ; Section 14.33
                       | Retry-After             ; Section 14.37
                       | Server                  ; Section 14.38
                       | Vary                    ; Section 14.44
                       | WWW-Authenticate        ; Section 14.47
*/
int main ()
{
//
//
   int ret;
   char *str = (char*)malloc(1024);

   strcpy(str,"Agora vai\t.Se voce espera mais, alcanca\nmais!");
   ret=scan (str, str+strlen(str));
   switch(ret)
   {
      case 0:printf("no text found\n");break;
      case 1:printf("hex match\n");break;
      case 2:printf("text match\n");break;
      case 3:printf("separators match\n");break;
      case 4:printf("http match\n");break;
   }

   strcpy(str,"");
   ret=scan (str, str+strlen(str) );
   switch(ret)
   {
      case 0:printf("no text found\n");break;
      case 1:printf("hex match\n");break;
      case 2:printf("text match\n");break;
      case 3:printf("separators match\n");break;
      case 4:printf("http match\n");break;
   }

  strcpy(str,"Affacb1234ffFaEeeee");
   ret=scan (str, str+strlen(str) );
   switch(ret)
   {
      case 0:printf("no text found\n");break;
      case 1:printf("hex match\n");break;
      case 2:printf("text match\n");break;
      case 3:printf("separators match\n");break;
      case 4:printf("http match\n");break;
   }

   strcpy(str,"{ } ( ) < > @ , ; : \\ \" / [ ] ? = ");
   ret=scan (str, str+strlen(str));
   switch(ret)
   {
      case 0:printf("no text found\n");break;
      case 1:printf("hex match\n");break;
      case 2:printf("text match\n");break;
      case 3:printf("separators match\n");break;
      case 4:printf("http match\n");break;
   }

   strcpy(str,"HTTP/1.1");
   ret=scan(str, str+strlen(str));
   switch(ret)
   {
      case 0:printf("no text found\n");break;
      case 1:printf("hex match\n");break;
      case 2:printf("text match\n");break;
      case 3:printf("separators match\n");break;
      case 4:printf("http match\n");break;
   }


   strcpy(str,"GET /index.html HTTP/1.1");
   ret=scan2(str, str+strlen(str));
   printf("scan2 [%d]\n",ret);
   switch(ret)
   {
      case 0:printf("no text found\n");break;
      case -1:printf("error !!\n");break;
      case 4:printf("http method match\n");break;
   }
   free(str);
   return 0;
}



