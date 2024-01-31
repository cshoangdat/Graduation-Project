from tkinter import *

from ultralytics import YOLO
import cv2
import cvzone
import math

from awscrt import mqtt, http
from awsiot import mqtt_connection_builder
import sys
import threading
import time
import json
import os
from multiprocessing import Process
import imutils
import numpy as np
import datetime

import torch

ip = None
password = None
gui_process = None

def on_close(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        print("Window closed by user")
        cv2.destroyAllWindows()

def delay(seconds):
    start_time = time.time()
    while time.time() < start_time + seconds:
        pass

# Callback when connection is accidentally lost.
def on_connection_interrupted(connection, error, **kwargs):
    print("Connection interrupted. error: {}".format(error))


# Callback when an interrupted connection is re-established.
def on_connection_resumed(connection, return_code, session_present, **kwargs):
    print("Connection resumed. return_code: {} session_present: {}".format(return_code, session_present))

    if return_code == mqtt.ConnectReturnCode.ACCEPTED and not session_present:
        print("Session did not persist. Resubscribing to existing topics...")
        resubscribe_future, _ = connection.resubscribe_existing_topics()

        # Cannot synchronously wait for resubscribe result because we're on the connection's event-loop thread,
        # evaluate result with a callback instead.
        resubscribe_future.add_done_callback(on_resubscribe_complete)


def on_resubscribe_complete(resubscribe_future):
    resubscribe_results = resubscribe_future.result()
    print("Resubscribe results: {}".format(resubscribe_results))

    for topic, qos in resubscribe_results['topics']:
        if qos is None:
            sys.exit("Server rejected resubscribe to topic: {}".format(topic))


# Callback when the subscribed topic receives a message
def on_message_received(topic, payload, dup, qos, retain, **kwargs):
    print("Received message from topic '{}': {}".format(topic, payload))

# Callback when the connection successfully connects
def on_connection_success(connection, callback_data):
    assert isinstance(callback_data, mqtt.OnConnectionSuccessData)
    print("Connection Successful with return code: {} session present: {}".format(callback_data.return_code, callback_data.session_present))

# Callback when a connection attempt fails
def on_connection_failure(connection, callback_data):
    assert isinstance(callback_data, mqtt.OnConnectionFailureData)
    print("Connection failed with error code: {}".format(callback_data.error))

# Callback when a connection has been disconnected or shutdown successfully
def on_connection_closed(connection, callback_data):
    print("Connection closed")

INPUT_ENDPOINT = "a3suuuxay09k3c-ats.iot.ap-southeast-2.amazonaws.com"
INPUT_PORT = 8883
TOPIC = "camera"

mqtt_connection = mqtt_connection_builder.mtls_from_path(
    endpoint= INPUT_ENDPOINT,
    port= INPUT_PORT,
    cert_filepath="./aws_cert/controlSystem.cert.pem",
    pri_key_filepath="./aws_cert/controlSystem.private.key",
    ca_filepath="./aws_cert/root-CA.crt",
    on_connection_interrupted=on_connection_interrupted,
    on_connection_resumed=on_connection_resumed,
    client_id= "camera",
    clean_session=False,
    keep_alive_secs=30,
    http_proxy_options= None,
    on_connection_success=on_connection_success,
    on_connection_failure=on_connection_failure,
    on_connection_closed=on_connection_closed)

    
def aws_connect():
    print(f"Connecting to {INPUT_ENDPOINT} with client ID camera...")
    connect_future = mqtt_connection.connect()
    # Future.result() waits until a result is available
    connect_future.result()
    print("Connected!")

def aws_subcribe(topic):
    print(f"Subscribing to topic {topic}...")
    subscribe_future, packet_id = mqtt_connection.subscribe(
        topic=topic,
        qos=mqtt.QoS.AT_LEAST_ONCE,
        callback=on_message_received)
    subscribe_result = subscribe_future.result()
    print("Subscribed with {}".format(str(subscribe_result['qos'])))

def aws_publish(topic, message_object, message_data):
    # message = {message_object: message_data}
    # print("Publishing message to topic '{}': {}".format(topic, message))
    # message_json = json.dumps(message)
    timestamp = datetime.datetime.now().strftime("%m/%d/%Y, %H:%M:%S")
    data_json = {
            "timestamp": timestamp,
            message_object[0]: message_data[message_object[0]],
            message_object[1]: message_data[message_object[1]],
            message_object[2]: message_data[message_object[2]],
            message_object[3]: message_data[message_object[3]],
            message_object[4]: message_data[message_object[4]],
            message_object[5]: message_data[message_object[5]],
            message_object[6]: message_data[message_object[6]],
            message_object[7]: message_data[message_object[7]]          
            }
    message_json = json.dumps(
        {
            "camera": data_json
        }, indent= 2
    )

    mqtt_connection.publish(
        topic=topic,
        payload=message_json,
        qos=mqtt.QoS.AT_LEAST_ONCE)
    # time.sleep(1)

def aws_disconnect():
    print("Disconnecting...")
    disconnect_future = mqtt_connection.disconnect()
    disconnect_future.result()
    print("Disconnected!")
    
def run(ip, password):
    aws_connect()
    aws_subcribe(TOPIC)
    count = 0
    cv2.namedWindow("Camera Detection App", cv2.WINDOW_NORMAL)
    cv2.setMouseCallback("Camera Detection App", on_close)
    cv2.resizeWindow("Camera Detection App", 1920, 1080)
    url = "rtsp://admin:" + password + "@" + ip + ":554/onvif1"
    cap = cv2.VideoCapture(url, cv2.CAP_FFMPEG)

    result, first_frame = cap.read()
    if not result:
        cap.release()
        cv2.destroyAllWindows()
        exit()
    first_frame = imutils.resize(first_frame, width=1920, height=1080)
    first_frame = cv2.cvtColor(first_frame, cv2.COLOR_BGR2GRAY) 
    first_frame = cv2.GaussianBlur(first_frame, (21, 21), 0)
    

    model = YOLO("./train_pt/16.1/best.pt")

    classNames = ['F_G1', 'F_G2', 'F_G3', 'F_G4', 'M_G1', 'M_G2', 'M_G3', 'M_G4']
    message_object = ['F_G1', 'F_G2', 'F_G3', 'F_G4', 'M_G1', 'M_G2', 'M_G3', 'M_G4']

    while True:    
        success, img = cap.read()
        results = model(img, stream = True)

        for r in results:
            boxes = r.boxes
            cls_tensor = r.boxes.cls
            class_count = {'F_G1': 0, 'F_G2': 0, 'F_G3': 0, 'F_G4': 0, 'M_G1': 0, 'M_G2': 0, 'M_G3': 0, 'M_G4': 0}
            for class_value in cls_tensor:
                class_value = int(class_value.item())
                match class_value:
                    case 0:
                        class_name = 'F_G1'
                    case 1:
                        class_name = 'F_G2'
                    case 2:
                        class_name = 'F_G3'
                    case 3:
                        class_name = 'F_G4'
                    case 4:
                        class_name = 'M_G1'
                    case 5:
                        class_name = 'M_G2'
                    case 6:
                        class_name = 'M_G3'
                    case 7:
                        class_name = 'M_G4'
                class_count[class_name] += 1
            
            count += 1
            for box in boxes:
                x1, y1, x2, y2 = box.xyxy[0]
                x1, y1, x2, y2 = int(x1), int(y1), int(x2), int(y2)

                w,h = x2 - x1, y2 - y1

                conf = math.ceil((box.conf[0]*100.0))/100

                cls = int(box.cls[0])

                currentClass = classNames[cls]
                cvzone.putTextRect(img,f'{classNames[cls]} {conf}',(max(0,x1),max(0,y1)), scale = 0.6, thickness = 1, offset = 3)
                cvzone.cornerRect(img,(x1,y1,w,h), l = 9,  rt = 1)
            if count > 100:
                aws_publish(TOPIC, message_object, class_count)
                count = 0
        cv2.imshow("Camera Detection App", img)
        key = cv2.waitKey(1)
        if cv2.getWindowProperty("Camera Detection App", cv2.WND_PROP_VISIBLE) < 1:
            break
    
    cap.release()
    cv2.destroyAllWindows()
        

def getData(ipInput, passInput):
    ip = ipInput.get()
    password = passInput.get()
    print("IP: ", ip)
    print("Pass: ", password)
    return ip, password
    
def gui():    
    root = Tk()

    root.geometry("1920x1080") 
    root.title("Camera Detection App")
    bg = PhotoImage(file = "background.png") 
    canvas1 = Canvas( root, width = 1920, 
                    height = 1080) 
    
    canvas1.pack(fill = "both", expand = True) 
    canvas1.create_image( -2, 0, image = bg,  
                        anchor = "nw") 
    
    canvas1.create_text(750, 80, text = "Intelligence Control System", fill="white", font="Mokoto 48 bold")
    canvas1.create_text(540, 300, text = "Camera IP Address:", fill = "white", font = "Mokoto 25 bold")
    canvas1.create_text(525, 420, text = "Admin Password:", fill = "white", font = "Mokoto 25 bold")

    ipInput = Entry(root, font = "Mokoto 25 bold")
    canvas1.create_window(925, 300, window=ipInput, width=400)

    passInput = Entry(root, font = "Mokoto 25 bold")
    canvas1.create_window(925, 420, window=passInput, width=400)
    
    def onSubmit():
        ip, password = getData(ipInput, passInput)
        root.destroy()
        video_process = Process(target= run, args=(ip,password))
        video_process.start()
        video_process.join()
        

    buttonSubmit = Button(root, text = "SUBMIT", font = "Mokoto 25 bold", command= onSubmit)

    button_canvas = canvas1.create_window(680, 540,  
                                        anchor = "nw", 
                                        window = buttonSubmit) 
    root.mainloop() 
    
def main():
    os.environ["OPENCV_FFMPEG_CAPTURE_OPTIONS"] = "rtsp_transport;udp"       
    gui_process = Process(target = gui)
    gui_process.start()
    gui_process.join() 

if __name__ == '__main__':
    main()