#include <wiringPi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SERVO_PIN 12     // 서보모터 핀
#define SERVO_RANGE 2000 // 서보 모터 PWM 범위

#define DC_START 18   // DC 모터 핀
#define DC_END 19     // DC 모터 핀
#define DC_SPEED 100  // DC 모터 속도 (0 ~ 100)
#define DC_RANGE 1023 // DC 모터 PWM 범위

// 전역 변수 및 뮤텍스
pthread_mutex_t motor;

// 서보 모터 각도 이동
void rotate_servo(float angle)
{
    int duty = 150 + (angle * 100 / 90);

    pthread_mutex_lock(&motor);

    pwmWrite(SERVO_PIN, duty);
    delay(500); // 0.5초 대기

    pthread_mutex_unlock(&motor);

    printf("%.2f도 위치로 이동\n", angle);
}

// DC 모터 카드 분배
void rotate_dc(int speed)
{
    pthread_mutex_lock(&motor);

    for (int i = 0; i < 3; i++) // 카드 수만큼 반복
    {
        pwmWrite(DC_START, speed); // 카드 분배
        delay(700);
        pwmWrite(DC_START, 0);

        delay(500);

        pwmWrite(DC_END, speed); // 카드 밀어넣기
        delay(500);
        pwmWrite(DC_END, 0);
        pwmWrite(DC_START, 0);
    }

    pthread_mutex_unlock(&motor);

    printf("카드 분배 완료\n\n");
}

// 사용자 좌표
void *get_location(void *args)
{
    float *user = (float *)args; // 사용자 좌표 배열
    int user_count = (int)user[0];

    for (int i = 1; i <= user_count; i++)
    {
        printf("사용자 %d의 좌표: %.2f도\n", i, user[i]);
        rotate_servo(user[i]);                  // 서보 모터 각도 이동
        rotate_dc(DC_SPEED * (DC_RANGE / 100)); // DC 모터 카드 분배
    }
}

int main()
{
    if (wiringPiSetupGpio() == -1)
    {
        printf("GPIO 초기화 실패\n");
        return 1;
    }

    // 서보 모터 및 DC 모터 핀 설정
    pinMode(SERVO_PIN, PWM_OUTPUT);
    pinMode(DC_START, PWM_OUTPUT);
    pinMode(DC_END, PWM_OUTPUT);

    pwmSetMode(PWM_MODE_MS);
    pwmSetRange(SERVO_RANGE);
    pwmSetClock(19200000 / (50 * SERVO_RANGE));

    // 사용자 좌표 테스트
    // [0]: 사용자 수, [n]: 사용자 n 좌표
    float user[] = {4, 30.0, -45.0, 60.0, 90.0};

    pthread_t user_thread;

    pthread_mutex_init(&motor, NULL);                               // 뮤텍스 초기화
    pthread_create(&user_thread, NULL, get_location, (void *)user); // 쓰레드 생성
    pthread_join(user_thread, NULL);                                // 쓰레드 종료 대기
    pthread_mutex_destroy(&motor);                                  // 뮤텍스 제거

    printf("카드 분배 종료\n");

    return 0;
} 