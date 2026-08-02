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

// 3초 타이머 세서 게임 시작하고 
void init_session(int s){
    char buf[32];
    sock = s;
    // 뒷자리 '\n' -> '\0'으로 전환
    read(sock, buf, 32);
    printf("%s", buf);
    // 서버에서 보내준 type your name 옆에서 이름 입력
    if (fgets(buf, 32, stdin) == NULL)
        return;
    buf[strcspn(buf, "\n")] = '\0';
    // socket으로 다시 반환해줌
    write(sock, buf, strlen(buf)+1);

    // 플레이어 (자신의) 순번 읽어오기 
    read(sock, buf, 32);
    // 버퍼에 저장된 문자열 읽어 정수변환 뒤 enty_num변수 저장
    sscanf(buf, "%d", &entry_num);
    printf("> Your entry number is %d\n", entry_num);

    printf("Set READY (press ENTER)");
    getchar();
    // 서버에 OK 문자열 보내주기
    write(sock, "OK", strlen("OK"));
    printf("\nWaiting other Player...\n");
    
    // msg 읽어  플레이어 수 total_laps 정수로 변환한뒤 저장
    read(sock, buf, 32);    
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


    // 종료 조건 설정
    signal(SIGINT, die);    // ctrl+C 누르면 die(int) 실행
    signal(SIGQUIT, die);   // ctrl+\ 누르면 die(int) 실행
    signal(SIGTERM, die);   // 프로그램 종료 요청 들어오면 die(int) 실행
    signal(SIGSEGV, die);   // 잘못된 메모리에 접근하면 die(int) 실행
    signal(SIGPIPE, die);   // client가 종료했는데 서버가 write하는 Brokenpipe 발생하면 die(int) 실행
    

    // window 키기
    initscr();
    
    // 도로 생성
    // newwin= ncurses 정의된 함수 newwin(높이,너비, 시작 y,시작 x  )
    win_course = newwin(22, 12, 1, 10); 
    box(win_course, 0, 0);  // 테두리 그리기
    wrefresh(win_course);   //메모리 창 -> 실제 터미널 화면 출력


    // 점수판 생성
    win_board = newwin(11, 25, 5, 40);  
    box(win_board, 0, 0);
    wrefresh(win_board);

    noecho();     // 입력 문자 숨김
    cbreak();     // Enter 없이 입력
    // nonl();
    curs_set(0);  // 커서 숨김
    keypad(win_course, TRUE);  // 방향키 사용
    
    // courses 1.txt | 2차원배열에 저장 및 커서 옮기고 커서 위치에 buf 문자열 출력 
    new_stage(1);

    //모든 플레이어 기록 초기화
    init_data();
    // 기록판에서 (y,x) 위치로 이동 -> Entry Num= %d 출력
    mvwprintw(win_board, 2, 1, "Entry Num = %d", entry_num);
    mvwprintw(win_board, 3, 2, "Num Lap Damage");
    mvwprintw(win_board, entry_num+4, 1, "*");  
    show_all_locations(); // player 구조체의 내용을 실제로 화면에 갱신후 시각화하는 코드
}

void session()
{
    state = 1;

    while(state){
        state = get_my_location(); // 내 위치를 키보드 입력에 따라 받아서 이동
        send_my_data(); // Player temp(플레이 과정 데이터) 서버로 보내기
        del_all_locations();    // 모든 플레이어에 대해 me와 같은 랩에 있으면 현재 위치 공백으로 바꿈
        recv_all_data();        // 소켓에서 모든 플레이어 구조체를 읽어서 p에 저장하는 코드
        show_all_locations();   // player 구조체의 내용을 실제로 화면에 갱신후 시각화하는 코드
    }
    game_over(final_state);  
    die(0);
}


static int get_my_location(){
    int c, d;

    // 키보드 입력을 받아 c에 저장
    c = wgetch(win_course);
    switch(c){
        // 
        case KEY_UP:
            // 골인선 도착했으면 랩 증가 후 시작점으로 이동
            if (temp.y == 1){
                temp.lap++;
                temp.y = 20;
                // 만약 랩이 끝이면 새로운 코스 넘어가지 않고 종료
                if (new_stage(temp.lap)){
                    final_state = 0;
                    return 0;
                }
            }
            // 한 칸위로 이동할 수 있는지 확인 후 가능하면 이동
            else if ((d = check(0, -1)) == 1)
                temp.y--;   
            else 
                //아니면 데미지 1증가
                temp.damage++;
            break;
        // 아래로 내려가는 키
        case KEY_DOWN:
            if (check(0, 1) == 1)
                temp.y++;
            else 
                temp.damage++;
            break;
        // 왼쪽으로 움직이는 키
        case KEY_LEFT:
            if (check(-1, 0) == 1)
                temp.x--;
            else 
                temp.damage++;
            break;
        // 오른쪽으로 움직이는 키
        case KEY_RIGHT:
            if (check(1, 0) == 1)
                temp.x++;
            else 
                temp.damage++;
            break;
        // 4가지 상황이 아니라면 종료
        default:
            break;
    }
    // 데미지 최대로 받으면 종료
    if (temp.damage >= MAX_DAMAGE){
        final_state = -1;
        return 0;
    }
    return 1;
}

