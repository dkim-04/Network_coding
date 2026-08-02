
# 레이싱 게임 코드

### 코드 구조도
```
race_game
├──race_origin/
│   ├── session_man.c
│   ├── server.c
│   ├── client.c
│   ├── session.c
│   ├── snet.h
│   ├── session_man.h
│   ├── session_man.h
│   ├── race.h
│   └── makefile
├──lib/
│   ├── setup_m_srv.c
│   ├── setup_server.c
│   ├── setup_client.c
│   ├── setup_host.c
│   ├── snet.a
│   └── makefile
└──courses/
    ├── c1.txt --> 2차원 배열에 옮길 도로 코스 텍스트
    └── c2.txt --> c1.txt과 역할 동일
```

### 함수 기능들
```

1. lib 폴더 

- 1-1. setup_m_srv.c
        ├── setup_m_srv(hostname,port,backlog) --> server가 client 입장을 받아들이기 전까지의 과정 준비하는 코드 (전체 인원 한명씩 다 받음)
        ├── m_accept(soc,limit,(*func)(int,int))-  func()=enter()함수:session_man.c 존재  --> 클라이언트 연결 수락 및 enter()함수를 이용한 client와 소켓 주고 받기
        └── accept_client(soc) --> client connect() 들어오면 통신용 소켓 fd 반환 

- 1-2. setup_server.c
        └── setup_server(hostname,port) --> setup_m_srv전 통신용 소켓 반환하던 함수

- 1-3. setup_client.c
        └── setup_client(hostname,port) --> hostname 입력하고 client용 통신용 소켓 반환

- 1-4. setup_host.c
        └── input_hostname(hostname[],size) - size_t:메모리 크기나 배열 크기 저장하기 위한 정수형 자료 --> hostname 키보드 입력해 hostname[]에 저장
-----------------------

2. race_origin 폴더 

- 2-1. session_man.c
        ├── void enter(i,fd) -> m_accept()에서 client 소켓 주고받은 다음 플레이어에게 순번 write 보내줌
        ├── void init_session(n,fin,maxfd) --> select 감시 위해 mask 초기화해 비트마스크 준비후 초기 데이터 전원에게 보냄
        ├── void session_man() --> 경기 중 변환된 데이터 구조체 recv() 받고 업데이트 후 다시 client에게 send()
        ├── void recv_data() --> client에게 플레이어 데이터 구조체 받아서 내 P구조체에 덮어 씌움 
        ├── void send_dat() --> finished, retired 변한 데이터 구조체 전송
        └── void end() --> 모든 플레이어의 g 구조체 전송

- 2-2. server.c
        ├── main(int argc,char *argv[]) --> 서버 시작 및 게임 진행

- 2-3. session.c
        ├──init_session(int s) --> 게임 시작 및 게임 진행 전까지의 모든 과정 진행 
        ├── init_data() --> 모든 플레이어 기록 초기화
        ├── session() --> 게임 진행 및 종료
        ├── get_my_location --> 입력된 키에 따라 나의 위치 이동
        ├── send_my_data() --> 이동된 나의 데이터를 서버에 전송
        ├── del_all_locations() --> 나와 같은 랩에 있는 모든 플레이어의 현재위치 공백 처리
        ├── show_all_locations() --> player 구조체의 내용을 실제로 화면에 갱신후 시각화하는 코드
        ├── recv_all_data() --> 서버에서 업데이트 된 P구조체 recv받음
        ├── new_stage() --> 새로운 코스 도로 생성
        ├── game_over() --> 서버 end() 함수에서 g 구조체 받고 화면 갱신 후 종료
        ├── check() --> 커서 위치를 다음 위치로 움직일 수 있는지 확인
        └── die() --> window 종료하고 exit() 안의 값에 따라 종료

- 2-4. client.c
        └── main() --> client 실행 및 client 게임 진행

-----------------------


```
