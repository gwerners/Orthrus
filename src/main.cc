#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <regex.h>

#include <linux/limits.h>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <map>

#include <sstream>

//libuv
#include "uv.h"
#include "task.h"
#include "logger.h"
#include "plugin.h"
#include "request.pb.h"
#include "response.pb.h"
//lua functions
#include "lua_utils.h"

#define BO_JAVA 1
#define BO_CPP  2
#define BO_LUA  3

#define HPE_OK 0

#define HTTP_REQUEST  0
#define HTTP_RESPONSE 1
#define HTTP_BOTH     2

void *scanner=NULL;
char * java_class_path=NULL;

struct child_worker {
    uv_process_t req;
    uv_process_options_t options;
    uv_pipe_t pipe;
};

typedef struct Service_t
{
   int type;
   int instances;
   int round_robin_counter;
   regex_t       *preg;
   std::string regexp;
   std::string object;
   std::string name;
   struct child_worker *workers;
} Service;


typedef struct {
    uv_write_t req;
    uv_buf_t buf;
} write_req_t;

#define STDIN   0
#define STDOUT  1
#define STDERR  2

int server_closed=0;
int must_abort=0;

uv_loop_t *loop;

uv_tcp_t tcpServer;
uv_tcp_t tcpServerAdmin;

//uv_stream_t *last_client=NULL;

uv_process_t child_req;
uv_process_options_t options;

std::map<std::string,Service*> services;
//std::map<int,std::string> request;

static void get_process_name(const pid_t pid, char * name) {
	char procfile[BUFSIZ];
	sprintf(procfile, "/proc/%d/cmdline", pid);
	FILE* f = fopen(procfile, "r");
	if (f) {
		size_t size;
		size = fread(name, sizeof (char), sizeof (procfile), f);
		if (size > 0) {
         //log_debug("pname [%s]",name);
			if ('\n' == name[size - 1])
				name[size - 1] = '\0';
		}
		fclose(f);
	}
}
static void get_process_parent_id(const pid_t pid, pid_t * ppid)
{
   char *ptr;
	char buffer[BUFSIZ];
	sprintf(buffer, "/proc/%d/stat", pid);
	FILE* fp = fopen(buffer, "r");
   *ppid=0;
	if (fp)
   {
		size_t size = fread(buffer, sizeof (char), sizeof (buffer), fp);
		if (size > 0)
      {
			// See: http://man7.org/linux/man-pages/man5/proc.5.html section /proc/[pid]/stat
			ptr=strtok(buffer, " "); // (1) pid  %d
         //log_debug("pid %s",ptr);
			ptr=strtok(NULL, " "); // (2) comm  %s
         //log_debug("comm %s",ptr);
			ptr=strtok(NULL, " "); // (3) state  %c
         //log_debug("state %s",ptr);
			char * s_ppid = strtok(NULL, " "); // (4) ppid  %d
         //log_debug("ppid %s",s_ppid);
			*ppid = atoi(s_ppid);
		}
		fclose(fp);
	}

}
void cleanup_bo()
{
   struct child_worker *worker;
   pid_t child_pid;
   pid_t ppid;
   int i;
   std::map<std::string,Service*>::iterator itor;
   Service *p_service;
   for(itor=services.begin();itor!=services.end();itor++)
   {
      p_service = itor->second;
      for(i=0;i<p_service->instances;i++)
      {
         worker=&p_service->workers[i];

	      char name[BUFSIZ];
	      //get_process_name(worker->req.pid, name);
	      //log_debug("worker %6d - %s", worker->req.pid, name);

         log_debug("send kill to all child processes if they are my children!");
         get_process_parent_id(worker->req.pid,&ppid);
         //if the parent process is the server, then kill this client
         log_debug("ppid[%d] my pid[%d]",ppid,getpid());
         if(ppid == getpid())
         {
            log_debug("found child to kill!");
            uv_process_kill(&worker->req,SIGKILL);
         }

      }
      //free(p_service->workers);// coredump! do not uncomment!
      regfree(p_service->preg);
      //delete p_service->preg;// codedump! do not uncomment!
      delete p_service;
   }
   services.clear();
}


