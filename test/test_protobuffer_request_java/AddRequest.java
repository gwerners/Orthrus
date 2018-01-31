// See README.txt for information and build instructions.

import com.m4u_interfaces.request.RequestProtos.MessageBlock;
import com.m4u_interfaces.request.RequestProtos.Request;
import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.PrintStream;

class AddRequest {
  // This function fills in a Person message based on user input.
  static Request PromptForRequest(BufferedReader stdin,
                                 PrintStream stdout) throws IOException {
   Request.Builder request = Request.newBuilder();

   stdout.print("Enter path: ");
   request.setPath(stdin.readLine());


   stdout.print("Enter method(GET,HEAD,POST,PUT,DELETE,CONNECT,OPTIONS,TRACE): ");
   String type = stdin.readLine();
   if (type.equals("GET")) {
     request.setType(Request.MethodType.GET);
   } else if (type.equals("HEAD")) {
     request.setType(Request.MethodType.HEAD);
   } else if (type.equals("POST")) {
     request.setType(Request.MethodType.POST);
   } else if (type.equals("PUT")) {
     request.setType(Request.MethodType.PUT);
   } else if (type.equals("DELETE")) {
     request.setType(Request.MethodType.DELETE);
   } else if (type.equals("CONNECT")) {
     request.setType(Request.MethodType.CONNECT);
   } else if (type.equals("OPTIONS")) {
     request.setType(Request.MethodType.OPTIONS);
   } else if (type.equals("TRACE")) {
     request.setType(Request.MethodType.TRACE);
   } else {
     stdout.println("Unknown method type.  Using default.");
   }

   while (true) {
      stdout.print("Enter parameter list ");
      stdout.print("key: ");
      String key = stdin.readLine();
      if (key.length() == 0) {
        break;
      }

      Request.Parameters.Builder parameter =
        Request.Parameters.newBuilder().setKey(key);

      stdout.print("value: ");
      parameter.setValue(stdin.readLine());

      request.addParams(parameter);
    }

   stdout.print("Enter body: ");
   request.setBody(stdin.readLine());
   return request.build();
  }

  // Main function:  Reads the entire address book from a file,
  //   adds one person based on user input, then writes it back out to the same
  //   file.
  public static void main(String[] args) throws Exception {
    if (args.length != 1) {
      System.err.println("Usage:  AddRequest REQUEST_FILE");
      System.exit(-1);
    }

    MessageBlock.Builder msg = MessageBlock.newBuilder();

    // Read the existing address book.
    try {
      FileInputStream input = new FileInputStream(args[0]);
      try {
        msg.mergeFrom(input);
      } finally {
        try { input.close(); } catch (Throwable ignore) {}
      }
    } catch (FileNotFoundException e) {
      System.out.println(args[0] + ": File not found.  Creating a new file.");
    }

    // Add an address.
    msg.addRequest(
      PromptForRequest(new BufferedReader(new InputStreamReader(System.in)),
                       System.out));

    // Write the new address book back to disk.
    FileOutputStream output = new FileOutputStream(args[0]);
    try {
      msg.build().writeTo(output);
    } finally {
      output.close();
    }
  }
}
