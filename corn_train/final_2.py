import cv2 as cv
from ultralytics import YOLO
import RPi.GPIO as GPIO
from time import sleep, time
import threading

# === Step Motor Pins ===
STEP_PUL = 20  # Pulse pin
STEP_DIR = 21  # Direction pin

# === Servo Pin ===
SERVO_PIN = 18

# === GPIO Setup ===
GPIO.setmode(GPIO.BCM)
GPIO.setup(SERVO_PIN, GPIO.OUT)
GPIO.setup(STEP_PUL, GPIO.OUT)
GPIO.setup(STEP_DIR, GPIO.OUT)

# Set direction (LOW or HIGH)
GPIO.output(STEP_DIR, GPIO.LOW)  # LOW: one direction, HIGH: reverse

# Servo PWM setup
pwm = GPIO.PWM(SERVO_PIN, 50)  # 50Hz
pwm.start(0)

# Convert angle (0~180) to duty cycle
def angle_to_duty(angle):
    angle = max(0, min(180, angle))
    return 2 + (angle / 18)

# Mapping of class IDs to servo angles
angle_map = {
    0: 105,
    1: 120,
    2: 75,
    3: 60
}

# Start servo at neutral position
pwm.ChangeDutyCycle(angle_to_duty(90))
sleep(0.6)
pwm.ChangeDutyCycle(0)

# === Step motor function (runs in background thread) ===
def step_motor_loop():
    try:
        while True:
            GPIO.output(STEP_PUL, GPIO.HIGH)
            sleep(0.0012)  # 1200 microseconds high
            GPIO.output(STEP_PUL, GPIO.LOW)
            sleep(0.0012)  # 1200 microseconds low
    except:
        pass  # Exit thread silently when main ends

# Start step motor in background thread
step_thread = threading.Thread(target=step_motor_loop, daemon=True)
step_thread.start()

# === Load YOLO model ===
model = YOLO('/home/hwang/corn/runs/detect/train/weights/best.pt')

# Initialize camera
cap = cv.VideoCapture(0)
if not cap.isOpened():
    print('Cannot open camera')
    GPIO.cleanup()
    exit()

TOP_THRESHOLD = 10
prev_time = time()

try:
    while cap.isOpened():
        success, frame = cap.read()
        if not success:
            break

        # Calculate FPS
        curr_time = time()
        fps = 1 / (curr_time - prev_time)
        prev_time = curr_time

        # Run YOLO detection
        results = model(frame)
        annotated_frame = results[0].plot()

        # Analyze detections
        for r in results:
            for box in r.boxes:
                cls_id = int(box.cls[0])
                x1, y1, x2, y2 = box.xyxy[0]

                if y1 < TOP_THRESHOLD:
                    if cls_id in angle_map:
                        angle = angle_map[cls_id]
                        duty = angle_to_duty(angle)
                        print(f"Top exit detected! Class {cls_id} -> rotate to {angle} degrees")
                        pwm.ChangeDutyCycle(duty)
                        sleep(0.6)
                        pwm.ChangeDutyCycle(0)

        # Display FPS
        cv.putText(annotated_frame, f"FPS: {fps:.2f}", (10, 30),
                   cv.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
        cv.imshow("YOLOv8 with Servo & Step Motor", annotated_frame)

        if cv.waitKey(1) & 0xFF == ord("q"):
            break

except KeyboardInterrupt:
    print("\nTerminating...")

finally:
    cap.release()
    cv.destroyAllWindows()
    pwm.stop()
    GPIO.cleanup()
    print("Camera, servo, and step motor shutdown complete.")
