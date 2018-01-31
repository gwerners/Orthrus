#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main()
{
   char header[1024];
   char page[1024];
   char body[1024];
   header[0]='\0';
   page[0]='\0';
   body[0]='\0';
   

   strcat(header,"HTTP/1.0 200 OK\n");
   strcat(header,"Date: Fri, 31 Dec 1999 23:59:59 GMT\n");
   strcat(header,"Content-Type: text/html\n");
   strcat(header,"Content-Length: %d\r\n\r\n%s");

   strcat(page,"<html>\n");
   strcat(page," <head>\n");
   strcat(page,"  <title>this is the title</title>\n");
   strcat(page," </head>\n");
   strcat(page," <body>\n");
   strcat(page,"  <h1>Tick</h1>\n");
   strcat(page,"  <p>This is the first paragraph of text.</p>\n");
   strcat(page,"  <p>This is the second paragraph of text.</p>\n");
   strcat(page," </body>\n");
   strcat(page,"</html>\n");

   sprintf(body,header,strlen(page),page);
   printf("%s",body);
   fflush(stdout);
   sleep(1);
    return 0;
}
