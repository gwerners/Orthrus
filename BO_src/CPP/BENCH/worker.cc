
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

#include "lua_utils.h"

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
  char answer[1024];
  char* gpage;
  uv_shutdown_t* sreq;
  bool ret;
  write_req_t* req;
  uv_buf_t wrbuf;

  if (nread < 0) {
    if (nread != UV_EOF) {
      log_error("Read error %s", uv_err_name(nread));
    }

    if (buf->base)
      free(buf->base);
    sreq = (uv_shutdown_t*)malloc(sizeof *sreq);
    uv_shutdown(sreq, (uv_stream_t*)client, after_shutdown);
    return;
  }
  // gpage=RunLuaPage("GetPerformancePage");
  sprintf((char*)&answer,
          "HTTP/1.0 200 OK\r\n"
          "Date:  \r\n"
          "Content-Type: text/html\r\n"
          "Content-Length: %d\r\n\r\n%s",
          75,
          "<html><body><h1>My First Heading</h1><p>My first "
          "paragraph.</p></body></html>");
  req = (write_req_t*)malloc(sizeof(write_req_t));
  // req->buf = uv_buf_init(gpage, strlen(gpage));
  req->buf = uv_buf_init(answer, strlen(answer));

  uv_write((uv_write_t*)req, client, &req->buf, 1, echo_write);
  if (gpage) {
    free(gpage);
    gpage = NULL;
  }
  free(buf->base);

  uv_read_stop(client);
  sreq = (uv_shutdown_t*)malloc(sizeof *sreq);
  uv_shutdown(sreq, (uv_stream_t*)client, after_shutdown);
}

void
on_new_connection(uv_stream_t* q, ssize_t nread, const uv_buf_t* buf)
{
  uv_shutdown_t* sreq;
  if (nread < 0) {
    if (nread != UV_EOF) {
      log_error("Read error %s", uv_err_name(nread));
      return;
    }
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

  RunConfiguration("init.lua");

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
