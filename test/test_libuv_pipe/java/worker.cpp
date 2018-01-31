#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <uv.h>
#include <jni.h>


uv_loop_t *loop;
uv_pipe_t queue;

///java method
jmethodID mid=0;

//java environment
JNIEnv *env;

//java class
jclass cls;

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
  buf->base = malloc(suggested_size);
  buf->len = suggested_size;
}

void on_new_connection(uv_stream_t *q, ssize_t nread, const uv_buf_t *buf)
{

   jint square;
   int fd;
   if (nread < 0) {
      if (nread != UV_EOF)
         fprintf(stderr, "Read error %s\n", uv_err_name(nread));
      uv_close((uv_handle_t*) q, NULL);
      return;
   }

   uv_pipe_t *pipe = (uv_pipe_t*) q;
   if (!uv_pipe_pending_count(pipe)) {
      fprintf(stderr, "No pending count\n");
      return;
   }

   uv_handle_type pending = uv_pipe_pending_type(pipe);
   assert(pending == UV_TCP);


   fd=q->accepted_fd;
   fprintf(stderr, "Worker %d: Accepted fd %d\n", getpid(), fd);

   fprintf(stderr,"java called to answer client\n");
   square = (*env)->CallStaticIntMethod(env, cls, mid, fd,getpid());
   fprintf(stderr,"Result of HandleClient: %d\n", square);

}

int CreateJVM()
{

   JavaVMOption options[1];
   JavaVM *jvm;
   JavaVMInitArgs vm_args;
   long status;

   options[0].optionString = "-Djava.class.path=.";
   memset(&vm_args, 0, sizeof(vm_args));
   vm_args.version = JNI_VERSION_1_2;
   vm_args.nOptions = 1;
   vm_args.options = options;
   status = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
   if (status != JNI_ERR)
   {
      cls = (*env)->FindClass(env, "Worker");
      if(cls !=0)
      {
         mid = (*env)->GetStaticMethodID(env, cls, "HandleClient", "(II)I");

         ////////////////////////////////////////////////////////////////////
      }
      //precisa ficar ativa para nao queimar chamadas jni!
      //(*jvm)->DestroyJavaVM(jvm);
      return 0;
   }
   else
      return -1;
}
int main() {
    CreateJVM();
    loop = uv_default_loop();
    uv_pipe_init(loop, &queue, 1 /* ipc */);
    uv_pipe_open(&queue, 0);
    uv_read_start((uv_stream_t*)&queue, alloc_buffer, on_new_connection);
    //create jvm server
    return uv_run(loop, UV_RUN_DEFAULT);
}
