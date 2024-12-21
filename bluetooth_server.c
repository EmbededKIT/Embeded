//앱을 통해 값이 제대로 보내지는지 확인하기 위한 라즈베리파이 C언어 코드

#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#define BAUD_RATE 115200
static const char* UART1_DEV = "/dev/ttyAMA1"; // RPi5: UART1 장치 파일
int card_number = 0; // 전역 변수

unsigned char serialRead(const int fd); // 1Byte 데이터를 수신하는 함수

// 1Byte 데이터를 수신하는 함수
unsigned char serialRead(const int fd) {
    unsigned char x;
    if (read(fd, &x, 1) != 1) // 1바이트 읽기 실패 시 -1 반환
        return -1;
    return x;
}

int main() {
    int fd_serial;
    unsigned char dat;

    // WiringPi 초기화
    if (wiringPiSetupGpio() < 0) {
        printf("WiringPi 초기화 실패\n");
        return 1;
    }

    // UART1 포트 열기
    if ((fd_serial = serialOpen(UART1_DEV, BAUD_RATE)) < 0) {
        printf("UART 포트를 열 수 없습니다: %s\n", UART1_DEV);
        return 1;
    }

    while (1) {
        if (serialDataAvail(fd_serial)) { // 읽을 데이터가 있는 경우
            dat = serialRead(fd_serial);  // 1바이트 데이터를 읽음
            
            // 읽은 데이터가 1~5인 경우만 처리
            if (dat >= '1' && dat <= '5') {
                card_number = dat - '0'; // 아스키 값을 정수로 변환
                printf("Card number: %d\n", card_number);
            } else {
                printf("무시된 데이터: %c\n", dat);0
            }
        }
        delay(10); // 짧은 대기 (10ms)
    }
}
// gcc –o bluetooth_server bluetooth_server.c –lwiringPi
// ./bluetooth_server
