#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <arpa/inet.h>  
#include <fcntl.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <dirent.h>
#define BUFSIZE 50  
    
void error_handling(char *message);  
void upload();
void download();
void delete();
void serverlist();
void clientlist();

int serv_sd, clnt_sd;  
int fd;
char buf[BUFSIZE];
char cbuf[BUFSIZE];
char lsbuff[BUFSIZE];
char nullbuf[BUFSIZE];
struct sockaddr_in serv_addr;  
struct sockaddr_in clnt_addr;  
int clnt_addr_size;  
int len, lslen; 
int ch, i, j;
DIR *pdir;
struct dirent *pde; 	
	
int main(int argc, char *argv[]){    
	
				      
	if(argc!=2){  
		printf("Usage : %s <port>\n", argv[0]);  
		exit(1);  
	}
	serv_sd=socket(PF_INET, SOCK_STREAM, 0);     
	if(serv_sd == -1)
		error_handling("socket() error");  
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;  
    	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);  
	serv_addr.sin_port=htons(atoi(argv[1]));

	if( bind(serv_sd, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1 )  
		error_handling("bind() error");  

	if( listen(serv_sd, 5)==-1 )  
		error_handling("listen() error");  

	clnt_addr_size=sizeof(clnt_addr);      
	clnt_sd=accept(serv_sd, (struct sockaddr*)&clnt_addr,&clnt_addr_size);  
	if(clnt_sd==-1)  
		error_handling("accept() error");
	
	while(1){
		len = read(clnt_sd, buf, 3);
		printf("%s\n", buf);				
		ch = atoi(buf);
		fflush(stdin);
		if(ch == 1){
			upload();
		}
		else if(ch==2){
			download();
		}
		else if(ch==3){
			delete();
		}
		else if(ch==4){
			serverlist();
		}
		else if(ch==5){
		}
		else if(ch==6){
			break;
		}
		else{
			printf("no select\n");
		}
		ch = 0;
	
	}  
	close(clnt_sd);  
	return 0;  
}  

void download(){
	len = read(clnt_sd, buf, BUFSIZE);  
	printf("1read : %s\n", buf);
		
	fd = open(buf, O_RDONLY );   
	if(fd == -1){  
		error_handling("File open error");
	}  
	while(1){
		len = read(fd,buf,BUFSIZE);
		printf("\n-----------len:%d---------\n",len);
		if(len == BUFSIZE){
			write(clnt_sd, buf, len);
			printf("4write : %s\n", buf);
		}
		else if(len < BUFSIZE && len !=0){
			write(clnt_sd, buf, len);
			printf("buf : %s\n", buf);
			break;
		}
	}
	printf("\n");
	for(i=0;i<BUFSIZE;i++){
		buf[i] = '\0';
	}  
	close(fd);  
}
void delete(){
	len = read(clnt_sd, cbuf, BUFSIZE);
	printf("%s : delete\n", cbuf);
	if(-1 == remove(cbuf)){
		printf("file remove fail");
	}
	printf("\n");
}

void clientlist(){
	for(i=0;i<BUFSIZE;i++){
		buf[i]='\0';
		cbuf[i]='\0';
		lsbuff[i]='\0';
	}
	len = read(clnt_sd,buf,BUFSIZE);
}
void error_handling(char *message){  
	fputs(message, stderr);  
	fputc('\n', stderr);  
	exit(1);
}
void serverlist(){
	lslen = read(clnt_sd,buf,BUFSIZE);
	if(strcmp(buf,"ls")==0){
		getcwd(lsbuff, BUFSIZE); //working directory
		if((pdir= opendir(lsbuff))<0){ //open directory
			perror("opendir");
			exit(1);
		}
		while(1){
			if((pde = readdir(pdir))!=NULL){ //read directory
           			sprintf(buf,"%d%c",strlen(pde->d_name),'\0'); //pde->d_name  -> file name
		        	len = write(clnt_sd,buf,strlen(buf));
				if(len == -1){
					exit(1);
				}
        	   		len = read(clnt_sd,buf,2);       
	           		buf[2]='\0';
	           		if(strcmp(buf,"ok")==0){
        	   			write(clnt_sd,pde->d_name,strlen(pde->d_name));	
				}
				len = read(clnt_sd,buf,BUFSIZE);
			}
			else if((pde = readdir(pdir))==NULL){
				len = write(clnt_sd,"no",2); 
				break;
			}	
		}
 	}
	
	printf("\n");
	closedir(pdir);
}
void upload(){
	len = read(clnt_sd, buf, BUFSIZE); //file name
	fd = open(buf, O_WRONLY | O_TRUNC | O_CREAT, 0777);  //file create
	if(fd == -1){  
		error_handling("File create error");
	}
	write(clnt_sd, buf, strlen(buf));
	while(1){ 
		len=read(clnt_sd, buf, BUFSIZE);
		if(len !=0){
			write(fd, buf, len);
			printf("len = %d\n", len);

			if(len < BUFSIZE){
				break;
			}
		}  
	}
	printf("upload end\n");
	for(i=0;i<BUFSIZE;i++){
		buf[i]='\0';
	} 
	close(fd);
}
