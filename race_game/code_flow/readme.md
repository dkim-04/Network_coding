# Race_game 코드 흐름


## 1. 서버 동작 과정
```mermaid
flowchart LR
    main --> input_hostname
    input_hostname --> setup_m_srv
    setup_m_srv --> m_accept
    m_accept --> init_session
    init_session --> session_man
```

## 2. client 동작 과정
```mermaid
flowchart LR
    main --> input_hostname
    input_hostname --> setup_client
    setup_client --> init_session
    init_session --> session
```

## 3. Client-Server 통신 과정
```mermaid
sequenceDiagram
    participant Client
    participant Server

    Server->>Client write(sock,msg)
    Note left of Server<br/>session_man.c의 enter()를 m_accept에서 사용하여 hostname입력하도록 메시지 전송
```