static const char * str_regcomp_error(int vlr)
{
   switch(vlr)
   {
      case REG_BADBR:   return "Invalid use of back reference operator.";
      case REG_BADPAT:  return "Invalid use of pattern operators such as group or list.";
      case REG_BADRPT:  return "Invalid use of repetition operators such as using '*' as the first character.";
      case REG_EBRACE:  return "Un-matched brace interval operators.";
      case REG_EBRACK:  return "Un-matched bracket list operators.";
      case REG_ECOLLATE:return "Invalid collating element.";
      case REG_ECTYPE:  return "Unknown character class name.";
#ifdef _linux_
      case REG_EEND:    return "Nonspecific error. This is not defined by POSIX.2.";
#endif
      case REG_EESCAPE: return "Trailing backslash.";
      case REG_EPAREN:  return "Un-matched parenthesis group operators.";
      case REG_ERANGE:  return "Invalid use of the range operator, e.g., the ending point of the range occurs prior to the starting point.";
#ifdef _linux_
      case REG_ESIZE:   return "Compiled regular expression requires a pattern buffer larger than 64Kb. This is not defined by POSIX.2.";
#endif
      case REG_ESPACE:  return "The regex routines ran out of memory.";
      case REG_ESUBREG: return "Invalid back reference to a subexpression.";
   }
   return "unknow error code!";
}
//#############################################################################
static void on_close(uv_handle_t* peer)
{
   log_debug("onclose");
   free(peer);
}

static void after_shutdown(uv_shutdown_t* req, int status)
{
  uv_close((uv_handle_t*)req->handle, on_close);
  free(req);
}

static void close_process_handle(uv_process_t *req, int64_t exit_status, int term_signal)
{
   struct child_worker *worker;
   pid_t child_pid;
   pid_t ppid;
   int i;
   int found=0;
   std::string name;
   std::map<std::string,Service*>::iterator itor;

   Service *p_service;
   log_debug("Service child closed[%d]",req->pid);
   for(itor=services.begin();itor!=services.end();itor++)
   {
      name = itor->first;
      p_service = itor->second;
      for(i=0;i<p_service->instances;i++)
      {
         if(p_service->workers[i].req.pid==req->pid){
            found = 1;
            break;
         }
      }
      if(found)
      {
         log_debug("Service[%s] removed from service list!",itor->first.c_str());
         for(i=0;i<p_service->instances;i++)
         {
            worker=&p_service->workers[i];
            log_debug("worker[%d] vs req.pid[%d] ",worker->req.pid,req->pid);
            if(worker->req.pid != req->pid)
            {
               log_debug("send kill to all child processes if they are my children!");
               get_process_parent_id(worker->req.pid,&ppid);
               //if the parent process is the server, then kill this client
               log_debug("ppid[%d] my pid[%d]",ppid,getpid());
               if(ppid == getpid())
               {
                  log_debug("found child to kill!");
                  uv_process_kill(&worker->req,SIGKILL);
               }
            }
         }
         //free(p_service->workers);// coredump! do not uncomment!
         regfree(p_service->preg);// codedump! do not uncomment!
         //delete p_service->preg;
         delete p_service;
         log_debug("child removal completed!");
         break;
      }
   }
   if(found)
   {
      //itor= services.find(name);
      services.erase(name);
   }else{
      log_debug("cleanup skiped, another child dying...");
   }
   log_debug("Process[%d] exited with status %" PRId64 ", signal %d",req->pid, exit_status, term_signal);
   uv_close((uv_handle_t*) req, NULL);
}

