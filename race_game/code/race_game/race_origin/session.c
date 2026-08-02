// session.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/select.h>
#include <ncurses.h>

#include "race.h"
#include "session.h"

static PLAYER p[MAX_PLAYERS];
static PLAYER temp;
static GRADE g[MAX_PLAYERS];

static int num_players;
static int total_laps;
static int sock;
static int entry_num;
static int state;
static int final_state;
static char course[21][13];

static PLAYER *me;
static WINDOW *win_course, *win_board;

static int get_my_location();
static void send_my_data();
static void del_all_locations();
static void show_all_locations();
static void recv_all_data();

static int new_stage(int z);
static void game_over(int current_state);

static void init_data();
static int check(int mx, int my);

static void die(int sig);

void init_session(int s){
    char buf[32];
    sock = s;
    // 뒷자리 '\n' -> '\0'으로 전환
    read(sock, buf, 32);
    printf("%s", buf);
    if (fgets(buf, 32, stdin) == NULL)
        return;
    buf[strcspn(buf, "\n")] = '\0';
    // socket으로 다시 반환해줌
    write(sock, buf, strlen(buf)+1);

    read(sock, buf, 32);
    // 버퍼에 저장된 문자열 읽어 정수변환 뒤 enty_num변수 저장
    sscanf(buf, "%d", &entry_num);
    printf("> Your entry number is %d\n", entry_num);

    printf("Set READY (press ENTER)");
    getchar();

    write(sock, "OK", strlen("OK"));
    printf("\nWaiting other Player...\n");
    
    read(sock, buf, 32);    
    // 버퍼 읽어 플레이어 수 total_laps에 정수로 변환한뒤 저장
    sscanf(buf, "%d %d", &num_players, &total_laps);
    printf("> Players are %d\n", num_players);
    printf("> Total lap is %d\n", total_laps);
    
    printf("\nGame starts in...\n");


    // 1초마다 3,2,1 출력 후 시작하게 하는 함수 
    for (int i = 3; i >= 1; --i) {
        printf("%d...\n", i);
        fflush(stdout);
        sleep(1);
    }
    
    printf("GO!\n");
    fflush(stdout);
    sleep(1);     
    // 3,2,1 타이머 



    signal(SIGINT, die);
    signal(SIGQUIT, die);
    signal(SIGTERM, die);
    signal(SIGSEGV, die);
    signal(SIGPIPE, die);

    initscr();

    win_course = newwin(22, 12, 1, 10);
    box(win_course, 0, 0);
    wrefresh(win_course);

    win_board = newwin(11, 25, 5, 40);
    box(win_board, 0, 0);
    wrefresh(win_board);

    noecho();     // 입력 문자 숨김
    cbreak();     // Enter 없이 입력
    // nonl();
    curs_set(0);  // 커서 숨김
    keypad(win_course, TRUE);  // 방향키 사용
    
    new_stage(1);

    init_data();
    mvwprintw(win_board, 2, 1, "Entry Num = %d", entry_num);
    mvwprintw(win_board, 3, 2, "Num Lap Damage");
    mvwprintw(win_board, entry_num+4, 1, "*");
    show_all_locations();
}

void session()
{
    state = 1;

    while(state){
        state = get_my_location();
        send_my_data();
        del_all_locations();
        recv_all_data();
        show_all_locations();
    }
    game_over(final_state);
    die(0);
}

static int get_my_location(){
    int c, d;

    c = wgetch(win_course);
    switch(c){
        case KEY_UP:
            if (temp.y == 1){
                temp.lap++;
                temp.y = 20;
                if (new_stage(temp.lap)){
                    final_state = 0;
                    return 0;
                }
            }
            else if ((d = check(0, -1)) == 1)
                temp.y--;
            else 
                temp.damage++;
            break;

        case KEY_DOWN:
            if (check(0, 1) == 1)
                temp.y++;
            else 
                temp.damage++;
            break;

        case KEY_LEFT:
            if (check(-1, 0) == 1)
                temp.x--;
            else 
                temp.damage++;
            break;

        case KEY_RIGHT:
            if (check(1, 0) == 1)
                temp.x++;
            else 
                temp.damage++;
            break;
        
        default:
            break;
    }
    if (temp.damage >= MAX_DAMAGE){
        final_state = -1;
        return 0;
    }
    return 1;
}

static void send_my_data(){
    write(sock, &temp, sizeof(PLAYER));
}

static void del_all_locations(){
    for(int i=0; i<num_players; i++){
        if (p[i].lap == me->lap)
            mvwaddch(win_course, p[i].y, p[i].x, ' ');
    }
    wrefresh(win_course);
}

static void show_all_locations(){
    // const int curr_lap = me->lap;
    
    for(int i=0; i<num_players; i++){
        if (p[i].lap == me->lap)
            mvwaddch(win_course, p[i].y, p[i].x, (char)('0'+i));
    }
    wrefresh(win_course);

    mvwprintw(win_board, 9, 2, "laps=%2d/%2d", me->lap, total_laps);
    
    for(int i=0; i<num_players; i++){
        if (p[i].lap ==  me->lap)
            mvwprintw(win_board, i+4, 2, "%2d %3d %2d", i, p[i].lap, p[i].damage);
    }
    wrefresh(win_board);

    /* 각각 업데이트 안하고 한번에 업데이트 하려면*/
    // wnoutrefresh(win_course);
    // wnoutrefresh(win_board);
    // doupdate();
}

static void recv_all_data(){
    read(sock, p, sizeof(PLAYER)*num_players);
}

static int new_stage(int z){
    FILE *fp;

    static char buf[16];

    if (z > total_laps)
        return 1;

    sprintf(buf, "courses/c%d.txt", z);
    fp = fopen(buf, "r");
    for(int i=1; i<=20; i++){
        fgets(buf, 16, fp);
        buf[10] = '\0';
        wmove(win_course, i, 1);
        waddstr(win_course, buf);
        for(int j=1; j<=10; j++)
            course[i][j] = buf[j-1];
    }
    fclose(fp);
    return 0;
}

static void game_over(int current_state){
    wclear(win_board);
    box(win_board, 0, 0);
    if (current_state == 0)
        mvwprintw(win_board, 1, 3, "Goal-in!");
    else
        mvwprintw(win_board, 1, 3, "Retired!");
    wrefresh(win_board);

    read(sock, g, sizeof(GRADE)*num_players);
    for(int i=0; i<num_players; i++){
        mvwprintw(win_board, i+3, 1, " [%d] %d: %s", i+1, g[i].entrynum, g[i].name);
    }
    wrefresh(win_board);
    wgetch(win_board);
}

static void init_data(){
    me = p + entry_num;
    for(int i=0; i<num_players; i++){
        p[i].lap = 1;
        p[i].x = 2+i;
        p[i].y = 20;
        p[i].damage = 0;
    }
    temp = *me;
}

static int check(int mx, int my){
    int nx = temp.x + mx;
    int ny = temp.y + my;

    if (nx < 1 || nx > 10 || ny < 1 || ny > 21)
        return 0;
    if (course[ny][nx] == '*')
        return 0;
    return 1;
}

static void die(int sig){
    delwin(win_course);
    delwin(win_board);
    endwin();
    echo();
    exit(sig);
}