# 코드 수정된 상태
- quit/Quit/QUit등을 입력해도 quit으로 바뀌어 코드가 종료되며 다른 사용자들한테 보내짐

## 코드 수정 중
- 귓속말
  - client가 귓속말 모드 킴(원하는 이름 입력)
  - server에 이 내용 send -> server가 이름 보고 원하는 이름의 클라이언트 있는지 찾아보고 OK sign 내줌
  - 그럼 client가 연결됐다 확답 받고 이후부터 치는 말은 그 원ㅇ하는 이름의 클라이언트에게만 메시지 보냄
  -  

## 코드 문제점
* warning 
<img width="468" height="90" alt="image" src="https://github.com/user-attachments/assets/054eec9e-ddca-4886-ab9d-3e9462b72baa" />

- 오류 원인 = 정의한 struck sockaddr_in은 ipv4 전용 구조체이고, bind은 모든 구조체를 받을 수 있으므로 ipv4 구조체로 받을 수 있도록 &me 캐스팅 할  (struct sockaddr *)  붙여줘야 함