static int FileExists(const char*filename)
{
   int ret;
   struct stat buf;
   ret=stat(filename, &buf);
   if(ret==-1)
      return 0;
   if(buf.st_mode & S_IXUSR) //check execute permission!
      return 1;
   return 0;
}
static void buf_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
   buf->base = (char*)calloc(1,suggested_size);
   buf->len = suggested_size;
}
void after_read_pipe(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf)
{
    if (nread < 0) {
        if (nread != UV_EOF)
            log_error("Read error %s", uv_err_name(nread));
        if (buf->base) free(buf->base);
        return;
    }
    log_debug("PIPE received[%s]",buf->base);
    if(buf->base)free(buf->base);
}
void setup_workers(Service * service)
{
   int ret;
   size_t path_size = PATH_MAX;
   char current_path[PATH_MAX];
   char worker_path[PATH_MAX];
   uv_cpu_info_t *info;
   int cpu_count,index=0;
   struct child_worker * worker;

   char* args[3];

   uv_cwd(current_path, &path_size);
   log_debug("current path: %s", current_path);

   sprintf(worker_path,"%s%s",current_path, service->object.c_str());
   log_debug("Worker path: %s", worker_path);
   if(FileExists(worker_path))
   {
      log_debug("File exists[Worker] %s", worker_path);
   }else{
      log_error("File is missing![Worker] %s", worker_path);
      if(must_abort)exit(1);
      service->workers=NULL;
      return;
   }

   args[0] = worker_path;

   if(service->type==BO_JAVA){
      log_debug("service->type %d",service->type);
      args[1] = java_class_path;
      args[2] = NULL;
   }else{
      log_debug("service->type %d",service->type);
      args[1] = NULL;
      args[2] = NULL;
   }

   uv_cpu_info(&info, &cpu_count);
   uv_free_cpu_info(info, cpu_count);

   if(!service->instances)
   {
      service->instances = cpu_count;
   }else{
      cpu_count = service->instances;
   }

   service->workers = (struct child_worker*)calloc(sizeof(struct child_worker), cpu_count);
   while (cpu_count--)
   {
      struct child_worker *worker = &service->workers[cpu_count];

      uv_pipe_init(loop, &worker->pipe, 1);

      uv_stdio_container_t child_stdio[3];
      child_stdio[0].flags = (uv_stdio_flags)(UV_CREATE_PIPE | UV_READABLE_PIPE | UV_WRITABLE_PIPE);
      child_stdio[0].data.stream = (uv_stream_t*) &worker->pipe;
      child_stdio[1].flags = UV_IGNORE;

      child_stdio[2].flags = UV_INHERIT_FD;
      child_stdio[2].data.fd = 2;

      worker->options.stdio = child_stdio;
      worker->options.stdio_count = 3;

      worker->options.exit_cb = close_process_handle;
      worker->options.file = args[0];
      worker->options.args = args;

      uv_spawn(loop, &worker->req, &worker->options);

      log_info("Started worker %d", worker->req.pid);
      ret = uv_read_start((uv_stream_t*) &worker->pipe, buf_alloc, after_read_pipe);
   }
}
//#############################################################################

static void on_server_close(uv_handle_t* handle)
{
   //ASSERT(handle == server);
   log_debug("onserverclose");
   uv_stop(loop);
}
void int_handler(uv_signal_t *handle, int signum)
{
   log_debug("SIGINT received: %d", signum);
   //uv_signal_stop(handle);//uninstall the signal handler!
   uv_stop(loop);//stop the main loop!
   fflush(log_output);

   //cleanup_bo();
}

void usr1_handler(uv_signal_t *handle, int signum)
{
   log_debug("SIGUSR1 received: %d", signum);
}

void usr2_handler(uv_signal_t *handle, int signum)
{
   log_debug("SIGUSR2 received: %d", signum);
}

void uv_on_exit(uv_process_t *req, int64_t exit_status, int term_signal) {
    log_debug("Process exited signal %d", term_signal);
    uv_close((uv_handle_t*) req->data, NULL);
    uv_close((uv_handle_t*) req, NULL);
}

static void check_sockname(struct sockaddr* addr, const char* compare_ip, int compare_port, const char* context)
{
   struct sockaddr_in check_addr = *(struct sockaddr_in*) addr;
   struct sockaddr_in compare_addr;
   char check_ip[17];
   int r;

   ASSERT(0 == uv_ip4_addr(compare_ip, compare_port, &compare_addr));

   /* Both addresses should be ipv4 */
   ASSERT(check_addr.sin_family == AF_INET);
   ASSERT(compare_addr.sin_family == AF_INET);

   /* Check if the ip matches */
   //ASSERT(memcmp(&check_addr.sin_addr, &compare_addr.sin_addr, sizeof compare_addr.sin_addr) == 0);

   /* Check if the port matches. If port == 0 anything goes. */
   ASSERT(compare_port == 0 || check_addr.sin_port == compare_addr.sin_port);

   r = uv_ip4_name(&check_addr, (char*) check_ip, sizeof check_ip);
   ASSERT(r == 0);

   log_debug("%s: %s:%d", context, check_ip, ntohs(check_addr.sin_port));
}

