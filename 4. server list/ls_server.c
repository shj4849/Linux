#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <arpa/inet.h>  
#include <fcntl.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <dirent.h>  
#define BUFSIZE 30  
void error_handling(char *message);  
int main(int argc, char *argv[]){    
	int serv_sd;  
	int clnt_sd;  
	int fd;
	DIR *pdir;
	struct dirent *pde;  
	char buf[BUFSIZE];
	char lsbuff[BUFSIZE];
	char cbuf[BUFSIZE]; 
	struct sockaddr_in serv_addr;  
	struct sockaddr_in clnt_addr;  
	int clnt_addr_size;  
	int len;
	int i;
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
	
        int lslen = read(clnt_sd,buf,BUFSIZE);//      read1
	
        if(strcmp(buf,"ls")==0){
		getcwd(lsbuff, BUFSIZE);
		if((pdir= opendir(lsbuff))<0){
			perror("opendir");
			exit(1);
		}
		while((pde = readdir(pdir))!=NULL){
                	sprintf(buf,"%d\0",strlen(pde->d_name));
		
	                len = write(clnt_sd,buf,strlen(buf));	// write 1
			if(len == -1){
				exit(1);
			}
                	len = read(clnt_sd,buf,2);        // read 2
                	buf[2]='\0';
                	if(strcmp(buf,"ok")==0){
                      		write(clnt_sd,pde->d_name,strlen(pde->d_name));	//write 2
			}
		}
 	}
	printf("\n");
	close(clnt_sd);  
	closedir(pdir);
	return 0;  
}  
void error_handling(char *message){  
	fputs(message, stderr);  
	fputc('\n', stderr);  
	exit(1);
}
