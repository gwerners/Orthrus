
#include <assert.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
extern "C" {
#include <uv.h>
}
#include "request.pb.h"

using namespace std;

int num_request = 1;
uv_loop_t* loop;

uv_pipe_t queue;
uv_pipe_t stdin_pipe;
uv_pipe_t stdout_pipe;

void
alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
  buf->base = (char*)malloc(suggested_size);
  buf->len = suggested_size;
}

void
echo_write(uv_write_t* req, int status)
{
  ++num_request;
  if (status) {
    fprintf(stderr, "Write error %s\n", uv_err_name(status));
  }
  free(req);
}

void
echo_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
  char header[1024];
  char page[1024];
  char page2[1024];
  char body[1024];
  header[0] = '\0';
  page[0] = '\0';
  page2[0] = '\0';
  body[0] = '\0';
  printf("http header[%s]\n", buf->base);
  fflush(stdout);

  strcat(header, "HTTP/1.0 200 OK\n");
  strcat(header, "Date: Fri, 31 Dec 1999 23:59:59 GMT\n");
  strcat(header, "Content-Type: text/html\n");
  strcat(header, "Content-Length: %d\r\n\r\n%s");

  strcat(page, "<html>\n");
  strcat(page, " <head>\n");
  strcat(page, "  <title>this is the title</title>\n");
  strcat(page, " </head>\n");
  strcat(page, " <body>\n");
  strcat(page, "  <h1>My Heading</h1>\n");
  strcat(page, "  <p>Mypid is %d</p>\n");
  strcat(page, "  <p>This is request %d.</p>\n");
  strcat(page, " </body>\n");
  strcat(page, "</html>\n");
  sprintf(page2, page, getpid(), num_request);
  sprintf(body, header, strlen(page2), page2);

  if (nread < 0) {
    if (nread != UV_EOF)
      fprintf(stderr, "Read error %s\n", uv_err_name(nread));
    uv_close((uv_handle_t*)client, NULL);
    return;
  }
  printf("received[%s]\n", buf->base);
  fflush(stdout);
  uv_write_t* req = (uv_write_t*)malloc(sizeof(uv_write_t));
  uv_buf_t wrbuf = uv_buf_init(body, strlen(body));
  uv_write(req, client, &wrbuf, 1, echo_write);
  free(buf->base);
}

void
on_new_connection(uv_stream_t* q, ssize_t nread, const uv_buf_t* buf)
{

  if (nread < 0) {
    if (nread != UV_EOF)
      fprintf(stderr, "Read error %s\n", uv_err_name(nread));
    uv_close((uv_handle_t*)q, NULL);
    return;
  }

  fprintf(stderr, "the message is[%s]\n", buf->base);

  uv_pipe_t* pipe = (uv_pipe_t*)q;
  if (!uv_pipe_pending_count(pipe)) {
    fprintf(stderr, "No pending count\n");
    return;
  }

  uv_handle_type pending = uv_pipe_pending_type(pipe);
  assert(pending == UV_TCP);

  uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
  uv_tcp_init(loop, client);
  if (uv_accept(q, (uv_stream_t*)client) == 0) {
    uv_os_fd_t fd;
    uv_fileno((const uv_handle_t*)client, &fd);
    fprintf(stderr, "Worker %d: Accepted fd %d\n", getpid(), fd);
    uv_read_start((uv_stream_t*)client, alloc_buffer, echo_read);
  } else {
    uv_close((uv_handle_t*)client, NULL);
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
