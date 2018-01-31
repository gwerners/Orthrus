
#include <stdio.h>
#include <string.h>
#include <stdlib.h>




/*
 


 


 rel_path      = rel_segment [ abs_path ]
 
 rel_segment   = 1*( unreserved | escaped | ";" | "@" | "&" | "=" | "+" | "$" | "," )
 
 scheme        = alpha *( alpha | digit | "+" | "-" | "." )
 
 authority     = server | reg_name
 

 








 
 path          = [ abs_path | opaque_part ]
 path_segments = segment *( "/" segment )


 

 
 fragment      = *uric
 
 


 

 
 alphanum      = alpha | digit
 alpha         = lowalpha | upalpha
 
 lowalpha = "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x" | "y" | "z"
 upalpha  = "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z"
 digit    = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
 
 */
typedef struct {
   const char*begin;
   const char*end;
}URL;

URL uri;

//      CR             = [\015];
//      LF             = [\012];
//Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
int parse_request(const char * cursor, const char * const limit)
{
   char str[256];
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
      CR             = '\r';
      LF             = '\n';
      SP             = [\040];
      HT             = [\011];
      CRLF           = CR LF;
      digit         = [0-9];
      hex           = digit | [A-F] | [a-f];
      escaped       = "%" hex hex;
      reserved      = ";" | "/" | "?" | ":" | "@" | "&" | "=" | "+" | "$" | "," ;
      mark          = "-" | "_" | "." | "!" | "~" | "*" | "'" | "(" | ")" ;
      lowalpha      = [a-z];
      upalpha       = [A-Z];
      alpha         = lowalpha | upalpha;
      alphanum      = alpha | digit; 
      unreserved    = alphanum | mark;
      scheme        = alpha ( alpha | digit | "+" | "-" | "." )*;
      pchar         = unreserved | escaped | ":" | "@" | "&" | "=" | "+" | "$" | ","; 
      param         = pchar*;
      segment       = pchar* ( ";" param )*;
      path_segments = segment ( "/" segment )*;
      abs_path      = "/"  path_segments;
      userinfo      = ( unreserved | escaped | ";" | ":" | "&" | "=" | "+" | "$" | "," )*;
      toplabel      = alpha | alpha ( alphanum | "-" )* alphanum;
      domainlabel   = alphanum | alphanum ( alphanum | "-" )* alphanum;
      hostname      = ( domainlabel "." )* toplabel ( "." )?;
      IPv4address   = digit+ "." digit+ "." digit+ "." digit+;
      host          = hostname | IPv4address ;
      port          = digit*;
      hostport      = host ( ":" port )?;
      server        = ( (userinfo "@")? hostport)?;
      reg_name      = ( unreserved | escaped | "$" | "," | ";" | ":" | "@" | "&" | "=" | "+" )+;
      authority     = server | reg_name;
      uric          = reserved | unreserved | escaped;
       query         = uric*;
      net_path      = "//" authority abs_path?;
      hier_part     = ( net_path | abs_path ) ( "?" query )?;

      uric_no_slash = unreserved | escaped | ";" | "?" | ":" | "@" | "&" | "=" | "+" | "$" | ",";
      opaque_part   = uric_no_slash uric*;
      absoluteURI   = scheme ":" ( hier_part | opaque_part );

      CTOKEN = [\041]|[\043]|[\044]|[\045]|[\046]|[\047]|[\052]|[\053]|[\055]|[\056]|[\060]|[\061]|[\062]|[\063]|[\064]|[\065]|[\066]|[\067]|[\070]|[\071]|[\101]|[\102]|[\103]|[\104]|[\105]|[\106]|[\107]|[\110]|[\111]|[\112]|[\113]|[\114]|[\115]|[\116]|[\117]|[\120]|[\121]|[\122]|[\123]|[\124]|[\125]|[\126]|[\127]|[\130]|[\131]|[\132]|[\136]|[\137]|[\140]|[\141]|[\142]|[\143]|[\144]|[\145]|[\146]|[\147]|[\150]|[\151]|[\152]|[\153]|[\154]|[\155]|[\156]|[\157]|[\160]|[\161]|[\162]|[\163]|[\164]|[\165]|[\166]|[\167]|[\170]|[\171]|[\172]|[\174]|[\176]|[\200]|[\201]|[\202]|[\203]|[\204]|[\205]|[\206]|[\207]|[\210]|[\211]|[\212]|[\213]|[\214]|[\215]|[\216]|[\217]|[\220]|[\221]|[\222]|[\223]|[\224]|[\225]|[\226]|[\227]|[\230]|[\231]|[\232]|[\233]|[\234]|[\235]|[\236]|[\237]|[\240]|[\241]|[\242]|[\243]|[\244]|[\245]|[\246]|[\247]|[\250]|[\251]|[\252]|[\253]|[\254]|[\255]|[\256]|[\257]|[\260]|[\261]|[\262]|[\263]|[\264]|[\265]|[\266]|[\267]|[\270]|[\271]|[\272]|[\273]|[\274]|[\275]|[\276]|[\277]|[\300]|[\301]|[\302]|[\303]|[\304]|[\305]|[\306]|[\307]|[\310]|[\311]|[\312]|[\313]|[\314]|[\315]|[\316]|[\317]|[\320]|[\321]|[\322]|[\323]|[\324]|[\325]|[\326]|[\327]|[\330]|[\331]|[\332]|[\333]|[\334]|[\335]|[\336]|[\337]|[\340]|[\341]|[\342]|[\343]|[\344]|[\345]|[\346]|[\347]|[\350]|[\351]|[\352]|[\353]|[\354]|[\355]|[\356]|[\357]|[\360]|[\361]|[\362]|[\363]|[\364]|[\365]|[\366]|[\367]|[\370]|[\371]|[\372]|[\373]|[\374]|[\375]|[\376]|[\377];
      TOKEN = CTOKEN+;
      ""        { method = 0;goto lbl_end;}
      "GET"     { method = 1;goto lbl_uri;}
      "HEAD"    { method = 2;goto lbl_uri;}
      "POST"    { method = 3;goto lbl_uri;}
      "PUT"     { method = 4;goto lbl_uri;}
      "DELETE"  { method = 5;goto lbl_uri;}
      "CONNECT" { method = 6;goto lbl_uri;}
      "OPTIONS" { method = 7;goto lbl_uri;}
      "TRACE"   { method = 8;goto lbl_uri;}

*/
lbl_uri:
uri.begin=0;
uri.end=0;
printf("method OK\n");
   if(*cursor!=' ') goto lbl_end;
   ++cursor;
   uri.begin=cursor;
printf("uri begin\n");
    /*!re2c
      Request_URI    = "*" | absoluteURI | abs_path | authority;
      ""           { return 0; }
      Request_URI  {uri.end=cursor;goto lbl_version;}
*/

lbl_version:
printf("uri OK\n");
strncpy(str,uri.begin,uri.end-uri.begin);
str[uri.end-uri.begin]='\0';
printf("uri is[%s]\n",str);
   if(*cursor!=' ') goto lbl_end;
   ++cursor;
    /*!re2c

   HTTP_Version   = "HTTP" "/" digit+ "." digit+ ;
   ""           { return 0; }
   HTTP_Version CRLF { return 4; }
*/
lbl_end:
   printf("end\n");
   return 0;
}

