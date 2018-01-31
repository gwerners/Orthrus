#include <iostream>
#include <fstream>
#include <string>
#include "request.pb.h"
using namespace std;


void PromptForRequest(m4u_interfaces::Request* request)
{
   cout << "Enter socket: ";
   int socket;
   cin >> socket;
   request->set_socket(socket);
   cin.ignore(256, '\n');

   cout << "Enter path: ";
   getline(cin, *request->mutable_path());

   cout << "Enter method(GET,HEAD,POST,PUT,DELETE,CONNECT,OPTIONS,TRACE):";

   string type;
   getline(cin, type);
   if (type == "GET") {
      request->set_type(m4u_interfaces::Request::GET);
   } else if (type == "HEAD") {
      request->set_type(m4u_interfaces::Request::HEAD);
   } else if (type == "POST") {
      request->set_type(m4u_interfaces::Request::POST);
   } else if (type == "PUT") {
      request->set_type(m4u_interfaces::Request::PUT);
   } else if (type == "DELETE") {
      request->set_type(m4u_interfaces::Request::DELETE);
   } else if (type == "CONNECT") {
      request->set_type(m4u_interfaces::Request::CONNECT);
   } else if (type == "OPTIONS") {
      request->set_type(m4u_interfaces::Request::OPTIONS);
   } else if (type == "TRACE") {
      request->set_type(m4u_interfaces::Request::TRACE);
   } else {
      cout << "Unknown method type.  Using default." << endl;
   }

   while (true) {
      cout << "Enter parameter list" << endl;
      cout << "key: ";
      string key;
      getline(cin, key);
      if (key.empty()) {
         break;
      }

      m4u_interfaces::Request::Parameters* parameter = request->add_params();
      parameter->set_key(key);
      cout << "value: ";
      string value;
      getline(cin, value);
      parameter->set_value(value);
   }
   cout << "Enter body: ";
   getline(cin, *request->mutable_body());
}
void ListRequests(const m4u_interfaces::MessageBlock& msg)
{
   for (int i = 0; i < msg.request_size(); i++) {
      const m4u_interfaces::Request& request = msg.request(i);
      cout << "###########################################" << endl;
      cout << " socket: " << request.socket() << endl;
      cout << " path: " << request.path() << endl;

      cout << " Method : ";
      switch (request.type()) {
         case m4u_interfaces::Request::GET:
            cout << "GET" << endl;
            break;
         case m4u_interfaces::Request::HEAD:
            cout << "HEAD" << endl;
            break;
         case m4u_interfaces::Request::POST:
            cout << "POST" << endl;
            break;
         case m4u_interfaces::Request::PUT:
            cout << "PUT" << endl;
            break;
         case m4u_interfaces::Request::DELETE:
            cout << "DELETE" << endl;
            break;
         case m4u_interfaces::Request::CONNECT:
            cout << "CONNECT" << endl;
            break;
         case m4u_interfaces::Request::OPTIONS:
            cout << "OPTIONS" << endl;
            break;
         case m4u_interfaces::Request::TRACE:
            cout << "TRACE" << endl;
            break;
      }

      if(request.params_size()>0)
      {
         cout << " Parameters : "<<endl;
         for (int j = 0; j < request.params_size(); j++) {
            const m4u_interfaces::Request::Parameters& parameter = request.params(j);

            cout << "  " << parameter.key() << ":" <<parameter.value() << endl;
         }
      }
      cout << " body: " << request.body() << endl;
   }
}

int test_write(int argc,char**argv)
{

   if (argc != 2) {
      cerr << "Usage:  " << argv[0] << " REQUEST_FILE" << endl;
      return -1;
   }

   m4u_interfaces::MessageBlock msg;

   {
      // Read the existing request
      fstream input(argv[1], ios::in | ios::binary);
      cout << "filename:" << argv[1] << endl;
      if (!input)
      {
         cout << argv[1] << ": File not found.  Creating a new file." << endl;
      } else if (!msg.ParseFromIstream(&input))
      {
         cerr << "Failed to parse request." << endl;
         return -1;
      }
   }

   // Add an address.
   PromptForRequest(msg.add_request());

   {
      // Write the new address book back to disk.
      fstream output(argv[1], ios::out | ios::trunc | ios::binary);
      if (!msg.SerializeToOstream(&output))
      {
         cerr << "Failed to request." << endl;
         return -1;
      }
   }

}

int test_read(int argc,char**argv)
{


   if (argc != 2) {
      cerr << "Usage:  " << argv[0] << " REQUEST_FILE" << endl;
      return -1;
   }

   m4u_interfaces::MessageBlock msg;

   {
      // Read the existing request
      fstream input(argv[1], ios::in | ios::binary);
      if (!msg.ParseFromIstream(&input)) {
         cerr << "Failed to parse request." << endl;
         return -1;
      }
   }

   ListRequests(msg);
   return 0;
}

int main(int argc, char**argv)
{
   // Verify that the version of the library that we linked against is
   // compatible with the version of the headers we compiled against.
   GOOGLE_PROTOBUF_VERIFY_VERSION;
   std::cout << "teste begin" << std:: endl;
   //;test_write(argc,argv);
   test_read(argc,argv);

   // Optional:  Delete all global objects allocated by libprotobuf.
   google::protobuf::ShutdownProtobufLibrary();
   return 0;
}

