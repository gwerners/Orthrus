/* Copyright Joyent, Inc. and other Node contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
extern "C"{
#include "uv.h"
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT(expr)                                      \
 do {                                                     \
  if (!(expr)) {                                          \
    fprintf(stderr,                                       \
            "Assertion failed in %s on line %d: %s\n",    \
            __FILE__,                                     \
            __LINE__,                                     \
            #expr);                                       \
    abort();                                              \
  }                                                       \
 } while (0)
#define FATAL(msg)                                        \
   do {                                                    \
     fprintf(stderr,                                       \
             "Fatal error in %s on line %d: %s\n",         \
             __FILE__,                                     \
             __LINE__,                                     \
             msg);                                         \
     fflush(stderr);                                       \
     abort();                                              \
   } while (0)

#define TEST_PORT 9000

#define HELPER_IMPL(name)                                                     \
   int run_helper_##name(void);                                                \
   int run_helper_##name(void)

typedef struct {
  uv_write_t req;
  uv_buf_t buf;
} write_req_t;

static uv_loop_t* loop;

static int server_closed;
static uv_tcp_t tcpServer;
static uv_udp_t udpServer;
static uv_pipe_t pipeServer;
static uv_handle_t* server;

static void after_write(uv_write_t* req, int status);
static void after_read(uv_stream_t*, ssize_t nread, const uv_buf_t* buf);
static void on_close(uv_handle_t* peer);
static void on_server_close(uv_handle_t* handle);
static void on_connection(uv_stream_t*, int status);


static void after_shutdown(uv_shutdown_t* req, int status) {
  uv_close((uv_handle_t*) req->handle, on_close);
  free(req);
}

void
after_write_close(uv_write_t* req, int status)
{
  write_req_t* wreq = (write_req_t*)req;
  //printf("after write close\n");
  if (status) {
    printf("Write error %s\n", uv_err_name(status));
  }
  free(wreq);
}
typedef struct workData_t{
  uv_stream_t* handle;
  uv_buf_t* buf;
} workData;
static void work_cb(uv_work_t*req){
    write_req_t *wr;
    workData *wrk = (workData*)req->data;
    uv_stream_t*handle = (uv_stream_t*)wrk->handle;
    uv_buf_t* buf=(uv_buf_t*)wrk->buf;
    const char* r404 = "HTTP/1.1 404 Not Found\r\nContent-Length: "
                       "35\r\nConnection: close\r\n\r\n<html><h1>404 NOT "
                       "FOUND</h1></html>\r\n";
    //printf("received %s\n",buf->base);
    wr = (write_req_t*)malloc(sizeof(write_req_t));
    ASSERT(wr != NULL);
    wr->buf = uv_buf_init((char*)r404, strlen(r404)+1);
    //wr->buf = uv_buf_init(buf->base, nread);
     //printf("send %s\n",wr->buf.base);
    if (uv_write(&wr->req, handle, &wr->buf, 1, after_write_close)) {
      FATAL("uv_write failed");
    }

    if (buf->base)
      free(buf->base);
    //free(wrk);
    uv_read_stop(handle);
    uv_close((uv_handle_t*)handle, on_close);
}
static void after_work_cb(uv_work_t*req,int status){
    ASSERT(status==0);
    free(req->data);
    free(req);
}
static void after_read(uv_stream_t* handle,
                       ssize_t nread,
                       const uv_buf_t* buf) {
  int i;

  uv_shutdown_t* sreq;
  //printf("after read \n");
  if (nread < 0) {
    /* Error or EOF */
    ASSERT(nread == UV_EOF);

    free(buf->base);
    sreq = (uv_shutdown_t*)malloc(sizeof* sreq);
    ASSERT(0 == uv_shutdown(sreq, handle, after_shutdown));
    return;
  }

  if (nread == 0) {
    /* Everything OK, but nothing read. */
    free(buf->base);
    return;
  }


  uv_work_t* req=(uv_work_t*)malloc(sizeof(uv_work_t));
  workData *wrk = (workData*)calloc(1,sizeof(workData));
  wrk->handle=handle;
  wrk->buf=(uv_buf_t*)buf;
  req->data=wrk;
  int r=uv_queue_work(loop,req,work_cb,after_work_cb);
  ASSERT(r==0);
}


static void on_close(uv_handle_t* peer) {
  free(peer);
}


static void echo_alloc(uv_handle_t* handle,
                       size_t suggested_size,
                       uv_buf_t* buf) {
  //printf("echo_alloc \n");
  buf->base = (char*)malloc(suggested_size);
  buf->len = suggested_size;
}


static void on_connection(uv_stream_t* server, int status) {
  uv_stream_t* stream;
  int r;

  if (status != 0) {
    fprintf(stderr, "Connect error %s\n", uv_err_name(status));
  }
  ASSERT(status == 0);


  stream = (uv_stream_t*)malloc(sizeof(uv_tcp_t));
  ASSERT(stream != NULL);
  r = uv_tcp_init(loop, (uv_tcp_t*)stream);
  ASSERT(r == 0);


  /* associate server with stream */
  stream->data = server;

  r = uv_accept(server, stream);
  ASSERT(r == 0);

  r = uv_read_start(stream, echo_alloc, after_read);
  ASSERT(r == 0);
}


static void on_server_close(uv_handle_t* handle) {
  ASSERT(handle == server);
}

static int tcp4_echo_start(int port) {
  struct sockaddr_in addr;
  int r;

  ASSERT(0 == uv_ip4_addr("0.0.0.0", port, &addr));

  server = (uv_handle_t*)&tcpServer;

  r = uv_tcp_init(loop, &tcpServer);
  if (r) {
    /* TODO: Error codes */
    fprintf(stderr, "Socket creation error\n");
    return 1;
  }

  r = uv_tcp_bind(&tcpServer, (const struct sockaddr*) &addr, 0);
  if (r) {
    /* TODO: Error codes */
    fprintf(stderr, "Bind error\n");
    return 1;
  }

  r = uv_listen((uv_stream_t*)&tcpServer, SOMAXCONN, on_connection);
  if (r) {
    /* TODO: Error codes */
    fprintf(stderr, "Listen error %s\n", uv_err_name(r));
    return 1;
  }

  return 0;
}





HELPER_IMPL(tcp4_echo_server) {
  loop = uv_default_loop();

  if (tcp4_echo_start(TEST_PORT))
    return 1;
  printf("started %d\n",TEST_PORT);
  uv_run(loop, UV_RUN_DEFAULT);
  return 0;
}
int main(int argc,char**argv){
  run_helper_tcp4_echo_server();
  return 0;
}
