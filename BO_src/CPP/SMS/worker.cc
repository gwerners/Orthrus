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

#include "logger.h"
#include "request.pb.h"
#include "response.pb.h"
#include <uv.h>

typedef struct
{
  uv_write_t req;
  uv_buf_t buf;
} write_req_t;

using namespace std;

int num_request = 1;
uv_loop_t* loop;

uv_pipe_t queue;
uv_pipe_t stdin_pipe;
uv_pipe_t stdout_pipe;

m4u_interfaces::RequestBlock req_msg;
m4u_interfaces::ResponseBlock res_msg;

uv_stream_t* last_client = NULL;
uv_stream_t* close_client = NULL;

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
    free(handle);
}
static void
after_shutdown(uv_shutdown_t* req, int status)
{
  uv_close((uv_handle_t*)req->handle, on_close);
  free(req);
}
void
echo_write(uv_write_t* req, int status)
{
  write_req_t* wr;
  ++num_request;
  if (status) {
    log_error("Write error %s", uv_err_name(status));
  }

  wr = (write_req_t*)req;
  if (wr)
    free(wr);
}
void
readed(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
  free(buf->base);
}
void
new_answer_request(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
  uv_shutdown_t* sreq;
  std::string str_output;
  bool ret;
  write_req_t* req;
  uv_buf_t wrbuf;
  std::string input;
  char header[1024];
  char page[1024];
  char page2[1024];
  char body[1024];
  header[0] = '\0';
  page[0] = '\0';
  page2[0] = '\0';
  body[0] = '\0';
  m4u_interfaces::Request request;
  request.Clear();
  if (nread < 0) {
    if (nread != UV_EOF) {
      log_error("Read error %s", uv_err_name(nread));
    }
    // uv_close((uv_handle_t*) client, NULL);
    if (buf->base)
      free(buf->base);
    sreq = (uv_shutdown_t*)malloc(sizeof *sreq);
    uv_shutdown(sreq, (uv_stream_t*)client, after_shutdown);
    return;
  }
  log_debug("the message is[%s]", buf->base);
  input = buf->base;
  ret = request.ParseFromString(input);
  if (!ret) {
    log_error("Failed to parse request.");
    const char* r400 = "HTTP/1.1 400 Bad Request\r\nContent-Length: "
                       "37\r\nConnection: close\r\n\r\n<html><h1>400 BAD "
                       "REQUEST</h1></html>\r\n";
    // sprintf(body,"worker-Failed to parse request(%s)
    // (%s).",buf->base,req_msg.DebugString().c_str());
    log_error("worker-Failed to parse request(%s) (%s).",
              buf->base,
              req_msg.DebugString().c_str());
    strcpy(body, r400);
    req = (write_req_t*)malloc(sizeof(write_req_t));
    req->buf = uv_buf_init(body, strlen(body));
    uv_write((uv_write_t*)req, client, &req->buf, 1, echo_write);
    log_debug("%s", body);
    if (buf->base)
      free(buf->base);
    sreq = (uv_shutdown_t*)malloc(sizeof *sreq);
    uv_shutdown(sreq, (uv_stream_t*)client, after_shutdown);
    return;
  }
  strcat(header, "HTTP/1.0 200 OK\n");
  strcat(header, "Date: Fri, 31 Dec 1999 23:59:59 GMT\n");
  strcat(header, "Content-Type: text/html\n");
  strcat(header, "Content-Length: %d\r\n\r\n%s");

  strcat(page, "<html>\n");
  strcat(page, " <head>\n");
  strcat(page, "  <title>REQUESTED %s</title>\n");
  strcat(page, " </head>\n");
  strcat(page, " <body>\n");
  if (request.headers_size() > 0) {
    strcat(page, "  <h1>Parameters:</h1>\n");
    for (int j = 0; j < request.headers_size(); j++) {
      const m4u_interfaces::Request::Header& headers = request.headers(j);
      strcat(page, "  <p>");
      strcat(page, headers.key().c_str());
      strcat(page, " -> ");
      strcat(page, headers.value().c_str());
      strcat(page, "</p>\n");
    }
  }
  strcat(page, "  <p>Service Name is %s</p>\n");
  strcat(page, "  <p>Mypid is %d</p>\n");
  strcat(page, "  <p>This is request %d</p>\n");
  strcat(page, "  <p>body (%s)</p>\n");
  strcat(page, " </body>\n");
  strcat(page, "</html>\n");

  sprintf(page2,
          page,
          request.request_path().c_str(),
          request.name().c_str(),
          getpid(),
          num_request,
          request.body().c_str());

  sprintf(body, header, strlen(page2), page2);

  req = (write_req_t*)malloc(sizeof(write_req_t));
  req->buf = uv_buf_init(body, strlen(body));
  log_debug("answer:%s", body);
  uv_write((uv_write_t*)req, client, &req->buf, 1, echo_write);

  free(buf->base);
  // close_client=(uv_stream_t*)client;
  uv_read_stop(client);
  sreq = (uv_shutdown_t*)malloc(sizeof *sreq);
  uv_shutdown(sreq, (uv_stream_t*)client, after_shutdown);
}

void
on_new_connection(uv_stream_t* q, ssize_t nread, const uv_buf_t* buf)
{
  uv_shutdown_t* sreq;
  if (nread < 0) {
    if (nread != UV_EOF)
      log_error("Read error %s", uv_err_name(nread));
    log_debug("before shutdown");
    if (q && (nread != UV_EOF)) {
      sreq = (uv_shutdown_t*)malloc(sizeof *sreq);
      uv_shutdown(sreq, q, after_shutdown);
    }
    if (buf->base)
      free(buf->base);
    return;
  }
  // log_error("the message is[%s]",buf->base);
  uv_pipe_t* pipe = (uv_pipe_t*)q;
  if (!uv_pipe_pending_count(pipe)) {
    // log_debug("No pending count");
    new_answer_request(last_client, nread, buf);
    last_client = NULL;
    return;
  }
  // log_error("Pending count[%d]",uv_pipe_pending_count(pipe));
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
  } else {
    log_debug("before shutdown");
    sreq = (uv_shutdown_t*)malloc(sizeof *sreq);
    uv_shutdown(sreq, (uv_stream_t*)client, after_shutdown);
    last_client = NULL;
  }
  if (buf->base)
    free(buf->base);
}

int
main()
{
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  InitializeLog(stdout);
  EnabledLog = LOG_COLOR | LOG_INFO | LOG_ERROR;
  //;test_write(argc,argv);
  // test_read(argc,argv);

  // Optional:  Delete all global objects allocated by libprotobuf.
  // google::protobuf::ShutdownProtobufLibrary();
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
  return uv_run(loop, UV_RUN_DEFAULT);
}
