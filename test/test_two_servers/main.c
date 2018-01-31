#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <uv.h>


#include <uv.h>

#define STDIN   0
#define STDOUT  1
#define STDERR  2

typedef enum {
  TCP = 0,
  UDP,
  PIPE
} stream_type;

uv_loop_t *loop;

static stream_type serverType;


uv_process_t child_req;
uv_process_options_t options;

void uv_on_exit(uv_process_t *req, int64_t exit_status, int term_signal) {
    fprintf(stderr, "Process exited with status %" PRId64 ", signal %d\n", exit_status, term_signal);
    uv_close((uv_handle_t*) req->data, NULL);
    uv_close((uv_handle_t*) req, NULL);
}


void invoke_cgi_script(uv_tcp_t *client,const char*filename) {
    size_t size = 500;
    char path[size];


    uv_exepath(path, &size);
    sprintf(path + (strlen(path) - strlen("client.exe")), "cgi/%s.exe",filename);

    char* args[2];
    args[0] = path;
    args[1] = NULL;

    printf("path[%s]\n",path);
    /* ... finding the executable path and setting up arguments ... */


    uv_stdio_container_t child_stdio[3];

   child_stdio[STDIN].flags    =  UV_IGNORE;
   child_stdio[STDOUT].flags   =  UV_INHERIT_STREAM;
   child_stdio[STDERR].flags   =  UV_IGNORE;

   // pipe tick stdout into the tcp stream connected to the client
   child_stdio[STDOUT].data.stream = (uv_stream_t*) client;

   // https://github.com/thlorenz/libuv-dox/blob/master/types.md#uv_process_options_t
   options.stdio_count =  3;
   options.stdio       =  child_stdio;
   options.exit_cb     =  uv_on_exit;
   options.file        =  path;
   options.args        =  args;

   // pass this handle along so we can close it on_exit
   child_req.data = (void*) client;

    int r;
    if ((r = uv_spawn(loop, &child_req, &options))) {
        fprintf(stderr, "%s\n", uv_strerror(r));
        return;
    }
}
void on_connection_A(uv_stream_t *server, int status) {

    if (status == -1) {
        // error!
        return;
    }
fprintf(stderr, "connection A\n");
    uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, client);
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        invoke_cgi_script(client,"tick");
    }
    else {
        uv_close((uv_handle_t*)&client, NULL);
    }
}
void on_connection_B(uv_stream_t *server, int status) {

    if (status == -1) {
        // error!
        return;
    }
fprintf(stderr, "connection B\n");
    uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, client);
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        invoke_cgi_script(client,"tack");
    }
    else {
        uv_close((uv_handle_t*)&client, NULL);
    }
}

static int tcp4_start(uv_tcp_t *tcpServer,int port)
{
   struct sockaddr_in addr;
   int r;
   uv_handle_t* server;

   uv_ip4_addr("127.0.0.1", port, &addr);

   server = (uv_handle_t*)tcpServer;
   serverType = TCP;

   r = uv_tcp_init(loop, tcpServer);
   if (r)
   {
      /* TODO: Error codes */
      printf("Socket creation error\n");
      return 1;
   }

   r = uv_tcp_bind(tcpServer, (const struct sockaddr*) &addr, 0);
   if (r)
   {
      /* TODO: Error codes */
      printf("Bind error\n");
      return 1;
   }
   if(port==7001)
   r = uv_listen((uv_stream_t*)tcpServer, SOMAXCONN, on_connection_A);
   else
   r = uv_listen((uv_stream_t*)tcpServer, SOMAXCONN, on_connection_B);
   if (r)
   {
      /* TODO: Error codes */
      printf("Listen error %s\n", uv_err_name(r));
      return 1;
   }
   printf("Server Listening to port[%d]\n",port);
   return 0;
}
uv_loop_t* create_loop()
{
   uv_loop_t *loop = malloc(sizeof(uv_loop_t));
   if (loop) {
      uv_loop_init(loop);
   }
   return loop;
}
int main(int argc, char *argv[])
{
   uv_tcp_t tcpServerA;
   uv_tcp_t tcpServerB;
   loop = create_loop();


   if (tcp4_start(&tcpServerA,7000))
      return 0;

   if (tcp4_start(&tcpServerB,7001))
      return 0;

    uv_run(loop, UV_RUN_DEFAULT);

    return 0;
}
