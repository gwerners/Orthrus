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
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <jni.h>

#include "logger.h"
#include "request.pb.h"
#include "response.pb.h"

// http://hildstrom.com/projects/jni/index.html
// http://stackoverflow.com/questions/12872742/sending-jstring-from-jni-c-code-to-a-java-function-that-receives-string-as-an-ar
using namespace std;

/// java method
jmethodID mid = 0;

// java environment
JNIEnv* env;

// java class
jclass cls;

int fd = -1;

m4u_interfaces::RequestBlock req_msg;
m4u_interfaces::ResponseBlock res_msg;

void
ListRequests(const m4u_interfaces::Request& request)
{
  log_debug("###########################################");
  log_debug(" scheme:%s ", request.scheme().c_str());
  log_debug(" name: %s", request.name().c_str());
  log_debug(" raw: %s", request.raw().c_str());
  log_debug(" type: %d", request.type());
  log_debug(" method: %d", request.method());
  log_debug(" status_code: %d", request.status_code());
  log_debug(" response_status: %s", request.response_status().c_str());
  log_debug(" request_path: %s", request.request_path().c_str());
  log_debug(" request_url: %s", request.request_url().c_str());
  log_debug(" fragment: %s", request.fragment().c_str());
  log_debug(" query_string: %s", request.query_string().c_str());
  log_debug(" body: %s", request.body().c_str());
  log_debug(" body_size: %llu", request.body_size());
  log_debug(" host: %s", request.host().c_str());
  log_debug(" userinfo: %s", request.userinfo().c_str());
  log_debug(" port: %d", request.port());
  log_debug(" num_headers: %d", request.num_headers());
  log_debug(" last_header_element: %d", request.last_header_element());
  log_debug(" should_keep_alive: %d", request.should_keep_alive());

  if (request.chunks_size() > 0) {
    log_debug(" Chunks : ");
    for (int j = 0; j < request.chunks_size(); j++) {
      const m4u_interfaces::Request::Chunk& chunk = request.chunks(j);
      log_debug("  %d", chunk.length());
    }
  }

  log_debug(" upgrade: %s", request.upgrade().c_str());
  log_debug(" http_major: %d", request.http_major());
  log_debug(" http_minor: %d", request.http_minor());

  if (request.headers_size() > 0) {
    log_debug(" Headers : ");
    for (int j = 0; j < request.headers_size(); j++) {
      const m4u_interfaces::Request::Header& header = request.headers(j);
      log_debug("  %s: %s", header.key().c_str(), header.value().c_str());
    }
  }
  log_debug(" session: %s", request.session().c_str());
}

void
on_new_connection(std::string& str)
{
  jint square;
  jbyteArray jbytepb = env->NewByteArray(str.length());
  env->SetByteArrayRegion(jbytepb, 0, str.length(), (jbyte*)str.c_str());
  log_debug("Worker %d: Accepted fd %d ", getpid(), fd);
  log_debug("java called to answer client->");
  square = env->CallStaticIntMethod(cls, mid, fd, getpid(), jbytepb);
  log_debug("Result of HandleClient: %d", square);
}

//"-Djava.class.path=.";
int
CreateJVM(char* classpath)
{

  JavaVMOption options[1];
  JavaVM* jvm;
  JavaVMInitArgs vm_args;
  long status;

  options[0].optionString = classpath;
  memset(&vm_args, 0, sizeof(vm_args));
  vm_args.version = JNI_VERSION_1_2;
  vm_args.nOptions = 1;
  vm_args.options = options;
  status = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
  if (status != JNI_ERR) {
    cls = env->FindClass("Worker");
    if (cls != 0) {
      mid = env->GetStaticMethodID(cls, "HandleClient", "(II[B)I");

      ////////////////////////////////////////////////////////////////////
    }
    // precisa ficar ativa para nao queimar chamadas jni!
    //(*jvm)->DestroyJavaVM(jvm);
    return 0;
  } else
    return -1;
}
void
FillRequest(m4u_interfaces::Request* request)
{
  std::string key;
  std::string value;
  m4u_interfaces::Request::Header* headers;
  m4u_interfaces::Request::Chunk* chunks;
  request->set_scheme("http");
  request->set_name("ussd");
  request->set_raw("raw request");
  request->set_type(1);
  request->set_method(1);
  request->set_status_code(0);
  request->set_response_status("response status!");
  request->set_request_path("/path/to/victory");
  request->set_request_url("url_servico");
  request->set_fragment("fragmento");
  request->set_query_string("query");
  request->set_body("body");
  request->set_body_size(4);
  request->set_host("www.host.com");
  request->set_userinfo("usuario:senha");
  request->set_port(1234);
  request->set_num_headers(0);
  request->set_last_header_element(0);

  request->set_should_keep_alive(0);

  request->set_http_major(1);
  request->set_http_minor(1);
  request->set_session("@#$@%@#$@!#$@#$%&%(O%IJHFSF@#%@B%@#$%@B#");
}
int
main(int argc, char** argv)
{
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  std::string output;

  m4u_interfaces::Request msg;

  InitializeLog(stdout);

  fd = 1; // stdout

  if (argc > 1) {
    log_debug("class path[%s]", argv[1])
  }
  CreateJVM(argv[1]);

  FillRequest(&msg);

  if (msg.SerializeToString(&output)) {
    log_debug("serialization ok");
    if (msg.ParseFromString(output)) {
      log_debug("parse ok");
      ListRequests(msg);
    }
  }
  on_new_connection(output);

  return 0;
}
