![image](https://github.com/user-attachments/assets/58148735-48e0-42a7-9560-5465a1d6cff6)#Embeded KIT Project(8조)
**#에 자신이 맡은 파트 그림 및 내용 보충**
카드 분배기

# 초기설계(11/28~) 개발시작

## 11/28 회의 내용 및 아이디어
1. 앱과 라즈베리파이를 이용한 전자피아노
2. 스마트팜
3. 앱으로 조종하는 4륜 자동차
4. 카드분배기

최종 - 카드 분배기 선택

![image](https://github.com/user-attachments/assets/c2aa950d-0bdb-463d-b120-7cbc635fa9d9)


1. 서보모터와 DC모터를 사용해서 카드분배기를 만들어보자.
2. DC모터는 카드를 내보낼때 사용하고 서보모터는 카드를 뿌려줄 위치로 옮기면 되지 않을까?
3. 모터만 사용하면 심심하니 여기에 OpenCV를 이용하여 객체 탐지를 도입해보자.
4. 이를 통해 얼굴인식을 하고 해당 좌표로 쏴주는 역할은 어떤가?
5. 3D프린터를 통해 슬라이딩을 하면 카드가 한개 나올 수 있을만한 덱과 회전몸통을 만들어보겠다.

프로토타입
## Bluetooth 
- 앱을 활용하여 사용자 Input값 받고 전송
- 사용자 수와 카드 수 전달 
- APP UI

## Camera
- opencv를 활용하여 사용자의 얼굴 인식. (opencv HaarCascade_Frontface 사용)
- 해당좌표를 라즈베리파이로 전송 및 비율을 나누어서 서보로 값 전달
  
## Servo Motor
- 카메라에서 전달받은 값을 통해 0~180도로 사용자의 위치로 전송 이후 DC모터
- 한장씩 뿌리고 원위치 반복.

## DC Motor
- 기기 하단에서 슬라이딩방식으로 카드 던져주기
- 서보모터로부터 신호를 받아 카드 1장 발사

## GPIO GPN NUMBER
### DC Motor
- VCC: 5V Power (2)
- GND: GROUND (39)
- INA: GPIO 18 (PWM0) (12)
- INB: GPIO 19 (PWM1) (35)
 
### SERVO Motor
- VCC (빨간선): 3v3 Power (1)
- GND (갈색선): GROUND(34)
- CONTROL SIGNAL (주황선): GPIO 12 (PWM0) (32)
  
### BLUETOOTH
- VCC: 5v Power(4)
- GND: GROUND (6)
- TXD: GPIO 1 (EEPROM SCL) (28)
- RXD: GPIO 0 (EEPROM SDA) (27)


## 설계도중 문제점 발생

1. 기존 구상대로의 1mm 파이프라인불가능.  
-> 시중에 쉽게 구할 수 있는 5mm파이프라인으로 대체. 안정성은 올라갔으나 정교함이 떨어짐.
2. 스택방식으로 쌓이는 덱, 바텀쪽이 우선순위로 슬라이딩 되는 구조가 불가능
-> 바텀에서 꺼내려고 시도해봤으나 DC모터의 세기가 약해 쌓여있는 카드의 무게를 감당하지 못함. 
-> 또한 카드 한장의 두께만 나올 수 있게끔 설계가 불가능하므로, 탑에서 카드 슬라이딩으로 변경 


# 3D프린터를 활용하여 모델 설계
설계도
1. 서보모터를 위한 바텀
![image](https://github.com/user-attachments/assets/88834557-4756-470d-8b10-36896af8df37)
2. 카드덱
![image](https://github.com/user-attachments/assets/6499c3d8-b2e1-4d59-874d-8eac54819ade)
3. 카드 슬라이딩을 위한 DC모터의 지지부
![image](https://github.com/user-attachments/assets/7cc292a6-84c3-4d2e-b7a3-76a19c0b6e11)
4. 카드 발사를 위한 원형 실린더
![image](https://github.com/user-attachments/assets/9d7987f4-d08f-43d9-bc71-8bcd43a5e6c7)

실제 구현시 다양한 문제 발생
3D프린터는 1mm단위의 정교한 설계가 불가능. 이로 인해 다양한 문제점 발생

# App
 * UART 통신 이용
 * 참고: 강의자료 5-3 UART 통신 예제 코드 2
 * 동작 순서
   1. UART1 포트 열기
   2. 데이터 수신
   3. 읽은 데이터가 원하는 데이터인 경우에 저장
   4. 데이터 출력
 * 참고: Serial Bluetooth Terminal 앱
 * 앱 화면
    1. 1페이지(메인 화면)
      - 시작 버튼: 2페이지로 이동
      - 설명 버튼: 앱 기능 설명 출력
![1734445958737-1](https://github.com/user-attachments/assets/21f7a83d-1df3-4bcb-be1f-78ac0e5fe784)
  2. 2페이지(블루투스 연결 화면)
      - 연결 버튼: 페어링 된 라즈베리 파이 모듈과 연결
      - 숫자 입력 박스: 1~5사이의 수 입력
      - 전송 버튼: 입력한 수를 라즈베리 파이로 전송
![1734445958737-0](https://github.com/user-attachments/assets/0af7d213-2b35-4b2e-b85c-0cc636eee748)
 * 사용 기술
   1. 안드로이드 권한 요청(근처 기기)
   2. 페어링 된 블루투스 모듈과 연결
   3. 연결된 모듈로 값 전달

# 주요 개발 내용
## 1. softPWM 사용
- 사용자의 좌표에 정확히 카드를 보내기 위해 PWM 주파수를 Servo Motor를 중심으로 설정
    - Servo Motor에 맞춘 주파수 DC Motor의 비정상적인 작동 유발
        - 느린 회전 속도
        - 종종 pwm 신호를 받지 못하여 회전하지 않음
- softPWM을 사용함으로 DC Motor의 정상적으로 작동하도로 구현
    - softPwmCreate
    - softPwmWrite
      
## 2. 카메라 기능을 구현한 Python과의 코드 통합
- Python을 활용하여 사용자의 수와 좌표를 인식
  - 수집한 데이터를 .txt 파일에 저장
- 메인 함수에서 저장된 .txt 파일을 읽음
  - 사용자 수에 맞게 사용자의 위치 배열을 malloc()을 이용하여 동적 할당
  - 사용자 위치 좌표를 카드 딜러기의 배치(-90도 ~ 90도)에 맞게 적절히 변환
    - 좌표 변환을 .py 파일에서 처리 수 있지만, 하드웨어 테스트 시 좌표 변경이 잦기 때문에 메인 코드에서 처리하도록 설계 

## 문제점
1) 정확히 한장을 뿌릴 수 있는가?
-> 한장이 나올때도 있고, 두장이 나올 때도 있음
2) 카드가 겹쳐있다면 어떻게 대처할 것인가?
-> 대응 불가능...

## 변경점
1. 초기엔 앱에서 인원수와 카드수를 Input값으로 받음. 
Dectection을 통해 인원수값을 받아오고, 엡에서는 카드수만 받기로 변경(12/12)
#앱 사진 과정 및 로직 기입

2. DC모터가 균형이 맞지 않아 헛돌거나 뒤로 돔.
-> 뒤쪽에 추를 달아서 해결
   
![image](https://github.com/user-attachments/assets/b4f688cd-49fb-4d13-8e61-c7aaad2273ad)

약 1g짜리 클립들을 이용해 무게 조절에 성공

# 최종(12/15)
1. 완성본
   
![image](https://github.com/user-attachments/assets/10a6c95a-74f5-4bf8-860c-992cab3a0379)

2. 카메라
   
![image](https://github.com/user-attachments/assets/3f4b5313-11b8-4072-b476-f0c5ec201fc4)

## 구조도
![image](https://github.com/user-attachments/assets/637a5f62-8723-4eec-86e0-024c01724285)
1. 앱에서 나누어 줄 카드 수를 받으면
2. 멈춰있던 카메라가 깨어나 동작을 시작
3. Opencv를 통해 사용자 객체 및 좌표를 저장
4. 서보모터가 해당 좌표로 차례차례 회전한다.
5. 마지막 인원일 경우 초기화를 거쳐 다시 한장부터 처리한다.
6. 모든 카드 배분이 끝나면 종료

## mutex를 어떻게 설계하였는가?
- 이 카드분배기에서 딜러의 움직임을 따라하기 위해, DC모터와 서보모터의 동작을 동기화
- 즉 서보모터가 회전하면서 DC모터의 회전을 동시에 하기 위해 쓰레드 활용
![image](https://github.com/user-attachments/assets/cf2e5445-803d-4f56-8d32-33d6b5e60730)
### ServoMotor
```c++
pthread_mutex_lock(&mutex);
servo_reached = i + 1;
pthread_cond_signal(&servo_at_position);
pthread_mutex_unlock(&mutex);
```
### DCMotor
```c++
 pthread_mutex_lock(&mutex);
while (!servo_reached)
{
      pthread_cond_wait(&servo_at_position, &mutex);
}
servo_reached = 0;
pthread_mutex_unlock(&mutex);
```
### Demo영상(Youtube)
[![image](https://github.com/user-attachments/assets/9269e44c-bf87-46bb-bcef-e6a763f2947d)](https://www.youtube.com/shorts/FnUUYvhQEJk)





