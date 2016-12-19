#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <arpa/inet.h>  
#include <fcntl.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
			       
#define BUFSIZE 30  
 
void error_handling(char *message);  
	       
int main(int argc, char *argv[]){  
	int sd;  		           
	char buf[BUFSIZE];
	int len, ls_len;  
	int i,j;
	struct sockaddr_in serv_addr;  
	if(argc!=3){  
		printf("Usage : %s <IP> <port>\n", argv[0]);  
		exit(1);  
	} 
	sd=socket(PF_INET, SOCK_STREAM, 0);     
	if(sd == -1){  
		error_handling("socket() error");
	} 
	memset(&serv_addr, 0, sizeof(serv_addr));  
	serv_addr.sin_family=AF_INET;  
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2])); 
	
	if( connect(sd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1 ){  
		error_handling("connect() error!");  
	}
	int lslen = write(sd,"ls",sizeof("ls"));	//write 1
        while(1){
		if(read(sd,buf,BUFSIZE)>0){	//read 1
        		ls_len=atoi(buf); 
			for(i=0;i<BUFSIZE;i++){			
				buf[i]='\0';
			}
        		len=write(sd,"ok",2); //write 2
			
			len=read(sd,buf,ls_len); //read 2
			if(len>0){
				printf("%-20s",buf);
				buf[0]='\0';
			}
			else if(len<0){
				printf("exit");
				exit(0);
			}
			if(++j % 4 == 0){
				printf("\n");
			}  
        	}else{
			len=write(sd,"no",2); // write2-1
			break;
		}	
	}
	printf("\n");   
	close(sd);  
	return 0;  
}  
void error_handling(char *message){
	fputs(message, stderr);  
	fputc('\n', stderr);  
	exit(1);  
}


