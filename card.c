#include <wiringPi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

// 상수 정의
#define SERVO_PIN 12 // 서보모터 핀
#define SERVO_RANGE 2000 // 서보 모터 PWM 범위

#define DC_START 18 // DC 모터 핀
#define DC_END 19 // DC 모터 핀
#define DC_SPEED 100 // DC 모터 속도 (0 ~ 100)
#define DC_RANGE 1023 // DC 모터 PWM 범위

// 전역 변수 및 조건 변수
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t servo_at_position = PTHREAD_COND_INITIALIZER;

volatile int servo_position_reached = 0;
volatile float current_servo_angle = 0.0;
volatile int total_cards_distributed = 0;
volatile int pause_flag = 0;
volatile int user_count = 0;
volatile int card_count = 0;
volatile float *user_positions = NULL;
volatile int reset_flag = 0;

// GPIO 초기화 함수
void initialize_gpio()
{
    pinMode(SERVO_PIN, PWM_OUTPUT);
    pinMode(DC_START, PWM_OUTPUT);
    pinMode(DC_END, PWM_OUTPUT);

    pwmSetMode(PWM_MODE_MS);
    pwmSetRange(SERVO_RANGE);
    pwmSetClock(192);
}

// 사용자 설정 초기화
// 사용자 수, 카드 수 -> 블루투스 입력
// 사용자 위치 배열 -> 카메라
void init_setting()
{
    if (user_positions != NULL)
    {
        free((float *)user_positions);
    }

    // (블루투스 입력)
    printf("\n사용자 수를 입력하세요: ");
    scanf("%d", &user_count);
    printf("사용자당 카드 수를 입력하세요: ");
    scanf("%d", &card_count);

    // 사용자 위치 배열 동적 할당
    user_positions = (float *)malloc(user_count * sizeof(float));
    if (user_positions == NULL)
    {
        printf("메모리 할당 실패\n");
        exit(1);
    }

    FILE *file = fopen("angles.txt", "r");
    if (file == NULL) {
        perror("파일을 열 수 없습니다.");
        return 1;
    }

    char buffer[1024];
    int user_count = 0;

    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        char *token = strtok(buffer, ",");
        while (token != NULL && user_count < MAX_USERS) {
            user_positions[user_count] = atof(token);  // 문자열을 float로 변환
            user_count++;
            token = strtok(NULL, ",");
        }
    }

    fclose(file);

    // 사용자 위치 출력
    printf("불러온 사용자 위치:\n");
    for (int i = 0; i < user_count; i++) {
        printf("사용자 %d의 위치: %.2f\n", i + 1, user_positions[i]);
    }


    total_cards_distributed = 0;
    printf("\n[설정] 설정이 초기화되었습니다.\n");
}

