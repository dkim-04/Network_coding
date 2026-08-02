// server.c

#include <stdio.h>
#include <stdlib.h>

#include "snet.h"
#include "race.h"
#include "session_man.h"

int main(int argc, char *argv[]){
    long num_player;
    long total_lap;
    int sock;
    int maxfd;
    char *end;
    char hostname[256];

    if(argc != 3){
        fprintf(stderr, "Usage: %s <players> <total laps>\n", argv[0]);
        exit(1);
    }
    
    num_player = strtol(argv[1], &end, 10);
    if (num_player <= 0 || num_player > MAX_PLAYERS) {
        fprintf(stderr, "invalid player number. please select 1 ~ %d\n", MAX_PLAYERS);
        exit(1);
    }
    
    total_lap = strtol(argv[2], &end, 10);
    if (total_lap <= 0 || total_lap > MAX_TOTAL_LAPS) {
        fprintf(stderr, "invalid total laps. please select 1 ~ %d\n", MAX_TOTAL_LAPS);
        exit(1);
    }
    
    if (input_hostname(hostname, sizeof(hostname)) == -1){
        fprintf(stderr, "invalid hostname\n");
        exit(1);
    }
    
    if ((sock = setup_m_srv(hostname, PORT, num_player)) == -1){
        fprintf(stderr, "cannot setup server\n");
        exit(1);
    }

    maxfd = m_accept(sock, num_player, enter);

    init_session(num_player, total_lap, maxfd);
    session_man();
    
    return 0;
}
