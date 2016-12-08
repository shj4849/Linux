#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <arpa/inet.h>  
#include <fcntl.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <sys/stat.h>
  
#define BUFSIZE 30  
    
void error_handling(char *message);  

int main(int argc, char *argv[]){    
	int sd;  
	int fd;  
	char name[BUFSIZE];
	char buf[BUFSIZE];
	char cbuf[BUFSIZE]; 

	struct sockaddr_in serv_addr;  

	int clnt_addr_size;  
	int len;  
						      
	if(argc!=3){  
		printf("Usage : %s <IP> <port>\n", argv[0]);  
		exit(1);  
	} 
	sd=socket(PF_INET, SOCK_STREAM, 0);  
	if(sd == -1)
		error_handling("socket() error");
  
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;  
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);  
	serv_addr.sin_port=htons(atoi(argv[2]));

	if( connect(sd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1 ){  
		error_handling("connect() error!");  
	}
	scanf("%s",name);
	len = strlen(name);
	write(sd, name, len); //file name

	len =read(sd,cbuf,BUFSIZE);
	write(1,cbuf,len);
	fd = open(cbuf, O_RDONLY);
	if(fd == -1){
		error_handling("File open error");
	}
	while( (len=read(fd, buf, BUFSIZE )) != 0 ){   //file send
		write(sd, buf, len);   
	} 
	if( shutdown(sd, SHUT_WR) == -1 ){  
		error_handling("shutdown error");  
	}
	len = read(sd, cbuf, BUFSIZE);  
	write(1, cbuf, len);
	printf("\n");
	close(fd);
	close(sd);
	return 0;
/*
	scanf("%s", cbuf);	
	write(sd,cbuf,5);
	file open
	fd = open( cbuf, O_RDONLY );
	if(fd == -1){
		error_handling("File open error");
	}

	//file send
	while( (len=read(fd, buf, BUFSIZE )) != 0 ){  
		write(sd, buf, len);   
	}
  	if( shutdown(sd, SHUT_WR) == -1 ){
        	error_handling("shutdown error");
	}  
	//len = read(sd, cbuf, BUFSIZE);  
	//write(1, cbuf, len);  	     
	close(fd);  
	close(sd);  
	return 0; */ 
}  
  
void error_handling(char *message){  
	fputs(message, stderr);  
	fputc('\n', stderr);  
	exit(1);
}
