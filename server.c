#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fcntl.h>

void sigchld_handler (int sig);

int main(int argc, char** argv){

int sockfd, new_sockfd, len_size,childpid,numbytes,pipefd[2],nread,b;
char buffer[2048],buffer1[100];
struct sockaddr_in my_addr;
struct sockaddr_in their_addr;

int sin_size;
struct sigaction sa;
int yes=1;
/*creat socket*/
if((sockfd = socket(AF_INET, SOCK_STREAM,0)) == -1){
  perror("\nsocket() error!!!\n");
  exit(1);
}

/*Avoid address in use*/
if(setsockopt(sockfd,SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
  perror("\nsetsockopt() error!!!\n");
  exit(1);
}

bzero((char *)&my_addr, sizeof(my_addr));/* zero the rest of the struct */
my_addr.sin_family = AF_INET;//assign serv_addr as internet protocol
my_addr.sin_port = htons(3490);// server's tcp port number
my_addr.sin_addr.s_addr = INADDR_ANY;//any ip address
bzero(&(my_addr.sin_zero),8);// zero the rest of the struct

/* assign the socket to ther address */
if(bind(sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr)) == -1){
  perror("\nbind() error!!!\n");
  exit(1);
}

/*A SERVER process indicate that it is willing to receive 5 connections request that can be queue by the system in sockfd*/
if(listen(sockfd,5) == -1){
  perror("\nlisten() error\n");
  exit(1);
}

printf("\nWaiting for connection!!!\n");

sa.sa_handler = sigchld_handler;// reap all dead processes
sigemptyset(&sa.sa_mask);
sa.sa_flags = SA_RESTART;

if(sigaction(SIGCHLD,&sa,(void *)0) == -1){
  printf("\nsigaction() error!!!\n");
  exit(1);
}

for(;;){ /*main accept() loop*/
  sin_size = sizeof(struct sockaddr_in);
  if((new_sockfd = accept(sockfd,(struct sockaddr *)&their_addr,&sin_size)) == -1){
    printf("\naccept() error!!!\n");
    exit(1);
  }
  
  if(pipe(pipefd)<0 ) //create 2 pipes
    perror("pipe error");
 
  if(childpid=fork()==0){ /* this is the child process */
    close(sockfd);

    printf("Client : Connection from %s\n",inet_ntoa(their_addr.sin_addr));
    do{
      recv(new_sockfd,buffer1,sizeof(buffer1),0);
      b=atoi(buffer1);
      printf("Receive [%s] from %s the CHILD PID is %d\n" ,buffer1,inet_ntoa(their_addr.sin_addr),getpid());
      switch(b){
    
      case 1:
      pipefd[1] = mkdir("./test", 0777);
      close(pipefd[0]);
      recv(new_sockfd,buffer,sizeof(buffer),0);
      printf("Receive [Directory Created] message from %s the CHILD PID is %d\n" ,inet_ntoa(their_addr.sin_addr),getpid());
      write(pipefd[1],buffer,strlen(buffer));
      printf("\n");
      bzero(buffer,strlen(buffer));
      break;

      case 2:
      pipefd[1]= rmdir("./test");
      printf("Receive [Directory Deleted] message from %s the CHILD PID is %d\n" ,inet_ntoa(their_addr.sin_addr),getpid());
      printf("\n");
      break;

      case 3:
      pipefd[1] = open("testing.txt", O_WRONLY | O_CREAT , 0777);
      close(pipefd[0]);
      recv(new_sockfd,buffer,sizeof(buffer),0);
      printf("Receive [%s] from %s the CHILD PID is %d\n" ,buffer,inet_ntoa(their_addr.sin_addr),getpid());
      printf("Files was Added and Send to Server\n");
      write(pipefd[1],buffer,strlen(buffer));
      printf("\n");
      bzero(buffer,strlen(buffer));
      break;

      case 4:
      printf("File was Downloaded and Displayed\n");
      pipefd[0]= open("testing.txt", O_RDONLY,0777);
      close(pipefd[1]);
      while( (nread = read(pipefd[0],buffer,sizeof(buffer))) > 0) 
      send(new_sockfd,buffer,nread,0);
      printf("\n");
      break;

      case 5:
      pipefd[1]=open("testing.txt",O_TRUNC);
      printf("Receive [Data Deleted] message from %s the CHILD PID is %d\n" ,inet_ntoa(their_addr.sin_addr),getpid());
      printf("\n");
      break;

      case 6:
      printf("Client Exit");
      exit(EXIT_SUCCESS);
      printf("\n");
      break;
    }
    }while(1);
    exit(0);
  }

  else if (childpid <0){ /* failed to fork */
  printf("Failed to fork\n");
  }

  else if(childpid > 0){  /* parent process */
  close(new_sockfd);   /* parent closes connected socket */
  }
}
  return 0;
}

void sigchld_handler(int sig){
   wait((int *)0);
}