// Player temp에 저장된 데이터 서버로 보내기
static void send_my_data(){
    write(sock, &temp, sizeof(PLAYER));
}

// 모든 플레이어에 대해 me와 같은 랩에 있으면 현재 위치 공백으로 바꿈
static void del_all_locations(){
    for(int i=0; i<num_players; i++){
        if (p[i].lap == me->lap)
            mvwaddch(win_course, p[i].y, p[i].x, ' ');
    }
    wrefresh(win_course);
}

// player 구조체의 내용을 실제로 화면에 갱신후 시각화하는 코드
static void show_all_locations(){
    // const int curr_lap = me->lap;

    for(int i=0; i<num_players; i++){
        // 모든 플레이어 중 나와 같은 lab에 있는 플레이어만 현재위치를 코스위에 숫자로 표현
        if (p[i].lap == me->lap)
            mvwaddch(win_course, p[i].y, p[i].x, (char)('0'+i));
    }
    // 창의 내용을 실제로 갱신
    wrefresh(win_course);
    점수판 커서를 (y,x)로 옮긴 뒤 그 위치에 내 랩과 총 랩 수 출력
    mvwprintw(win_board, 9, 2, "laps=%2d/%2d", me->lap, total_laps);
    
    for(int i=0; i<num_players; i++){
        if (p[i].lap ==  me->lap)
            점수판 커서를 옮긴 뒤 그 플레이어의 랩수와 데미지 출력
            mvwprintw(win_board, i+4, 2, "%2d %3d %2d", i, p[i].lap, p[i].damage);
    }
    wrefresh(win_board);

    /* 각각 업데이트 안하고 한번에 업데이트 하려면*/
    // wnoutrefresh(win_course);
    // wnoutrefresh(win_board);
    // doupdate();
}
// 소켓에서 player 구조체를 읽어오는 코드
static void recv_all_data(){
    read(sock, p, sizeof(PLAYER)*num_players);
}

// 새로운 코스 도로 생성
static int new_stage(int z){
    FILE *fp;

    static char buf[16];

    if (z > total_laps)
        return 1;

    // buf에 코스 텍스트 파일 도로를 buf에 저장
    sprintf(buf, "courses/c%d.txt", z);
    // 읽기 모드로 열기
    fp = fopen(buf, "r");
    for(int i=1; i<=20; i++){
        // 코스 텍스트 파일 buf에서 가져와 fp에 저장
        fgets(buf, 16, fp);
        buf[10] = '\0';
        // 창 안의 커서를 (y,x) 위치로 이동
        wmove(win_course, i, 1);
        //현재 커서 위치에 버퍼 문자열 출력
        waddstr(win_course, buf);

        for(int j=1; j<=10; j++)
            //buf에서 읽어온 문자열을 course 2차원 배열에 저장
            course[i][j] = buf[j-1];
    }
    // fp 종료
    fclose(fp);
    return 0;
}

// 마지막 game 종료 코드
static void game_over(int current_state){
    wclear(win_board);
    box(win_board, 0, 0);
    if (current_state == 0)
        mvwprintw(win_board, 1, 3, "Goal-in!");
    else
        mvwprintw(win_board, 1, 3, "Retired!");
    wrefresh(win_board);

    read(sock, g, sizeof(GRADE)*num_players);
    // 모든 플레이어들의 점수판에서 커서를 i+3,1로 이동시켜 출력 함
    for(int i=0; i<num_players; i++){
        mvwprintw(win_board, i+3, 1, " [%d] %d: %s", i+1, g[i].entrynum, g[i].name);
    }
    // 화면 갱신
    wrefresh(win_board);
    // 아무 키나 입력 후 종료
    wgetch(win_board);
}

// 모든 플레이어 기록 초기화
static void init_data(){
    // 플레이어 구조체의 [entry_num] 주소를 가리킴
    me = p + entry_num;
    for(int i=0; i<num_players; i++){
        // 모든 플레이어 기록 초기화
        p[i].lap = 1;
        p[i].x = 2+i;
        p[i].y = 20;
        p[i].damage = 0;
    }
    temp = *me;
}

// temp player 다음 위치 갈 수 있는지 확인하는 코드
static int check(int mx, int my){
    int nx = temp.x + mx;
    int ny = temp.y + my;

    if (nx < 1 || nx > 10 || ny < 1 || ny > 21)
        return 0;
    if (course[ny][nx] == '*')
        return 0;
    return 1;
}


// ncurses 모드를 종료하고 입력된 int sig에 따라 exit되는 코드
static void die(int sig){
    // win_course 라는 ncurses window창 삭제
    delwin(win_course);
    // 점수판 삭제
    delwin(win_board);
    // ncurses 모드 종료 후 터미널을 원래 상태로 종료됩니다.
    endwin();
    // 입력한 문자를 화면에 다시 보이도록 함
    echo();
    // exit(int)에 따라 종료
    exit(sig);
}