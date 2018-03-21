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
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>

#include <jni.h>
#include <uv.h>

#include "logger.h"
#include "request.pb.h"
#include "response.pb.h"

#define FREE(pointer)                                                          \
  log_debug("%p being freed!", pointer);                                       \
  free(pointer)
#define HANDLE(pointer) log_debug("HANDLE %p", pointer);

typedef struct
{
  uv_write_t req;
  uv_buf_t buf;
} write_req_t;

using namespace std;

/// java method
jmethodID mid = 0;

// java environment
JNIEnv* env;

// java class
jclass cls;

JavaVMOption options[1];
JavaVM* jvm;
JavaVMInitArgs vm_args;

int num_request = 1;

int request_fd;
pid_t pid;

uv_stream_t* last_client = NULL;

uv_loop_t* loop;

uv_pipe_t queue;
uv_pipe_t stdin_pipe;
uv_pipe_t stdout_pipe;

interfaces::RequestBlock req_msg;
interfaces::ResponseBlock res_msg;

char* default_class_path = (char*)"-Djava.class.path=.";

void
alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
  buf->base = (char*)malloc(suggested_size);
  buf->len = suggested_size;
}
static void
on_close(uv_handle_t* handle)
{
  log_debug("onclose");
  if (handle)
    FREE(handle);
}
static void
after_shutdown(uv_shutdown_t* req, int status)
{
  if (req) {
    uv_close((uv_handle_t*)req->handle, on_close);
    FREE(req);
  }
}
void
FillRequest(interfaces::Request* request)
{
  std::string key;
  std::string value;
  interfaces::Request::Header* headers;
  interfaces::Request::Chunk* chunks;
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
void
ListRequests(const interfaces::Request& request)
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
      const interfaces::Request::Chunk& chunk = request.chunks(j);
      log_debug("  %d", chunk.length());
    }
  }

  log_debug(" upgrade: %s", request.upgrade().c_str());
  log_debug(" http_major: %d", request.http_major());
  log_debug(" http_minor: %d", request.http_minor());

  if (request.headers_size() > 0) {
    log_debug(" Headers : ");
    for (int j = 0; j < request.headers_size(); j++) {
      const interfaces::Request::Header& header = request.headers(j);
      log_debug("  %s: %s", header.key().c_str(), header.value().c_str());
    }
  }
  log_debug(" session: %s", request.session().c_str());
}

void
echo_write(uv_write_t* req, int status)
{
  write_req_t* wr;
  ++num_request;
  if (status) {
    log_error("Write error %s", uv_err_name(status));
  }
  HANDLE(req);
  wr = (write_req_t*)req;
  if (wr)
    FREE(wr);
}
void
readed(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
  if (buf->base)
    FREE(buf->base);
}
void
new_answer_request(uv_stream_t* q, ssize_t nread, const uv_buf_t* buf)
{
  uv_shutdown_t* sreq;
  jint square;
  std::string output;
  HANDLE(q);
  HANDLE(buf);
#ifdef DEBUG_ME
  interfaces::Request msg;
#endif
  log_debug("java called to answer client->");
#ifdef DEBUG_ME
  if (msg.ParseFromString(buf->base)) {
    log_debug("parse ok");
    ListRequests(msg);
  }
#endif
  output = buf->base;
  jbyteArray jbytepb = env->NewByteArray(output.length());
  env->SetByteArrayRegion(jbytepb, 0, output.length(), (jbyte*)output.c_str());
  square = env->CallStaticIntMethod(cls, mid, request_fd, pid, jbytepb);
  env->DeleteLocalRef(jbytepb);
  log_debug("Result of HandleClient: %d", square);
  close(request_fd);
  log_debug("before shutdown");
  if (buf->base) {
    FREE(buf->base);
  }
  uv_read_stop(q);
  uv_close((uv_handle_t*)q, on_close);
  q = NULL;
}

void
on_new_connection(uv_stream_t* q, ssize_t nread, const uv_buf_t* buf)
{
  uv_shutdown_t* sreq;
  HANDLE(q);
  HANDLE(buf);
  if (nread < 0) {
    if (nread != UV_EOF)
      log_error("Read error %s", uv_err_name(nread));
    if (buf->base) {
      FREE(buf->base);
    }
    q = NULL;
    return;
  }
  uv_pipe_t* pipe = (uv_pipe_t*)q;
  if (!uv_pipe_pending_count(pipe)) {
    // log_debug("No pending count");
    new_answer_request(last_client, nread, buf);
    last_client = NULL;
    return;
  }

  uv_handle_type pending = uv_pipe_pending_type(pipe);
  assert(pending == UV_TCP);

  uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
  uv_tcp_init(loop, client);
  if (uv_accept(q, (uv_stream_t*)client) == 0) {
    uv_os_fd_t fd;
    uv_fileno((const uv_handle_t*)client, &fd);
    log_debug("Worker %d: Accepted fd %d", getpid(), fd);
    uv_read_start((uv_stream_t*)client, alloc_buffer, readed);
    last_client = (uv_stream_t*)client;
    request_fd = fd;
    pid = getpid();
    HANDLE(client);
  } else {
    log_debug("before shutdown");
    last_client = NULL;
    client = NULL;
  }
  if (buf->base) {
    FREE(buf->base);
  }
}

int
CreateJVM(char* classpath)
{

  long status;

  options[0].optionString = classpath; //"-Djava.class.path=.";
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
int
main(int argc, char** argv)
{
  int ret;
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  InitializeLog(stdout);

  EnabledLog = LOG_COLOR | LOG_INFO | LOG_WARNING | LOG_ERROR;

  if (argc > 1) {
    log_debug("class path[%s]", argv[1]);
    CreateJVM(argv[1]);
  } else {
    log_debug(
      "class path default[%s] %d:%s", default_class_path, argc, argv[0]);
    CreateJVM(default_class_path);
  }

  loop = uv_default_loop();

  uv_pipe_init(loop, &queue, 1 /* ipc */);
  uv_pipe_open(&queue, 0);

  /* adds input pipe to loop */
  uv_pipe_init(loop, &stdin_pipe, 0);
  /* opens input pipe */
  uv_pipe_open(&stdin_pipe, 0);

  /* adds stdoutput pipe to loop */
  uv_pipe_init(loop, &stdout_pipe, 0);
  /* opens stdoutput pipe */
  uv_pipe_open(&stdout_pipe, 1);

  uv_read_start((uv_stream_t*)&queue, alloc_buffer, on_new_connection);
  ret = uv_run(loop, UV_RUN_DEFAULT);

  jvm->DestroyJavaVM();
  return ret;
}
