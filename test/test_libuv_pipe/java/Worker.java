
import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.FileDescriptor;

import java.io.BufferedOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;


class Worker
{
   public static int counter=0;
   //public static native void Done(); //FROM C!!!!!!!!!!!!!!!!!!
   public static int HandleClient(int fd,int pid)
   {
      String header=new String("HTTP/1.0 200 OK\n" +
            "Date: Fri, 31 Dec 1999 23:59:59 GMT\n"+
            "Content-Type: text/html\n");
      String body=new String(
            "<html>\n"+
            " <head>\n"+
            "  <title>this is the title</title>\n"+
            " </head>\n"+
            " <body>\n"+
            "  <h1>From Java:My Heading</h1>\n"+
            "  <p>My fd is "+ fd +"</p>\n"+
            "  <p>My pid is "+ pid +"</p>\n"+
            "  <p>Counter is "+ counter +"</p>\n"+
            " </body>\n"+
            "</html>\n");

      String page=new String("");
      counter= counter +1;

      byte[] buffer = new byte[8192];

      FileDescriptor myFD = new FileDescriptor();

      sun.misc.SharedSecrets.getJavaIOFileDescriptorAccess().set(myFD,fd);
      try {
         BufferedOutputStream bos = new BufferedOutputStream(new FileOutputStream(myFD));

         page=header+"Content-Length: "+body.length()+"\r\n\r\n"+body;
         buffer=page.getBytes();

         bos.write(buffer);
         //Done(fd);
         bos.close();
      } catch (Exception e) {
         e.printStackTrace();
      } 
      return fd;
   }

}
