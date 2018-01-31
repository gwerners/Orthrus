#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/un.h>

#define MAX_PENDING 10
char *SOCKET_PATH = "\0hidden";

int create_server() {
   struct sockaddr_un addr;
   int fd;

   if ((fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
      printf("Failed to create server socket");
      return fd;
   }

   memset(&addr, 0, sizeof(addr));

   addr.sun_family = AF_LOCAL;
   unlink(SOCKET_PATH);
   strcpy(addr.sun_path, SOCKET_PATH);

   if (bind(fd, (struct sockaddr *) &(addr), sizeof(addr)) < 0) {
      printf("Failed to bind server socket");
      return -1;
   }

   if (listen(fd, MAX_PENDING) < 0) {
      printf("Failed to listen on server socket");
      return -1;
   }

   //setnonblocking(fd);

   /* Add handler to handle events on fd */

   return fd;
}
static int
recv_file_descriptor(int socket)
/* Socket from which the file descriptor is read */
{
   int sent_fd;
   struct msghdr message;
   struct iovec iov[1];
   struct cmsghdr *control_message = NULL;
   char ctrl_buf[CMSG_SPACE(sizeof(int))];
   char data[1];
   int res;

   memset(&message, 0, sizeof(struct msghdr));
   memset(ctrl_buf, 0, CMSG_SPACE(sizeof(int)));

   /* For the dummy data */
   iov[0].iov_base = data;
   iov[0].iov_len = sizeof(data);

   message.msg_name = NULL;
   message.msg_namelen = 0;
   message.msg_control = ctrl_buf;
   message.msg_controllen = CMSG_SPACE(sizeof(int));
   message.msg_iov = iov;
   message.msg_iovlen = 1;
   printf("before recvmsg\n");

   if((res = recvmsg(socket, &message, 0)) <= 0)
      return res;
   printf("message received!\n");
   /* Iterate through header to find if there is a file descriptor */
   for(control_message = CMSG_FIRSTHDR(&message);
      control_message != NULL;
      control_message = CMSG_NXTHDR(&message,control_message))
   {
      if( (control_message->cmsg_level == SOL_SOCKET) &&
      (control_message->cmsg_type == SCM_RIGHTS) )
      {
         printf("socket fd received\n");
         return *((int *) CMSG_DATA(control_message));
      }
   }

   return -1;
}

int main(int argc, char *argv[])
{
   char answer[200];
   char buf[100];
   int socket,rc,comm_fd;
   int server=create_server();
   printf("server created[%d]\n",server);

   while(1){
      comm_fd = accept(server, (struct sockaddr*) NULL, NULL);
      printf("accepted[%d]\n",comm_fd);
      socket=recv_file_descriptor(comm_fd);
      while( (rc=read(socket, buf, sizeof(buf))) > 0) {
         sprintf(answer,"from udserver[%s]",buf);
         rc=strlen(answer);
         if (write(socket, answer, rc) != rc) {
            if (rc > 0)
               fprintf(stderr,"partial write\n");
            else {
               perror("write error");
               exit(-1);
            }
         }
      }
   }




   return 0;
}

