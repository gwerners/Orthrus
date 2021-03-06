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
import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.FileDescriptor;

import java.io.BufferedOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;

import java.io.InputStream;
import java.io.ByteArrayInputStream;
import java.nio.charset.StandardCharsets;

import com.interfaces.request.RequestProtos.Request;
import com.google.protobuf.InvalidProtocolBufferException;

class Worker
{
   public static Request req;
   public static int counter=0;
   static void FillRequest(byte[] content)
   {
		try {
			req = Request.parseFrom(content);
         PrintRequest(req);
			//System.out.println("Request:" + req.toString());
			//System.out.println("Service name: " + req.getName());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
		}
      return;
   }
   public static void PrintRequest(Request request)
   {
      System.out.println(" PrintRequest");
      System.out.println(" scheme:"       +  request.getScheme());
      System.out.println(" name: "        +  request.getName());
      System.out.println(" raw: "         +  request.getRaw());
      System.out.println(" type: "        + request.getType());
      System.out.println(" method: "      + request.getMethod());
      System.out.println(" status_code: " + request.getStatusCode());
      System.out.println(" response_status: " + request.getResponseStatus());
      System.out.println(" request_path: "    + request.getRequestPath());
      System.out.println(" request_url: "     + request.getRequestUrl());
      System.out.println(" fragment: "        + request.getFragment());
      System.out.println(" query_string: "    + request.getQueryString());
      System.out.println(" body: "            + request.getBody());
      System.out.println(" body_size: "       + request.getBodySize());
      System.out.println(" host: "            + request.getHost());
      System.out.println(" userinfo: "        + request.getUserinfo());
      System.out.println(" port: "            + request.getPort());
      System.out.println(" num_headers: "     + request.getNumHeaders());
      System.out.println(" should_keep_alive: " + request.getShouldKeepAlive());
   }
   public static int HttpResponse(int fd,int pid)
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

         bos.close();
      } catch (Exception e) {
         e.printStackTrace();
      }


      return fd;
   }

   public static int HandleClient(int fd,int pid, byte[] content)
   {
      //String s1 = new String(content);

      //System.out.println(s1);
      //System.out.println("HandleClient begin");
      FillRequest(content);
      if(fd!=-1)
         HttpResponse(fd,pid);

      //System.out.println("HandleClient end");
      return counter;
   }

}
