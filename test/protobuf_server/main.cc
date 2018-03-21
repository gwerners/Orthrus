#include <fstream>
#include <inttypes.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>
/*
GET / HTTP/1.1
Host: 127.0.0.1:7000
User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:42.0) Gecko/20100101
Firefox/42.0 Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*
/*;q=0.8 Accept-Language: en-US,en;q=0.5 Accept-Encoding: gzip, deflate
Connection: keep-alive
Cache-Control: max-age=0





bool SerializeToString(string* output) const;: serializes the message and stores
the bytes in the given string. Note that the bytes are binary, not text; we only
use the string class as a convenient container. bool ParseFromString(const
string& data);: parses a message from the given string.


void on_new_connection(uv_stream_t *server, int status) {
    if (status == -1) {
        // error!
        return;
    }

    uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, client);
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
         struct child_worker *worker = &workers[round_robin_counter];
        //now write the protobuffer!!!! (dummy sample)
        char message[256];
        int size;
         uv_os_fd_t fd;
         uv_fileno((const uv_handle_t*) client, &fd);
        sprintf(message,"mensagem vinda do servidor!pid[%d]
fd[%d]",getpid(),fd); size=strlen(message)+1; write_req_t *req = (write_req_t*)
malloc(sizeof(write_req_t)); req->buf = uv_buf_init((char*) malloc(size), size);
        memcpy(req->buf.base,message,size);
        req->buf.len=size;
        uv_write((uv_write_t*) req, (uv_stream_t*) &worker->pipe, &req->buf, 1,
on_file_write);


         //write the socket handle!

        uv_write_t *write_req = (uv_write_t*) malloc(sizeof(uv_write_t));
        dummy_buf = uv_buf_init(".", 1);//1 char message to avoid problems on
receiving fd on client side

        uv_write2(write_req, (uv_stream_t*) &worker->pipe, &dummy_buf, 1,
(uv_stream_t*) client, NULL); round_robin_counter = (round_robin_counter + 1) %
child_worker_count;
    }
    else {
        uv_close((uv_handle_t*) client, NULL);
    }
}

*/
extern "C" {
#include <uv.h>
}
#include "request.pb.h"

using namespace std;

uv_loop_t* loop;
int num_request = 1;
struct child_worker
{
  uv_process_t req;
  uv_process_options_t options;
  uv_pipe_t pipe;
} * workers;

int round_robin_counter;
int child_worker_count;

uv_buf_t dummy_buf;
char worker_path[500];

void
close_process_handle(uv_process_t* req, int64_t exit_status, int term_signal)
{
  fprintf(stderr,
          "Process exited with status %" PRId64 ", signal %d\n",
          exit_status,
          term_signal);
  uv_close((uv_handle_t*)req, NULL);
}

void
alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
  buf->base = (char*)malloc(suggested_size);
  buf->len = suggested_size;
}
typedef struct
{
  uv_write_t req;
  uv_buf_t buf;
} write_req_t;

void
free_write_req(uv_write_t* req)
{
  write_req_t* wr = (write_req_t*)req;
  free(wr->buf.base);
  free(wr);
}
void
on_file_write(uv_write_t* req, int status)
{
  free_write_req(req);
}
void
rest_write(uv_write_t* req, int status)
{
  ++num_request;
  if (status) {
    fprintf(stderr, "Write error %s\n", uv_err_name(status));
  }
  free(req);
}

void
rest_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
  if (nread <= 0) {
    if (nread != UV_EOF)
      fprintf(stderr, "Read error %s\n", uv_err_name(nread));
    uv_close((uv_handle_t*)client, NULL);
    return;
  }
  // parse rest
  // remove client from reading queue!
  uv_read_stop(client);

  // pass protobuffer and handle to client
  free(buf->base);
}
void
on_new_connection(uv_stream_t* server, int status)
{
  if (status < 0) {
    fprintf(stderr, "New connection error %s\n", uv_strerror(status));
    // error!
    return;
  }

  uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
  uv_tcp_init(loop, client);
  if (uv_accept(server, (uv_stream_t*)client) == 0) {
    uv_read_start((uv_stream_t*)client, alloc_buffer, rest_read);
  } else {
    uv_close((uv_handle_t*)client, NULL);
  }
}

void
setup_workers()
{
  size_t path_size = 500;
  uv_exepath(worker_path, &path_size);
  strcpy(worker_path + (strlen(worker_path) - strlen("server.exe")),
         "worker.exe");
  fprintf(stderr, "Worker path: %s\n", worker_path);

  char* args[2];
  args[0] = worker_path;
  args[1] = NULL;

  round_robin_counter = 0;

  // ...

  // launch same number of workers as number of CPUs
  uv_cpu_info_t* info;
  int cpu_count;
  uv_cpu_info(&info, &cpu_count);
  uv_free_cpu_info(info, cpu_count);

  child_worker_count = cpu_count;

  workers =
    (struct child_worker*)calloc(sizeof(struct child_worker), cpu_count);
  while (cpu_count--) {
    struct child_worker* worker = &workers[cpu_count];
    uv_pipe_init(loop, &worker->pipe, 1);

    uv_stdio_container_t child_stdio[3];
    child_stdio[0].flags = (uv_stdio_flags)(UV_CREATE_PIPE | UV_READABLE_PIPE);
    child_stdio[0].data.stream = (uv_stream_t*)&worker->pipe;
    child_stdio[1].flags = UV_IGNORE;
    child_stdio[2].flags = UV_INHERIT_FD;
    child_stdio[2].data.fd = 2;

    worker->options.stdio = child_stdio;
    worker->options.stdio_count = 3;

    worker->options.exit_cb = close_process_handle;
    worker->options.file = args[0];
    worker->options.args = args;

    uv_spawn(loop, &worker->req, &worker->options);
    fprintf(stderr, "Started worker %d\n", worker->req.pid);
  }
}

