import cv2
import numpy as np
import math
import time
from ultralytics import YOLO

model = YOLO('yolov8n.pt') 
model.classes = [0]  

cap = cv2.VideoCapture(0)  
width = int(cap.get(3))
height = int(cap.get(4))
fps = cap.get(cv2.CAP_PROP_FPS)


resultFINAL = cv2.VideoWriter('demovideo.avi', cv2.VideoWriter_fourcc(*'XVID'), fps, (width, height))

frameno = 0
num_people = 0
fpsStart = 0

def box_coords(box):
    return [int(box[0]), int(box[1]), int(box[2]), int(box[3])]

def checkbot_box(coords, height):
    return coords[3] > height - (height / 54)

def box_cent(coords):
    return [int((coords[0] + coords[2]) / 2), int((coords[1] + coords[3]) / 2)]

def inters_area(coord1, coord2):
    dx = min(coord1[2], coord2[2]) - max(coord1[0], coord2[0])
    dy = min(coord1[3], coord2[3]) - max(coord1[1], coord2[1])
    return dx * dy if dx > 0 and dy > 0 else 0

def newbox(coordlist, i_list):
    return [k for k in coordlist if k not in [i[0] for i in i_list]]

def dispbox(prev_coordlist, i_list):
    return [k for k in prev_coordlist if k not in [i[1] for i in i_list]]

def matchboxes(coordlist, prev_coordlist, width):
    i_list = []
    for coord in coordlist:
        area = 0
        match = []
        for prev_coord in prev_coordlist:
            if inters_area(coord, prev_coord) > area and (math.dist(box_cent(coord), box_cent(prev_coord)) < width / 20):
                area = inters_area(coord, prev_coord)
                match = [[coord, prev_coord]]
        if match and coord not in [i[0] for i in i_list] and match[0][1] not in [j[1] for j in i_list]:
            i_list += match
    return i_list

def COUNT_PEOPLE_FRAMEOUT(dataPre, dataCur, frame, frameCopy, num_people):
    prev_coordlist = [box_coords(box.xyxy[0].cpu().numpy()) for box in dataPre.boxes]
    coordlist = [box_coords(box.xyxy[0].cpu().numpy()) for box in dataCur.boxes]

    for c in coordlist:
        cv2.rectangle(frameCopy, (c[0], c[1]), (c[2], c[3]), (255, 0, 0), thickness=-1)

    i_list = matchboxes(coordlist, prev_coordlist, width)
    new_list = newbox(coordlist, i_list)
    disp_list = dispbox(prev_coordlist, i_list)

    for new_coords in new_list:
        if checkbot_box(new_coords, height):
            num_people -= 1
            cv2.rectangle(frameCopy, (new_coords[0], new_coords[1]), (new_coords[2], new_coords[3]), (0, 0, 255), thickness=-1)

    for disp_coords in disp_list:
        if checkbot_box(disp_coords, height):
            num_people += 1
            cv2.rectangle(frameCopy, (disp_coords[0], disp_coords[1]), (disp_coords[2], disp_coords[3]), (0, 255, 0), thickness=-1)

    frame = cv2.addWeighted(frameCopy, 0.3, frame, 0.7, 1.0)
    return frame, num_people

def GPIO_LIGHT(numPeople, frame):
    color = (255, 255, 255) if numPeople > 0 else (0, 0, 0)
    cv2.circle(frame, (int(width * 0.9), int(height * 0.9)), 30, color, thickness=cv2.FILLED)

prev_results = None

while True:
    ret, frame = cap.read()
    if not ret:
        break

    frameno += 1
    rect_frame = frame.copy()

    results = model(frame, classes=[0], verbose=False)[0]

    if prev_results is None:
        prev_results = results

    frame, num_people = COUNT_PEOPLE_FRAMEOUT(prev_results, results, frame, rect_frame, num_people)
    GPIO_LIGHT(num_people, frame)

    fpsEnd = time.time()
    timeDiff = fpsEnd - fpsStart if fpsStart else 1
    fps = 1 / timeDiff
    fpsStart = fpsEnd

    cv2.putText(frame, f"FPS: {fps:.2f}", (30, 40), cv2.FONT_HERSHEY_COMPLEX, 1, (0, 255, 255), 2)
    cv2.putText(frame, f"Number of people: {num_people}", (int(width / 40), height - int(width / 40)),
                cv2.FONT_HERSHEY_SIMPLEX, round(width / 1000), (0, 0, 255), round(width / 1000), cv2.LINE_AA)

    cv2.imshow("result", frame)
    resultFINAL.write(frame) 

    prev_results = results

    key = cv2.waitKey(1) & 0xFF
    if key in [ord('q'), ord('Q')]: 
        break
    elif key in [ord('r'), ord('R')]:
        num_people = 0

cap.release()
resultFINAL.release()
cv2.destroyAllWindows()