void after_write(uv_write_t *req, int status)
{
    write_req_t *wreq=(write_req_t*)req;
    if (status) {
        log_error("Write error %s", uv_err_name(status));
    }
    log_debug("wrote");

    //if(wreq->buf.base)free(wreq->buf.base);
    free(wreq);
}
void after_read_admin(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf)
{
   uv_shutdown_t* sreq;
   char *body="<body> HA </body>";
    if (nread < 0) {
        if (nread != UV_EOF)
            log_error("Read error %s", uv_err_name(nread));
        if (buf->base) free(buf->base);
        sreq = (uv_shutdown_t*) malloc(sizeof *sreq);
        uv_shutdown(sreq, client, after_shutdown);
        return;
    }
    log_debug("received[%s]",buf->base);

   //basic close
   if(strncmp(buf->base,"quit",4)==0)//close stream
   {
      if(buf->base)free(buf->base);
      uv_close((uv_handle_t*)client, on_close);
      return;
   }
   if(strncmp(buf->base,"squit",5)==0)//server close
   {
      uv_close((uv_handle_t*)&tcpServer, on_server_close);
      uv_close((uv_handle_t*)&tcpServerAdmin, on_server_close);
      server_closed = 1;
   }


    write_req_t *req = (write_req_t *) malloc(sizeof(write_req_t));
    /*req->buf.base = strdup(body);//uv_buf_init(body, strlen(body));
    req->buf.len  = strlen(body)+1;*/
    req->buf = uv_buf_init(body, strlen(body));
    uv_write((uv_write_t*)req, client, &req->buf, 1, after_write);
    if(buf->base)free(buf->base);
}
void after_write_admin(uv_write_t *req, int status)
{
   write_req_t *wreq=(write_req_t*)req;
    if (status) {
        log_error("Write error %s", uv_err_name(status));
    }
    //free(wreq->buf.base);
    //free(wreq->req.data);
    free(wreq);
}
void after_write_close(uv_write_t *req, int status)
{
   write_req_t*wreq=(write_req_t*)req;
   log_debug("after write close");
    if (status) {
        log_error("Write error %s", uv_err_name(status));
    }
    //if(last_client)
    //  uv_close((uv_handle_t*) last_client, NULL);
    //free(wreq->buf.base);
    //free(wreq->req.data);
    free(wreq);
    //last_client=NULL;
}
void after_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf)
{
   write_req_t *req;
   uv_shutdown_t* sreq;
   int size;
   struct child_worker *worker;
   uv_os_fd_t fd;
   std::map<std::string,Service*>::iterator itor;
   std::string str_output;
   Service *p_service;
   const char* service_name="M4U_CORE";
   const char*match;
   m4u_interfaces::Request request;
   size_t     nmatch = 1;
   regmatch_t pmatch[1];
   int ret;
   int found,i;
   char *output,*path;
   if (nread < 0) {
      if (nread != UV_EOF)
         log_error("Read error %s", uv_err_name(nread));
      if (buf->base) free(buf->base);
      sreq = (uv_shutdown_t*) malloc(sizeof *sreq);
      uv_shutdown(sreq, client, after_shutdown);
      return;
   }

   log_debug("received(%lu)[%s]",buf->len,buf->base);

   //ret=scanner_plugin.p_Scanner(buf->base,buf->base+buf->len,&path,&output,0);
   ret=scanner_plugin.p_scan(scanner,service_name,buf->base,strlen(buf->base),&path,&output,0);

   log_debug("scanner [%d:%s]",ret,scanner_plugin.p_str_err(ret));
   if(ret==HPE_OK)
   {
      log_debug("parsing success[%s]",path);
      //log_debug("%s",output);
   }else {
      log_error("parsing failure[%s]",buf->base);

      /*for (i = 0; i < strlen(buf->base); i++)
      {
          if(isprint(buf->base[i])){
            if(buf->base[i]==' ')
               _log_debug_(" ");
            else
               if(buf->base[i]=='\t')
                  _log_debug_("\\t");
               else
                  _log_debug_("%c",buf->base[i]);
          }else{
            if(buf->base[i]=='\r')
               _log_debug_("\\r");
            else
               if(buf->base[i]=='\n')
                  _log_debug_("\\n\n");
               else
                  _log_debug_("%02X", buf->base[i]);
          }
      }*/
   }
   if(path){
      match=path;
      p_service=NULL;
      found=0;
      i=1;
      for(itor=services.begin();itor!=services.end();itor++)
      {
         log_debug("%d service[%s] try match[%s]",i++,itor->first.c_str(),match);
         p_service = itor->second;
         ret = regexec(p_service->preg,match, nmatch , pmatch, 0);
         if(!ret)
         {
            log_debug("With the whole expression, "
               "a matched substring \"%.*s\" is found at position %d to %d",
               pmatch[0].rm_eo - pmatch[0].rm_so, &match[pmatch[0].rm_so],
               pmatch[0].rm_so, pmatch[0].rm_eo - 1);
            log_debug("matched[%s] against [%s]",match,p_service->regexp.c_str());
            found=1;
            break;
         }

      }
      if (!found)
      {
         log_debug("Failed to match '%s' with '%s',returning %d.", match, p_service->regexp.c_str(), ret);
         p_service=NULL;
         req = (write_req_t*) malloc(sizeof(write_req_t));
         const char* r404="HTTP/1.1 404 Not Found\r\nContent-Length: 35\r\nConnection: close\r\n\r\n<html><h1>404 NOT FOUND</h1></html>\r\n";

         /*req->buf.base = strdup((char*)r404); //= uv_buf_init((char*)r404, strlen(r404)+1);
         req->buf.len  = strlen(r404)+1;*/
         req->buf = uv_buf_init((char*)r404, strlen(r404)+1);
         log_debug("msg[%s]",r404);
         uv_write((uv_write_t*) req, (uv_stream_t*) client, &req->buf , 1, after_write_close);
         if (buf->base) free(buf->base);
         uv_read_stop(client);
         uv_close((uv_handle_t*) client, on_close);

         if(path)free(path);
         if(output)free(output);
         return;
      }
      if(p_service && p_service ->workers)
      {
         uv_fileno((const uv_handle_t*) client, &fd);
         worker = &p_service->workers[p_service->round_robin_counter];
         req = (write_req_t*) malloc(sizeof(write_req_t));
         /*req->buf.base = strdup("a");//uv_buf_init("a", 1);
         req->buf.len  = 1;*/
         req->buf = uv_buf_init("a", 1);

         uv_write2((uv_write_t*)req, (uv_stream_t*) &worker->pipe, &req->buf, 1, (uv_stream_t*) client, after_write);
         p_service->round_robin_counter = (p_service->round_robin_counter + 1) % p_service->instances;

         //fd is in the client space, close here to avoid problems with listen maxfd backlog!
         close(fd);
//#ifdef INSERT_PROTOBUFFER
         if(request.ParseFromString(output))
         {
            log_debug("parsing output ok");
            request.set_name(p_service->name);
            request.set_id(fd);
         }
         log_debug("before serialize");
         if(request.SerializeToString(&str_output))
         {
            //log_debug("serializing[%s]",(char*)str_output.c_str());
            req = (write_req_t*) malloc(sizeof(write_req_t));

            /*req->buf.base = strdup((char*)str_output.c_str());//uv_buf_init((char*)str_output.c_str(), str_output.length()+1);
            req->buf.len  = str_output.length()+1;*/
            req->buf = uv_buf_init((char*)str_output.c_str(), str_output.length()+1);

            uv_write((uv_write_t*)req, (uv_stream_t*) &worker->pipe, &req->buf, 1, after_write);
         }
/*#else

            log_debug("serializing");

            write_req = (uv_write_t*) malloc(sizeof(uv_write_t));

            dummy_buf = uv_buf_init((char*)output, strlen(output)+1);

            uv_write((uv_write_t*)write_req, (uv_stream_t*) &worker->pipe, &dummy_buf, 1, after_write);

#endif*/

      }
   }
   if(path)free(path);
   if(output)free(output);
   if(buf->base)free(buf->base);
   //remove client from queue
   uv_read_stop(client);
   uv_close((uv_handle_t*) client, on_close);
}
void on_connection(uv_stream_t *server, int status)
{
   int ret;
   struct sockaddr sockname, peername;
   int namelen=sizeof peername;
   if (status == -1) {
      // error!
      log_error("status == -1");
      return;
   }
   log_debug("connection");
   uv_stream_t *client = (uv_stream_t*) malloc(sizeof(uv_tcp_t));
   uv_tcp_init(loop,(uv_tcp_t*) client);

   if (uv_accept(server, (uv_stream_t*) client) == 0) {
      ret = uv_tcp_getpeername((const uv_tcp_t*)client, &peername, &namelen);
      check_sockname(&peername, "127.0.0.1", 0/*anything match*/, "client");
   } else {
      uv_shutdown_t* sreq;
      sreq = (uv_shutdown_t*) malloc(sizeof *sreq);
      uv_shutdown(sreq, client, after_shutdown);
      return;
   }
   ret = uv_read_start((uv_stream_t*)client, buf_alloc, after_read);
}
void on_connection_admin(uv_stream_t *server, int status)
{
   int ret;
   struct sockaddr sockname, peername;
   int namelen=sizeof peername;
   if (status == -1) {
      // error!
      log_error("status == -1");
      return;
   }
   log_debug("connection ADMIN");
   uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
   uv_tcp_init(loop, client);
   if (uv_accept(server, (uv_stream_t*) client) == 0) {
      ret = uv_tcp_getpeername((const uv_tcp_t*)client, &peername, &namelen);
      check_sockname(&peername, "127.0.0.1", 0/*anything match*/, "client");
      //invoke_cgi_script(client,"tick");
      char *answer="<input admin command>";
      write_req_t *req = (write_req_t *) malloc(sizeof(write_req_t));
      /*req->buf.base = strdup(answer);//uv_buf_init(answer, strlen(answer));
      req->buf.len  = strlen(answer)+1;*/
      req->buf = uv_buf_init(answer, strlen(answer));
      uv_write((uv_write_t*)req, (uv_stream_t*)client, &req->buf, 1, after_write_admin);
   }else {
      uv_shutdown_t* sreq;
      sreq = (uv_shutdown_t*) malloc(sizeof *sreq);
      uv_shutdown(sreq,(uv_stream_s*) client, after_shutdown);
      return;
   }
   ret = uv_read_start((uv_stream_t*)client, buf_alloc, after_read_admin);
}

