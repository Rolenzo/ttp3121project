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
#include <netdb.h>
#include <fcntl.h>

int main(int argc, char* argv[]){
int sockfd, numbytes,fd,b;
char buffer[2048];
char choice[20];

struct hostent *he;
struct sockaddr_in their_addr;

if(argc!=2){
   fprintf(stderr,"\nPlease Enter : ./client RemoteIPaddress! (ex: ./client 192.168.1.1)\n");
   exit(1);
}
//get host info
if((he = gethostbyname(argv[1])) == (void *)0){ 
   printf("\n gethostbyname() error!!!\n");
   exit(1);
}

if((sockfd = socket(AF_INET, SOCK_STREAM,0))==-1){
   printf("\nsocket() error!!!");
   exit(1);
}

bzero(&(their_addr),sizeof(their_addr));/* zero the rest of the struct */
their_addr.sin_family = AF_INET; /* host byte order */ 
their_addr.sin_port = htons(3490);
their_addr.sin_addr = *((struct in_addr *)he->h_addr);
bzero(&(their_addr.sin_zero),8);// zero the rest of the struct

if(connect(sockfd,(struct sockaddr *)&their_addr,sizeof(struct sockaddr))==-1){
    printf("\nconnect() error!!!\n");
    exit(1);
}

else{

  do{
    printf("\n\t\t############################################\n");
    printf("\t\t\tFile Repository System");
    printf("\n\t\t############################################\n");
    printf("\n\t\t\t1-> Create New Directory");
    printf("\n\t\t\t2-> Delete Directory");
    printf("\n\t\t\t3-> Add and Send File");
    printf("\n\t\t\t4-> Download and Display File");
    printf("\n\t\t\t5-> Delete Data");
    printf("\n\t\t\t6-> Exit");
    printf("\nPlease enter your choice :");
    scanf("%s",choice);
    send(sockfd,choice,sizeof(choice),0);
    b=atoi(choice);
  
    if(b==1){
      printf("Create New Directory\n");
      send(sockfd,buffer,sizeof(buffer),0);
      bzero(buffer,sizeof(buffer));
    }

    else if(b==2){
      printf("Delete Directory");
    }

    else if(b==3){
    printf("Add and Send File\n");
    printf("Enter ID/NAME/CONTACT NUMBER/IC NUMBER/CAR MODEL/DAYS/LICENSE NUMBER\n");
    scanf("%s",buffer);
    send(sockfd,buffer,sizeof(buffer),0);
    bzero(buffer,sizeof(buffer));
    }

    else if(b==4){
    recv(sockfd,buffer,sizeof(buffer),0);
    write(1, buffer, strlen(buffer));
    printf("\n");
    bzero(buffer,sizeof(buffer));
    }

    else if(b==5){
    printf("Data Deleted");
    }

    else if(b==6){
    exit(EXIT_SUCCESS);
    }
  }
  while (1);

  close(sockfd);
  }
  return 0;
}
