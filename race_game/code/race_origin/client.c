// client.c

#include <stdio.h>
#include <stdlib.h>

#include "snet.h"
#include "race.h"
#include "session.h"


int main(void){
    int soc;
    char hostname[256];
    // 호스트 name 입력
    if (input_hostname(hostname, sizeof(hostname)) == -1){
        fprintf(stderr, "invalid hostname\n");
        exit(1);
    }

    if ((soc = setup_client(hostname, PORT)) == -1){
        fprintf(stderr, "connection fail\n");
        exit(1);
    }
    
    init_session(soc);
    session();

    return 0;
}