static int tcp4_start(uv_tcp_t *tcpServer,int port,void (*on_connection)(uv_stream_t *,int) )
{
   struct sockaddr_in addr;
   int r;
   uv_handle_t* server;

   //uv_ip4_addr("127.0.0.1", port, &addr);
   uv_ip4_addr("0.0.0.0", port, &addr);

   server = (uv_handle_t*)tcpServer;

   r = uv_tcp_init(loop, tcpServer);
   if (r)
   {
      /* TODO: Error codes */
      log_error("Socket creation error[%d]",port);
      if(must_abort)exit(1);
      return 1;
   }

   r = uv_tcp_bind(tcpServer, (const struct sockaddr*) &addr, 0);
   if (r)
   {
      /* TODO: Error codes */
      log_error("Bind error[%d]",port);
      if(must_abort)exit(1);
      return 1;
   }

   r = uv_listen((uv_stream_t*)tcpServer, SOMAXCONN, on_connection);
   if (r)
   {
      /* TODO: Error codes */
      log_error("Listen error %s[%d]", uv_err_name(r),port);
      if(must_abort)exit(1);
      return 1;
   }

   log_debug("Server Listening to port[%d]",port);
   return 0;
}
uv_loop_t* create_loop()
{
   uv_loop_t *loop = (uv_loop_t *)malloc(sizeof(uv_loop_t));
   if (loop) {
      uv_loop_init(loop);
   }
   return loop;
}

