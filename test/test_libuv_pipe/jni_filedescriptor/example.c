
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int GetDescriptor()
{

   int fd;
   const char *fname="/home/gwerners/workspace/Orthrus/test/test_libuv_pipe/jni_filedescriptor/readme.txt";


   fd = open(fname, O_RDWR | O_NONBLOCK);

   return fd;

}
void CloseDescriptor(int fd)
{
   close(fd);
}