void print_values()
{
   int last=0;
   int i=0;
   int j=0;
   const char* sep[]={ "(",")","<",">","@",",",";",":","\\","\"","/","[","]","?","=","{","}",NULL };
   printf("begin\n");
   while(i<=0377)
   {
      if((i>=0)&&(i<=037))//CTL
      {
//printf("excluded [\\%o %d]\n",i,i);
         goto next;
      }
      if(i==040) //SP
      {
//printf("excluded [\\%o %d %s]\n",i,i,"SP");
         goto next;
      }
      if(i==0177) //DEL
      {
//printf("excluded [\\%o %d %s]\n",i,i,"DEL");
         goto next;
      }
      j=0;
      while(sep[j])
      {
         if(*sep[j]==i)
         {
//printf("sep excluded [\\%o %d %c]\n",i,i,*sep[j]);
            goto next;
         }
         ++j;
      }
    printf("[\\%.3o]|",i);
next:
      ++i;
   }
   printf("\n");
}
int main ()
{
   int ret;
   char *str = (char*)malloc(1024);

//   strcpy(str,"GET /index.html HTTP/1.1");
//   strcpy(str,"GET * HTTP/1.1");
//   strcpy(str,"GET /name/bogus HTTP/1.1");
   strcpy(str,"GET http://user@site.com:4566/name/bogus?query HTTP/1.1\r\n");
   ret=parse_request(str, str+strlen(str));
   printf("scan2 [%d]\n",ret);
   switch(ret)
   {
      case 0:printf("no text found\n");break;
      case -1:printf("error !!\n");break;
      case 4:printf("http method match\n");break;
   }
   free(str);
   //print_values();
   return 0;
}

/*
   SP             = [\040];
   HT             = [\011];
   CTL            = [\000-\037] | [\177];
   SEPARATORS     = [()<>@,;:\\"/\[\]?={}] | SP | HT;
   token          = 1*<any CHAR except CTLs or separators>
   Request        = Request-Line              ; Section 5.1
                   *(( general-header        ; Section 4.5
                    | request-header         ; Section 5.3
                    | entity-header ) CRLF)  ; Section 7.1
                   CRLF
                   [ message-body ]          ; Section 4.3
   Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
   Method         = "OPTIONS"                ; Section 9.2
                      | "GET"                    ; Section 9.3
                      | "HEAD"                   ; Section 9.4
                      | "POST"                   ; Section 9.5
                      | "PUT"                    ; Section 9.6
                      | "DELETE"                 ; Section 9.7
                      | "TRACE"                  ; Section 9.8
                      | "CONNECT"                ; Section 9.9
                      | extension-method
   extension-method = token
   Request-URI      = "*" | absoluteURI | abs_path | authority
   request-header   = Accept                   ; Section 14.1
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
*/

