/*Required Headers*/
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/un.h>

char *SOCKET_PATH = "\0hidden";

int connect_server() {
   struct sockaddr_un addr;
   int fd;

   if ((fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
   printf("Failed to create client socket");
   return fd;
   }

   memset(&addr, 0, sizeof(addr));

   addr.sun_family = AF_LOCAL;
   strcpy(addr.sun_path, SOCKET_PATH);

   if (connect(fd,(struct sockaddr *) &(addr), sizeof(addr)) < 0) {
   printf("Failed to connect to server");
   return -1;
   }

   //setnonblocking(fd);

   /* Add handler to handle events */

   return fd;
}
static int
send_file_descriptor(
  int socket, /* Socket through which the file descriptor is passed */
  int fd_to_send) /* File descriptor to be passed, could be another socket */
{
    struct msghdr message;
    struct iovec iov[1];
    struct cmsghdr *control_message = NULL;
    char ctrl_buf[CMSG_SPACE(sizeof(int))];
    char data[1];

    memset(&message, 0, sizeof(struct msghdr));
    memset(ctrl_buf, 0, CMSG_SPACE(sizeof(int)));

    /* We are passing at least one byte of data so that recvmsg() will not return 0 */
    data[0] = ' ';
    iov[0].iov_base = data;
    iov[0].iov_len = sizeof(data);

    message.msg_name = NULL;
    message.msg_namelen = 0;
    message.msg_iov = iov;
    message.msg_iovlen = 1;
    message.msg_controllen =  CMSG_SPACE(sizeof(int));
    message.msg_control = ctrl_buf;

    control_message = CMSG_FIRSTHDR(&message);
    control_message->cmsg_level = SOL_SOCKET;
    control_message->cmsg_type = SCM_RIGHTS;
    control_message->cmsg_len = CMSG_LEN(sizeof(int));

    *((int *) CMSG_DATA(control_message)) = fd_to_send;
    printf("sendmsg\n");
    return sendmsg(socket, &message, 0);
}
int main()
{
    int udserver;
    char str[100];
    int listen_fd, comm_fd;
 
    struct sockaddr_in servaddr;
 
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
 
    bzero( &servaddr, sizeof(servaddr));
 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(22000);
 
    bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
 
    listen(listen_fd, 10);
 
    udserver= connect_server();

    while(1){
       comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
       printf("accepted[%d]\n",comm_fd);
       send_file_descriptor(udserver,comm_fd);
    }


}
