#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "mod.c"

#define SERVER_PORT 502

// byte toBytes[TO_BYTES_LEN];

int main() {

    int serv_sock;
    int clnt_sock;

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;

    socklen_t clnt_addr_size;


    // 제 1단계 - 소켓 생성(socket 함수 사용)
    serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serv_sock == -1)
    {
        printf("socket() error \n");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;

    // htonl 함수 사용
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // htons 함수 사용
    serv_addr.sin_port = htons(SERVER_PORT);


    // 제 2단계 - IP 주소와 PORT 번호 할당함(bind 함수 사용)
    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        printf("bind() error \n");
        close(serv_sock);
        exit(1);
    }


    // 제 3단계 - 소켓을 연결 요청을 받아들일 수 있는 상태로 만듬(listen 함수 사용)
    /* listen 함수 원형
       int listen(sockfd, int backlog);
       backlog의 의미 : 대기 시킬 수 있는 최대 연결 갯수 */
    if (listen(serv_sock, 5) == -1)
    {
        printf("listen() error \n");
        close(serv_sock);
        // return;
        /* return;은 해당 함수를 정상적으로 끝내고서 운영체제 (Windows, Linux 등) 에게 기술한 값을 반환하고
           현재 실행 중인 해당 함수(main)를 벗어나겠다는 뜻이다.(main 함수 종료 또는 프로그램 종료) */
        exit(1);
    }


    // 제 4단계 - 연결 요청 이후(listen 함수 호출 이후) 연결 요청을 수락함.(accept 함수 호출)
    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

    if (clnt_sock == -1)
    {
        printf("accept() error \n");
        exit(1);
    }
    else
    {
        printf("accept() success!!!\n\n");
    }


    byte en_request[RTU_LEN];


    /* 클라이언트 -> 서버 데이터 수신할 때 read 함수 호출
       read 함수 원형
       ssize_t read(int fd, void * buf, size_t nbytes);
       성공 시 수신한 바이트 수(단, 파이의 끝을 만나면 0), 실패 시 -1 반환
       fd : 데이터 수신 대상을 나타내는 파일 디스크립터 전달
       buf : 수신한 데이터를 저장할 버퍼의 주소 값 전달
       nbytes : 수신할 최대 바이트 수 전달 */
    read(clnt_sock, en_request, RTU_LEN);

    int req_len = reqLenCheck(en_request);

    printf("[Slave] 수신한 request 패킷 : ");

    int i;

    for (i = 0; i < req_len; i++)
    {
        printf("%u ", en_request[i]);
    }
    printf("\n\n");

    printf("[Master] -> [Slave]\nrequest -> decoder 처리 결과 : \n");


    req_decoder(en_request, req_len);


    int pause;
    pause = getchar();   // system("pause") 함수를 대체하기 위해 getchar 함수 사용(system("pause")는 윈도우에서만 사용 가능)
    /* 변수 pause를 사용하는 이유?
       getchar 함수가 무언가를 반환한다는 것을 분명히 하기 위해서 사용한다. */
    /* getchar 함수의 기능
       stdin 에서 한 문자를 가져온다.
       표준 입력에서 다음 문자를 리턴한다. 이는 인자가 stdin인 getc 함수와 동일하다.
       그리고 getchar 함수는 버퍼를 비우는데에도 사용될 수 있다.

       getchar 함수의 원형
       int getchar(void);
       인자는 없고, 읽어들인 문자를 int 값으로 리턴한다. */



    /* 서버 -> 클라이언트 데이터 전송할 때 write 함수 호출
       write 함수 원형
       ssize_t write(int fd, const void *buf, size_t nbytes);
       성공 시 전달한 바이트 수, 실패 시 -1 반환
       fd : 데이터 전송 대상을 나타내는 파일 디스크립터 전달.
       buf : 전송할 데이터가 저장된 버퍼의 주소 값 전달.
       nbytes : 전송할 데이터의 바이트 수 전달. */
    /*  decoder 함수 사용해야함!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        (슬레이브 -> 마스터로 전달)*/

    // byte 배열 response 선언 및 초기화
    // byte response[] = { 00, 00, 00, 00, 00, 05, 01, 01, 02, 00, 00, 00};

    byte * response = NULL;

    response = rtuSetResponse(en_request, req_len);

    int res_len = sizeof(response) / sizeof(byte);
    write(clnt_sock, response, res_len);

    system("read -p 'Press Enter to continue...' var");
    // 참고 자료 url 주소 : https://askubuntu.com/questions/906742/what-is-sh-1-pause-not-found


    printf("[Slave] 송신한 response 패킷 : ");

    int k;

    for (int k = 0; k < res_len; k++)
    {
        printf("%u ", response[k]);
    }


    close(clnt_sock);
    close(serv_sock);

    return 0;
}


