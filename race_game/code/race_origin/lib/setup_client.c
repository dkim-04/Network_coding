/*****************************/
/* lib/setup_client.c */
/*****************************/

#include <stdio.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h> 
#include <string.h> 

#include "snet.h"
// hostname 입력하고 client용 통신용 소켓 반환
int setup_client(char* hostname, u_short port){
    struct hostent *myhost; 
    struct sockaddr_in server; 
    int s; 

    if ((myhost = gethostbyname(hostname)) == NULL){
        herror("gethostbyname");
        return -1;
    }
 
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    memcpy(&server.sin_addr, myhost->h_addr, myhost->h_length); 

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return -1;
    }

    if (connect(s, (struct sockaddr*)&server, sizeof(server)) == -1) {
        perror("connect");
        close(s);
        return -1;
    }
    
    return s;
}
