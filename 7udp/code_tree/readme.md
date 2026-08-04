# 7udp conference

## 코드 구조도
```
udp_conference
└──7_udp/
    ├── server.c
    ├── session_man.c
    ├── client.c 
    ├── mtalk2.h
    └── session.c
```
## 함수 기능들

```
udp_conference
└──7_udp/
    ├── server.c 
    │   └──main() 
    │        ├──setup_dg_server()
    │        └──session_man()
    ├── session_man.c
        ├── setup_dg_server() --> 호스트 바인드 및 talker[인덱스] 초기화
        ├── session_man() --> client로부터 soc 받아 case에 따라 로그인, 로그아웃, distribute 실행 
        ├── do_login() --> talker[slot] 이름 주소 저장 및 buf/LOGIN_OK 보내기
        ├── do_logout --> 로그아웃된 인덱스 출력 후 로그아웃되지 않는 인덱스에 sendto
        ├── find_free_slot() --> flag가 비어있으면 인덱스 번호 return
        ├── distribute() --> 전체 인원 중 logout 되지않은 명단 sendto
        └── ending() -->  logout 되지않은 사람 주소 sendto
    ├── client.c
        ├── setup_dg_client() --> 소켓 생성 바인드하고 login() return
        ├── init_session()  
        └── session() 
    ├── session.c 
        ├── login() --> 이름 보내주고 로그인 불/가능 판단 버퍼 받음
        ├── init_session() --> 초기화 및 화면 생성
        ├── session() -->  문자열 출력 및 화면에 생성
        ├── die() -->  logout 및 소켓, 전체 종료 
        └── logout() --> logout 문자열과 인덱스  서버로 보내는 함수 
    ├── mtalk2.h -->  함수들 정의
```
