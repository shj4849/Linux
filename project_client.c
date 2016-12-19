#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <arpa/inet.h>  
#include <fcntl.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <sys/stat.h>
  
#define BUFSIZE 50
    
void error_handling(char *message);  
void upload();
void download();
void delete();
void serverlist();
void clientlist();
void print_start();

int len, lslen; 
int sd, fd;  
char name[BUFSIZE];
char buf[BUFSIZE];
char cbuf[BUFSIZE];
char number[BUFSIZE]; 
int ch;
struct sockaddr_in serv_addr;  
int clnt_addr_size;  
int i,j;

int main(int argc, char *argv[]){    					      
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
	while(1){
		print_start();
		scanf("%s",number);
		
		ch = atoi(number);		
		write(sd, number, 3); // write
		fflush(stdin);
		if(ch == 1){	
			upload();	
		}
		else if(ch == 2){
			download();
		}
		else if(ch == 3){
			delete();
		}
		else if(ch == 4){
			serverlist();
		}
		else if(ch == 5){
			clientlist();
		}
		else if(ch == 6){
			printf("Bye\n");
			break;
		}
		else{ 
			printf("sorry retry please \n");
		}
		ch = 0;
	}
	
	close(sd);
	return 0;
}

void download(){
	
	scanf("%s",name);
	write(sd, name, strlen(name));    
	printf("1write : %s\n",name); 	
	fd = open(name, O_WRONLY | O_TRUNC | O_CREAT, 0775);  
	if(fd == -1){  
		error_handling("File open error");
	}
	while(1){
		if((len = read(sd, buf, BUFSIZE)) != 0 ){
			write(fd,buf, len);
			if(len <BUFSIZE){
				break;
			}	
		}
		for(i=0;i<BUFSIZE;i++){
		buf[i]='\0';
		}   
	}  
	for(i=0;i<BUFSIZE;i++){
		name[i]='\0';
		buf[i]='\0';
	}    
	close(fd);  
}
void delete(){
	for(i=0;i<BUFSIZE;i++){
		name[i]='\0';
		buf[i]='\0';
		cbuf[i]='\0';
	} 
	scanf("%s",cbuf);
	write(sd, cbuf, strlen(cbuf));  	
}
void serverlist(){
	for(i=0;i<BUFSIZE;i++){
		name[i]='\0';
		buf[i]='\0';
		cbuf[i]='\0';
	} 
	lslen = write(sd,"ls",sizeof("ls"));
	while(1){
		if(len =read(sd,buf,BUFSIZE)>0){
       			lslen=atoi(buf);
			buf[2]='\0';			
			if(strcmp(buf,"no")==0){
				break;
			}
			for(i=0;i<BUFSIZE;i++){			
				buf[i]='\0';
			}
       			len=write(sd,"ok",2);
			
			len=read(sd,buf,lslen);
			printf("%-20s",buf);
			buf[0]='\0';
			len = write(sd,"ok",2);
			if(++j % 4 == 0){
				printf("\n");
			} 
		}		
	}
	printf("\n");   
}
void clientlist(){
	for(i=0;i<BUFSIZE;i++){
		name[i]='\0';
		buf[i]='\0';
		cbuf[i]='\0';
	} 
	system("ls");
	len = write(sd,"ok",sizeof("ok"));
}
void error_handling(char *message){  
	fputs(message, stderr);  
	fputc('\n', stderr);  
	exit(1);
}
void upload(){
	scanf("%s",name);
	write(sd, name, strlen(name)); //file name

	fd = open(name, O_RDONLY);
	if(fd == -1){
		error_handling("File open error");
	}
	lslen = read(sd,name,BUFSIZE);
	while(1){
		if((len=read(fd, buf, BUFSIZE )) != 0 ){   //file send
			write(sd, buf, len);
		} 
		else{
			break;
		}  
	} 
	for(i=0;i<BUFSIZE;i++){
		buf[i] = '\0';
		name[i]='\0';
	}
	close(fd);
}
void print_start(){
	printf("\n");
	printf("-----------------\n");
	printf("1. file upload\n");
	printf("2. file download\n");
	printf("3. file delete(server)\n");
	printf("4. server list\n");
	printf("5. user list\n");
	printf("6. exit\n");
	printf("-----------------\n");
}
