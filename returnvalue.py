import cv2

def detect_faces_and_get_angle(image_path):
    face_cascade = cv2.CascadeClassifier("haarcascade_frontalface_default.xml")

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

    for (x, w) in faces:
        face_center_x = x + w // 2  

        angle_x = int((face_center_x / img_width) * 180)

        print(f"Face detected at ({face_center_x}).")
        print(f"Mapped angle: X={angle_x} degrees")#test mapped angle
        return angle_x

    return None

# 테스트 실행
if __name__ == "__main__":
    image_path = "test.jpg"
    angle = detect_faces_and_get_angle(image_path)

