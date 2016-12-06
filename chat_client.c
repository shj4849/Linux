#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUFF_SIZE 100
#define NAME_SIZE 20

void* send_msg(void* arg);
void* recv_msg(void* arg);
void error(char *msg);


char name[NAME_SIZE] = "[Default]";
char msg[BUFF_SIZE];

int main(int argc, char *argv[]){
	int sock;
	struct sockaddr_in serv_addr;
	pthread_t snd_thread, rcv_thread;
	void* thread_result;

	if(argc != 4){
		printf("Usage : %s <ip> <port> \n", argv[0]);
		exit(1);
	}
	sprintf(name, "[%s]", argv[3]);
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error("socket() error\n");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if(connect(sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error("connect() error\n");

	pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
	pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
	pthread_join(snd_thread, &thread_result);
	pthread_join(rcv_thread, &thread_result);
	
	close(sock);
	return 0;
}
void* send_msg(void* arg){
	int sock =*((int*) arg);
	char name_msg[NAME_SIZE+BUFF_SIZE];
	while(1){
		fgets(msg, BUFF_SIZE, stdin);
		if(!strcmp(msg, "q\n") || !strcmp(msg,"Q\n")){
			close(sock);
			exit(0);
		}
		sprintf(name_msg, "%s %s", name, msg);
		write(sock, name_msg, strlen(name_msg));
	}
	return NULL;
}
void* recv_msg(void* arg){
	int sock =*((int*) arg);
	char name_msg[NAME_SIZE+BUFF_SIZE];
	int str_len;
	while(1){
		str_len = read(sock, name_msg, NAME_SIZE+BUFF_SIZE-1);
		if(str_len == -1)
			return (void*)-1;
		name_msg[str_len]=0;
		fputs(name_msg,stdout);
	}
	return NULL;
}
void error(char *msg){
	fputs(msg,stderr);
	fputc('\n', stderr);
	exit(1);
}
