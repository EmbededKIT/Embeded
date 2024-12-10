import cv2
import subprocess
def detect_faces_and_get_angle(image_path):
    face_cascade = cv2.CascadeClassifier("haarcascade_frontalface_default.xml")
    output_file="angles.txt"
    image = cv2.imread(image_path)
    if image is None:
        print("Error: Could not load image.")
        return None

    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    faces = face_cascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5, minSize=(30, 30))

    if len(faces) == 0:
        print("No faces detected.")
        return None

    img_height, img_width = gray.shape
    angles = []  

    for (x, y, w, h) in faces:
        face_center_x = x + w // 2
        angle_x = int((face_center_x / img_width) * 180)
        angles.append(angle_x)  

        print(f"Face detected at ({face_center_x}).")
        print(f"Mapped angle: X={angle_x} degrees")

    # 각도를 파일에 저장
    with open(output_file, "w") as file:
        file.write(",".join(map(str, angles)))

    print(f"Angles saved to {output_file}.")
    return angles

# 테스트 실행
if __name__ == "__main__":
    subprocess.run("libcamera-still -o test.jpg", shell=True,check=True)
    image_path = "test.jpg"
    anglelist=[0,0,0,0,0,0]
    angle = detect_faces_and_get_angle(image_path)