static int CreateTcp4Server (lua_State *L)
{
   int port= luaL_checkint(L,1);
   tcp4_start(&tcpServer,port,on_connection);
   return 0;
}
static int CreateTcp4AdminServer (lua_State *L)
{
   int port= luaL_checkint(L,1);
   tcp4_start(&tcpServerAdmin,port,on_connection_admin);
   return 0;
}
static int EnableSigInt(lua_State *L)
{
   uv_signal_t *sig1a=(uv_signal_t *)malloc(sizeof(uv_signal_t));
   log_debug("EnableSigInt");
   uv_signal_init(loop, sig1a);
   uv_signal_start(sig1a, int_handler, SIGINT);
   return 0;
}

static int EnableSigUSR1(lua_State *L)
{
   uv_signal_t *sig1b=(uv_signal_t *)malloc(sizeof(uv_signal_t));
   uv_signal_init(loop, sig1b);
   uv_signal_start(sig1b, usr1_handler, SIGUSR1);
   return 0;
}

static int EnableSigUSR2(lua_State *L)
{
   uv_signal_t *sig1c=(uv_signal_t *)malloc(sizeof(uv_signal_t));
   uv_signal_init(loop, sig1c);
   uv_signal_start(sig1c, usr2_handler, SIGUSR2);
   return 0;
}

static int ProcessEvents(lua_State *L)
{
   log_debug("processing events...");
   uv_run(loop, UV_RUN_DEFAULT);
   return 0;
}

