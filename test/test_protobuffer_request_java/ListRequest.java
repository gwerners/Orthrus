// See README.txt for information and build instructions.

import com.m4u_interfaces.request.RequestProtos.MessageBlock;
import com.m4u_interfaces.request.RequestProtos.Request;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.PrintStream;

class ListRequest {
  // Iterates though all people in the AddressBook and prints info about them.
  static void Print(MessageBlock msg) {

    for (Request request: msg.getRequestList()) {
      System.out.println("  path: " + request.getPath());

      switch (request.getType()) {
         case GET:
            System.out.println("  method: GET");
            break;
         case HEAD:
            System.out.println("  method: HEAD");
            break;
         case POST:
            System.out.println("  method: POST");
            break;
         case PUT:
            System.out.println("  method: PUT");
            break;
         case DELETE:
            System.out.println("  method: DELETE");
            break;
         case CONNECT:
            System.out.println("  method: CONNECT");
            break;
         case OPTIONS:
            System.out.println("  method: OPTIONS");
            break;
         case TRACE:
            System.out.println("  method: TRACE");
            break;
      }

      for (Request.Parameters parameter : request.getParamsList()) {
        System.out.println("  "+parameter.getKey() + ":" + parameter.getValue());
      }
      System.out.println("  body: " + request.getBody());
    }
  }

  // Main function:  Reads the entire address book from a file and prints all
  //   the information inside.
  public static void main(String[] args) throws Exception {
    if (args.length != 1) {
      System.err.println("Usage:  ListRequest REQUEST_FILE");
      System.exit(-1);
    }

    // Read the existing address book.
    MessageBlock msg =
      MessageBlock.parseFrom(new FileInputStream(args[0]));

    Print(msg);
  }
}