// 입력 처리
void *bluetooth_input_handler(void *arg)
{
    char input[10];
    while (1)
    {
        printf("\n[명령 입력] p: 중단 | r: 재개 | i: 초기화: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "p") == 0)
        {
            printf("\n[명령] 동작 중단\n");
            pause_flag = 1;
        }
        else if (strcmp(input, "r") == 0)
        {
            printf("\n[명령] 동작 재개\n");
            pause_flag = 0;
        }
        else if (strcmp(input, "i") == 0)
        {
            printf("\n[명령] 설정 초기화\n");
            reset_flag = 1;
            break;
        }
        else
        {
            printf("\n[경고] 잘못된 명령\n");
        }
    }
    return NULL;
}

// 서보 모터
void *rotate_servo(void *arg)
{
    int index = 0; // 각도 이동 크기
    int direction = 1; // 1:증가, -1:감소
    int reset_to_start = 0; // -90도로 초기화

    while (!reset_flag && total_cards_distributed < user_count * card_count)
    {
        while (pause_flag && !reset_flag) // 중단 상태 대기
        {
            usleep(100000);
        }

        if (reset_flag) // 초기화 명령 수신 시 종료
            break;

        if (reset_to_start)
        {
            current_servo_angle = -90.0; // -90도로 이동
            int duty = 150 + (current_servo_angle * 100 / 90);
            if (duty < 50)
                duty = 50;
            if (duty > 250)
                duty = 250;

            pwmWrite(SERVO_PIN, duty);
            delay(1000);

            reset_to_start = 0; // 플래그 초기화
            index = 0;     // 인덱스 리셋
            direction = 1; // 방향 초기화

            printf("\n[서보 모터] -90도로 초기화 완료\n");

            continue;
        }

        current_servo_angle = -90.0 + 5.0 * index * direction;

        // 각도 범위 초과 시 방향 변경
        if (current_servo_angle > 90.0 || current_servo_angle < -90.0)
        {
            direction *= -1;

            // 모든 사용자에게 한 장씩 나눠줬다면 -90도로 초기화
            pthread_mutex_lock(&mutex);
            if (total_cards_distributed % user_count == 0)
            {
                reset_to_start = 1;
            }
            pthread_mutex_unlock(&mutex);

            index = 0;
            continue;
        }

        int duty = 150 + (current_servo_angle * 100 / 90);
        if (duty < 50)
            duty = 50;
        if (duty > 250)
            duty = 250;

        pwmWrite(SERVO_PIN, duty);
        delay(500);

        printf("\n[서보 모터] %.2f도 위치로 이동\n", current_servo_angle);

        // 사용자 위치 비교
        for (int i = 0; i < user_count; i++)
        {
            if (fabs(current_servo_angle - user_positions[i]) < 1e-1)
            {
                printf("[서보 모터] 사용자 %d의 위치 (%.2f도)에 도달\n", i + 1, user_positions[i]);

                pthread_mutex_lock(&mutex);
                servo_position_reached = i + 1; // 사용자 번호 저장
                pthread_cond_signal(&servo_at_position); // DC 모터로 신호 전달
                pthread_mutex_unlock(&mutex);
            }
        }

        index++;
    }

    printf("\n[서보 모터] 종료\n");
    return NULL;
}

// DC 모터 동작
void *rotate_dc(void *arg)
{
    int speed = *(int *)arg;
    speed = speed * (DC_RANGE / 100);
    int *user_card_count = (int *)calloc(user_count, sizeof(int));

    while (!reset_flag && total_cards_distributed < user_count * card_count)
    {
        pthread_mutex_lock(&mutex);
        while (!servo_position_reached && !reset_flag) // 서보모터 신호 대기
        {
            pthread_cond_wait(&servo_at_position, &mutex);
        }
        if (reset_flag)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }

        int user = servo_position_reached - 1; // 사용자 번호
        servo_position_reached = 0;
        pthread_mutex_unlock(&mutex);

        if (pause_flag)
            continue;

        if (user_card_count[user] < card_count)
        {
            user_card_count[user]++;
            total_cards_distributed++;

            pwmWrite(DC_START, speed);
            delay(700);
            pwmWrite(DC_START, 0);

            pwmWrite(DC_END, speed);
            delay(500);
            pwmWrite(DC_END, 0);

            pwmWrite(DC_START, 30);
            delay(100);
            pwmWrite(DC_START, 0);

            printf("\n[DC 모터] 사용자 %d에게 카드 %d 분배 완료\n", user + 1, user_card_count[user]);
        }
    }

    free(user_card_count);
    return NULL;
}

// 메인 함수
int main()
{
    if (wiringPiSetupGpio() == -1)
    {
        printf("GPIO 초기화 실패\n");
        return 1;
    }

    initialize_gpio();

    while (1)
    {
        init_setting();

        int dc_speed = DC_SPEED;
        pthread_t servo_thread, dc_thread, input_thread;

        reset_flag = 0;

        pthread_create(&input_thread, NULL, bluetooth_input_handler, NULL);
        pthread_create(&servo_thread, NULL, rotate_servo, NULL);
        pthread_create(&dc_thread, NULL, rotate_dc, (void *)&dc_speed);

        pthread_join(input_thread, NULL);

        reset_flag = 1;
        pthread_join(servo_thread, NULL);
        pthread_join(dc_thread, NULL);

        pthread_mutex_lock(&mutex);
        if (total_cards_distributed >= user_count * card_count)
        {
            pthread_mutex_unlock(&mutex);
            printf("\n[시스템] 모든 카드가 분배되었습니다. 프로그램을 종료합니다.\n");
            break;
        }
        pthread_mutex_unlock(&mutex);

        printf("\n[시스템] 초기화 완료. 새로운 설정을 적용합니다.\n");
    }

    free((float *)user_positions);
    printf("프로그램 종료\n");
    return 0;
}