static int AbortOnError(lua_State *L)
{
   int ret = lua_isboolean(L, 1);
   if(ret){
      ret= lua_toboolean(L, 1);
      if(ret)
         must_abort = 1;
      else
         must_abort = 0;
      log_debug("AbortOnError[%s]",ret?"true":"false");
   }else{
      log_debug("AbortOnError not boolean!");
      if(must_abort)exit(1);
   }
   return 0;
}
void add_bo(const char *name,int type,const char*expression,int instances,const char*path)
{
   Service *p_service;
   int ret;
   regex_t *preg=new regex_t;
   p_service = new Service;
   p_service->name=name;
   p_service->type=type;
   p_service->instances=instances;
   ret = regcomp(preg,expression,REG_EXTENDED);
   if(ret)
   {
      log_error("error compiling expression[%s](%d:%s)",expression,ret,str_regcomp_error(ret));
      if(must_abort)exit(1);
   }
   p_service->preg=preg;
   p_service->regexp=expression;
   p_service->object=path;
   p_service->round_robin_counter=0;
   services.insert(std::pair<std::string,Service*>(name,p_service));
   log_debug("%s,%d,%s,%d,%s",name,type,expression,instances,path);
   setup_workers(p_service);
}
static int AddBusinessObject(lua_State *L)
{
   //AddBusinessObject("SMS",BO_JAVA,"/SMS/[0-9]*/[0-9]*",1,"BO/JAVA/SMS/Sample.class")
   //AddBusinessObject("SMS",BO_JAVA,"/SMS/[0-9]*/[0-9]*",1,"BO/JAVA/SMS/Sample.class")

   const char *name       = luaL_checkstring(L, 1);
   int type               = luaL_checkint(L,2);
   const char *expression = luaL_checkstring(L, 3);
   int instances          = luaL_checkint(L,4);
   const char *path       = luaL_checkstring(L, 5);

   add_bo(name,type,expression,instances,path);
   return 0;
}
static int RemoveBusinessObject(lua_State *L)
{
   Service *p_service;
   std::map<std::string,Service*>::iterator itor;
   const char *name   = luaL_checkstring(L, 1);
   itor=services.find(name);
   if (itor != services.end())
   {
      p_service = itor->second;
      regfree(p_service->preg);
      delete p_service->preg;
      delete p_service;
      services.erase(itor);
      log_debug("removed Object[%s]",name);
   }
   return 0;
}

static int ListBusinessObjects(lua_State *L)
{
   std::map<std::string,Service*>::iterator itor;
   Service *p_service;
   std::stringstream ss;
   for(itor=services.begin();itor!=services.end();itor++)
   {
      ss << "\nServico[" << itor->first << "]";
      p_service = itor->second;
      if (p_service->instances<=0)
         ss << "\n\tinstancias=" << "one-per-cpu-available";
      else
         ss << "\n\tinstancias=" << p_service->instances ;
      ss << "\n\ttype=" << p_service->type ;
      ss << "\n\tregexp=" << p_service->regexp ;
      ss << "\n\tobject=" << p_service->object ;
   }
   lua_pushstring(L,ss.str().c_str());
   return 1;
}

int load_plugin_library(const char *library_name,const char *load_function,const char *load_dummy_function)
{
   void(*PluginLoad)(const char*)=NULL;
   void(*PluginLoadDummy)()=NULL;
   if(strcmp(load_function,"LoadScannerPlugin")==0)
      PluginLoad=LoadScannerPlugin;

   if(strcmp(load_dummy_function,"LoadScannerPluginDummy")==0)
      PluginLoadDummy=LoadScannerPluginDummy;

   if(!PluginLoad)
   {
      log_error("unable to load plugin function[%s]",load_function);
      if(must_abort)exit(1);
      return 0;
   }

   if(!PluginLoadDummy)
   {
      log_error("unable to load plugin dummy function[%s]",load_dummy_function);
      if(must_abort)exit(1);
      return 0;
   }

   if(! LoadPlugin(library_name,PluginLoad,PluginLoadDummy))
   {
      if(must_abort)exit(1);
   }

   scanner = scanner_plugin.p_init(HTTP_REQUEST);
   log_debug("scanner plugin version[%lx]",scanner_plugin.p_version());
   return 0;
}
static int LoadPluginLibrary(lua_State *L)
{
   const char *library_name        = luaL_checkstring(L, 1);
   const char *load_function       = luaL_checkstring(L, 2);
   const char *load_dummy_function = luaL_checkstring(L, 3);
   load_plugin_library(library_name,load_function,load_dummy_function);
   return 0;
}

static int RestScanner(lua_State *L)
{
   int ret;
   const char *service_name="LUA_CORE";
   const char *str = luaL_checkstring(L, 1);
   char *output,*path;
   log_debug("input:\n%s\n",str);

   //ret=scanner_plugin.p_Scanner(str,str+strlen(str),&path,&output,0);
   ret=scanner_plugin.p_scan(scanner,service_name,str,strlen(str),&path,&output,0);

   log_debug("scanner (%s)[%d:%s]",path,ret,scanner_plugin.p_str_err(ret));
   if(ret==HPE_OK)
   {
      log_debug("parsing success");
   }else {
      log_debug("parsing failure");
   }
   if(path)free(path);
   if(output)free(output);
   return 0;
}

