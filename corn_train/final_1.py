import cv2 as cv
from ultralytics import YOLO
from gpiozero import Servo
from time import sleep, time

# Load YOLOv8 model
model = YOLO('runs/detect/train/weights/best.pt')  # yolov8n.pt 권장

# Setup servo (GPIO 18)
servo = Servo(18, min_pulse_width=0.0006, max_pulse_width=0.0023)

def angle_to_value(angle):
    angle = max(0, min(180, angle))
    return (angle / 90.0) - 1

angle_map = {
    0: 40,
    1: 140
}

servo.value = angle_to_value(90)
sleep(0.6)
servo.value = None

cap = cv.VideoCapture(0)
if not cap.isOpened():
    print('Cannot open camera')
    exit()

TOP_THRESHOLD = 10
prev_time = time()

try:
    while cap.isOpened():
        success, frame = cap.read()
        if not success:
            break

        # FPS 계산 시작
        curr_time = time()
        fps = 1 / (curr_time - prev_time)
        prev_time = curr_time

        # YOLO 추론
        results = model(frame)
        annotated_frame = results[0].plot()

        # 감지된 객체 처리
        frame_height = frame.shape[0]
        for r in results:
            for box in r.boxes:
                cls_id = int(box.cls[0])
                x1, y1, x2, y2 = box.xyxy[0]

                if y1 < TOP_THRESHOLD:
                    if cls_id in angle_map:
                        angle = angle_map[cls_id]
                        val = angle_to_value(angle)
                        print(f"Top exit! Class {cls_id} → rotate to {angle}°")
                        servo.value = val
                        sleep(0.6)
                        servo.value = None

        # FPS 표시
        cv.putText(annotated_frame, f"FPS: {fps:.2f}", (10, 30),
                   cv.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)

        cv.imshow("YOLOv8 with Servo & FPS", annotated_frame)

        if cv.waitKey(1) & 0xFF == ord("q"):
            break

except KeyboardInterrupt:
    print("\n종료 중...")

finally:
    cap.release()
    cv.destroyAllWindows()
    servo.value = None
    print("서보 제어 종료")
