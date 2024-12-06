import cv2
import os
import subprocess

subprocess.run("libcamera-still -o test.jpg",shell=True,check=True)
cascade_path = "haarcascade_frontalface_default.xml"

face_cascade = cv2.CascadeClassifier(cascade_path)

image_path = "test.jpg"  
image = cv2.imread(image_path)

if image is None:
    print("이미지를 로드할 수 없습니다.")
    exit()

gray_image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

faces = face_cascade.detectMultiScale(
    gray_image,
    scaleFactor=1.1,  # 이미지 스케일 비율
    minNeighbors=10,   # 최소 이웃 개수 (얼굴로 인정할 주변 박스 수)
    minSize=(200,200)  # 최소 얼굴 크기
)


print(f"감지된 얼굴 수: {len(faces)}")

# 얼굴에 박스 그리기
for (x, y, w, h) in faces:
    cv2.rectangle(image, (x, y), (x + w, y + h), (255, 0, 0), 2)


cv2.imshow("Detected Faces", image)

# ESC 키를 누르면 창 닫기
cv2.waitKey(0)
cv2.destroyAllWindows()