static int SetJavaClassPath(lua_State *L)
{
   const char *str = luaL_checkstring(L, 1);
   log_debug("%s",str);
   if(java_class_path) free(java_class_path);
   java_class_path=strdup(str);
   return 0;
}
int RunConfiguration(const char *filename)
{
   int ret;

   lua_State *L = luaL_newstate();
   if (!L)
   {
      log_error("unable to create new lua state");
      return 1;
   }

   luaL_openlibs(L); // Open standard libraries

   lua_register(L, "CreateTcp4Server", CreateTcp4Server);
   lua_register(L, "CreateTcp4AdminServer", CreateTcp4AdminServer);
   lua_register(L, "EnableSigInt", EnableSigInt);
   lua_register(L, "EnableSigUSR1", EnableSigUSR1);
   lua_register(L, "EnableSigUSR2", EnableSigUSR2);

   lua_register(L, "LogWithColor", LogWithColor);
   lua_register(L, "AbortOnError", AbortOnError);

   lua_register(L, "PrintPID", PrintPID);

   lua_register(L, "LogDebug", LuaLogDebug);
   lua_register(L, "LogError", LuaLogError);
   lua_register(L, "LogWarning", LuaLogWarning);
   lua_register(L, "LogInfo", LuaLogInfo);

   lua_register(L, "SetRoot", SetRoot);

   lua_register(L, "ProcessEvents", ProcessEvents);

   lua_register(L, "AddBusinessObject", AddBusinessObject);
   lua_register(L, "RemoveBusinessObject", RemoveBusinessObject);
   lua_register(L, "ListBusinessObjects", ListBusinessObjects);

   lua_register(L, "EnableLog", EnableLog);
   lua_register(L, "DisableLog", DisableLog);

   lua_register(L, "LoadPluginLibrary", LoadPluginLibrary);

   lua_register(L, "RestScanner", RestScanner);

   lua_register(L, "SetJavaClassPath", SetJavaClassPath);

   if(!java_class_path)java_class_path=strdup("-Djava.class.path=.");

   log_debug("loading configuration...");
   luaL_loadfile(L, filename);
   ret = lua_pcall(L, 0, 0, 0);
   if (ret != 0)
   {
      log_debug("%s",lua_tostring(L, -1));
      return 1;
   }
   close_state(&L);
   return 0;
}

void test_configuration()
{
   uv_signal_t *sig1a;
   const char *CLASSPATH=
   "-Djava.class.path=/home/gabrielfarinas/M4U_Interfaces/server"
   ":/home/gabrielfarinas/M4U_Interfaces/server/BO/JAVA/USSD"
   ":/home/gabrielfarinas/M4U_Interfaces/server/BO/JAVA/USSD/com"
   ":/home/gabrielfarinas/M4U_Interfaces/server/BO/JAVA/USSD/com/m4u_interfaces"
   ":/home/gabrielfarinas/M4U_Interfaces/server/BO/JAVA/USSD/com/m4u_interfaces/request"
   ":/home/gabrielfarinas/M4U_Interfaces/server/libraries/lib/protobuf-java-3.0.0-beta-1.jar";

   tcp4_start(&tcpServer,8085,on_connection);
   tcp4_start(&tcpServerAdmin,8086,on_connection_admin);

   if(java_class_path) free(java_class_path);
   java_class_path=strdup(CLASSPATH);

   add_bo("SMS",BO_CPP,"/tefmock/sms",0,"/BO/CPP/SMS/worker.exe");
   add_bo("USSD",BO_JAVA,"/tefmock/ussd",0,"/BO/JAVA/USSD/worker.exe");

   sig1a=(uv_signal_t *)malloc(sizeof(uv_signal_t));
   log_debug("EnableSigInt");
   uv_signal_init(loop, sig1a);
   uv_signal_start(sig1a, int_handler, SIGINT);

   load_plugin_library("./libraries/lib/libM4UI-Rest-2.0.so","LoadScannerPlugin","LoadScannerPluginDummy");

   uv_run(loop, UV_RUN_DEFAULT);

   uv_loop_close(loop);
   free(loop);
}

int main(int argc, const char **argv)
{
   const char *version="Version 1.0.0";
   GOOGLE_PROTOBUF_VERIFY_VERSION;
   loop = create_loop();
   InitializeLog(stdout);
   log_info("%s",version);

   RunConfiguration("scripts/init.lua");
   cleanup_bo();
   //test_configuration();
   google::protobuf::ShutdownProtobufLibrary();
   return 0;
}
