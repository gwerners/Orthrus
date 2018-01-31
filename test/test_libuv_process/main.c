#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#define STDIN   0
#define STDOUT  1
#define STDERR  2

uv_loop_t *loop;
uv_process_t child_req;
uv_process_options_t options;

void uv_on_exit(uv_process_t *req, int64_t exit_status, int term_signal) {
    fprintf(stderr, "Process exited with status %" PRId64 ", signal %d\n", exit_status, term_signal);
    uv_close((uv_handle_t*) req->data, NULL);
    uv_close((uv_handle_t*) req, NULL);
}

void invoke_cgi_script(uv_tcp_t *client) {
    size_t size = 500;
    char path[size];
    uv_exepath(path, &size);
    strcpy(path + (strlen(path) - strlen("client.exe")), "cgi/tick.exe");

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

void on_new_connection(uv_stream_t *server, int status) {
    if (status == -1) {
        // error!
        return;
    }

    uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, client);
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        invoke_cgi_script(client);
    }
    else {
        uv_close((uv_handle_t*) client, NULL);
    }
}

int main() {
    loop = uv_default_loop();

    uv_tcp_t server;
    uv_tcp_init(loop, &server);

    struct sockaddr_in bind_addr;
    uv_ip4_addr("0.0.0.0", 7000, &bind_addr);
    uv_tcp_bind(&server, (const struct sockaddr *)&bind_addr, 0);
    int r = uv_listen((uv_stream_t*) &server, 128, on_new_connection);
    if (r) {
        fprintf(stderr, "Listen error %s\n", uv_err_name(r));
        return 1;
    }
    return uv_run(loop, UV_RUN_DEFAULT);
}