void
PromptForRequest(interfaces::Request* request)
{
  // cout << "Enter socket: ";
  // int socket;
  // cin >> socket;
  // request->set_socket(socket);
  cin.ignore(256, '\n');

  cout << "Enter path: ";
  getline(cin, *request->mutable_path());

  cout << "Enter method(GET,HEAD,POST,PUT,DELETE,CONNECT,OPTIONS,TRACE):";

  string type;
  getline(cin, type);
  if (type == "GET") {
    request->set_type(interfaces::Request::GET);
  } else if (type == "HEAD") {
    request->set_type(interfaces::Request::HEAD);
  } else if (type == "POST") {
    request->set_type(interfaces::Request::POST);
  } else if (type == "PUT") {
    request->set_type(interfaces::Request::PUT);
  } else if (type == "DELETE") {
    request->set_type(interfaces::Request::DELETE);
  } else if (type == "CONNECT") {
    request->set_type(interfaces::Request::CONNECT);
  } else if (type == "OPTIONS") {
    request->set_type(interfaces::Request::OPTIONS);
  } else if (type == "TRACE") {
    request->set_type(interfaces::Request::TRACE);
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

    interfaces::Request::Parameters* parameter = request->add_params();
    parameter->set_key(key);
    cout << "value: ";
    string value;
    getline(cin, value);
    parameter->set_value(value);
  }
  cout << "Enter body: ";
  getline(cin, *request->mutable_body());
}
void
ListRequests(const interfaces::MessageBlock& msg)
{
  for (int i = 0; i < msg.request_size(); i++) {
    const interfaces::Request& request = msg.request(i);
    cout << "###########################################" << endl;
    // cout << " socket: " << request.socket() << endl;
    cout << " path: " << request.path() << endl;

    cout << " Method : ";
    switch (request.type()) {
      case interfaces::Request::GET:
        cout << "GET" << endl;
        break;
      case interfaces::Request::HEAD:
        cout << "HEAD" << endl;
        break;
      case interfaces::Request::POST:
        cout << "POST" << endl;
        break;
      case interfaces::Request::PUT:
        cout << "PUT" << endl;
        break;
      case interfaces::Request::DELETE:
        cout << "DELETE" << endl;
        break;
      case interfaces::Request::CONNECT:
        cout << "CONNECT" << endl;
        break;
      case interfaces::Request::OPTIONS:
        cout << "OPTIONS" << endl;
        break;
      case interfaces::Request::TRACE:
        cout << "TRACE" << endl;
        break;
    }

    if (request.params_size() > 0) {
      cout << " Parameters : " << endl;
      for (int j = 0; j < request.params_size(); j++) {
        const interfaces::Request::Parameters& parameter =
          request.params(j);

        cout << "  " << parameter.key() << ":" << parameter.value() << endl;
      }
    }
    cout << " body: " << request.body() << endl;
  }
}

int
test_write(int argc, char** argv)
{

  if (argc != 2) {
    cerr << "Usage:  " << argv[0] << " REQUEST_FILE" << endl;
    return -1;
  }

  interfaces::MessageBlock msg;

  {
    // Read the existing request
    fstream input(argv[1], ios::in | ios::binary);
    cout << "filename:" << argv[1] << endl;
    if (!input) {
      cout << argv[1] << ": File not found.  Creating a new file." << endl;
    } else if (!msg.ParseFromIstream(&input)) {
      cerr << "Failed to parse request." << endl;
      return -1;
    }
  }

  // Add an address.
  PromptForRequest(msg.add_request());

  {
    // Write the new address book back to disk.
    fstream output(argv[1], ios::out | ios::trunc | ios::binary);
    if (!msg.SerializeToOstream(&output)) {
      cerr << "Failed to request." << endl;
      return -1;
    }
  }
}

int
test_read(int argc, char** argv)
{

  if (argc != 2) {
    cerr << "Usage:  " << argv[0] << " REQUEST_FILE" << endl;
    return -1;
  }

  interfaces::MessageBlock msg;

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

int
main()
{
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  //;test_write(argc,argv);
  // test_read(argc,argv);

  // Optional:  Delete all global objects allocated by libprotobuf.
  // google::protobuf::ShutdownProtobufLibrary();

  loop = uv_default_loop();

  setup_workers();

  uv_tcp_t server;
  uv_tcp_init(loop, &server);

  struct sockaddr_in bind_addr;
  uv_ip4_addr("0.0.0.0", 7000, &bind_addr);
  uv_tcp_bind(&server, (const struct sockaddr*)&bind_addr, 0);
  int r;
  if ((r = uv_listen((uv_stream_t*)&server, 128, on_new_connection))) {
    fprintf(stderr, "Listen error %s\n", uv_err_name(r));
    return 2;
  }
  return uv_run(loop, UV_RUN_DEFAULT);
}
