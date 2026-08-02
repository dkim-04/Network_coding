/*****************************/
/* setup_m_srv.c */
/*****************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h> 

int setup_m_srv(char* hostname, u_short port, int backlog){
    struct hostent *myhost; 
    struct sockaddr_in me; 
    int s; 

    if ((myhost = gethostbyname(hostname)) == NULL){
        herror("gethostbyname");
        return -1;
    }

    memset(&me, 0, sizeof(me));
    me.sin_family = AF_INET;
    me.sin_port = htons(port);
    memcpy(&me.sin_addr, myhost->h_addr, myhost->h_length); 

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return -1;
    }

    if (bind(s, (struct sockaddr *)&me, sizeof(me)) == -1) {
        perror("bind");
        return -1;
    }
    
    if (listen(s, backlog) == -1) {
        perror("listen");
        return -1;
    }
    fprintf(stderr, "Server is Running...\n");
    
    return s;
}

int m_accept(int soc, int limit, void (*func)(int, int)){
    int fd;
    int maxfd = soc;

    for (int i = 0; i < limit; i++) {
        if ((fd = accept(soc, NULL, NULL)) == -1) {
            perror("accept");
            return -1;
        }

        if (fd > maxfd)
            maxfd = fd;

        func(i, fd);
        fprintf(stderr, "Client enters room[%d/%d]\n", i+1, limit);
    }

    close(soc);
    return maxfd;
}

int accept_client(int soc){
    int fd;
    if ((fd = accept(soc, NULL, NULL)) == -1) {
        perror("accept");
        return -1;
    }
    return fd;
}
