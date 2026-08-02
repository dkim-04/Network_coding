/*****************************/
/* lib/setup_server.c */
/*****************************/

#include <stdio.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h> 
#include <string.h> 

#include "snet.h"
// client 접속 기다리는 서버소켓 이용해 client와 통신하는 소켓 생성
int setup_server(char* hostname, u_short port){
    struct hostent *myhost; 
    struct sockaddr_in me; 
    //s_waiting 접속 기다리는 서버소켓, s= client와 통신하는 소켓
    int s_waiting, s; 

    if ((myhost = gethostbyname(hostname)) == NULL){
        herror("gethostbyname");
        return -1;
    }

    memset(&me, 0, sizeof(me));
    me.sin_family = AF_INET;
    me.sin_port = htons(port);
    memcpy(&me.sin_addr, myhost->h_addr, myhost->h_length); 

    if ((s_waiting = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return -1;
    }

    if (bind(s_waiting, (struct sockaddr *)&me, sizeof(me)) == -1) {
        perror("bind");
        close(s_waiting);
        return -1;
    }
    
    if (listen(s_waiting, 1) == -1) {
        perror("listen");
        close(s_waiting);
        return -1;
    }
    // client 접속 기다리는 서버 소켓을 accept하여 새로운 통신용 소켓 생성
    if ((s = accept(s_waiting, NULL, NULL)) == -1) {
        perror("accept");
        close(s_waiting);
        return -1;
    }
    
    close(s_waiting);
    // 통신용 소켓 반환
    return s;

