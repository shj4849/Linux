#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUFF_SIZE 100
#define MAX_CLNT 10

void *clnt_connect(void *arg);
void send_msg(char *msg, int len);
void error(char *msg);

int clnt_number = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char *argv[]){
	int serv_sock;
	int clnt_sock;
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	int clnt_addr_size;
	pthread_t thread;

	if(argc != 2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	pthread_mutex_init(&mutx, NULL);
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error("socket() error\n");
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error("bind() error\n");
	if(listen(serv_sock, 5) == -1)
		error("listen() error\n");
	while(1){
		clnt_addr_size = sizeof(clnt_addr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_number++] = clnt_sock;
		pthread_mutex_unlock(&mutx);
		pthread_create(&thread, NULL, clnt_connect, (void*)&clnt_sock);
		pthread_detach(thread);
		printf("IP : %s \n",inet_ntoa(clnt_addr.sin_addr));
	}
	close(serv_sock);
	return 0;
}

void *clnt_connect(void *arg){
	int clnt_sock = *((int*) arg);
	int str_len = 0;
	char msg[BUFF_SIZE];
	int i;

	while((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
		send_msg(msg, str_len);

	pthread_mutex_lock(&mutx);
	for(i=0;i<clnt_number;i++){
		if(clnt_sock == clnt_socks[i]){
			while(i++<clnt_number-1)
				clnt_socks[i] = clnt_socks[i+1];
			break;
		}
	}
	clnt_number--;
	pthread_mutex_unlock(&mutx);
	close(clnt_sock);
	return NULL;
}
void send_msg(char *msg, int len){
	int i;
	pthread_mutex_lock(&mutx);

	for(i=0;i<clnt_number;i++)
		write(clnt_socks[i], msg, len);

	pthread_mutex_unlock(&mutx);
}
void error(char *msg){
	fputs(msg, stderr);
	fputc('\n',stderr);
	exit(1);
}
