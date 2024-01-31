import tkinter as tk
import cv2
from PIL import Image, ImageTk

from ultralytics import YOLO
import cv2
import cvzone
import math

class VideoApp(tk.Tk):
    def __init__(self, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)

        container = tk.Frame(self)
        container.pack(side="top", fill="both", expand=True)
        container.grid_rowconfigure(0, weight=1)
        container.grid_columnconfigure(0, weight=1)

        self.frames = {}

        for F in (StartPage, VideoPage):
            frame = F(container, self)
            self.frames[F] = frame
            frame.grid(row=0, column=0, sticky="nsew")

        self.show_frame(StartPage)

    def show_frame(self, cont):
        frame = self.frames[cont]
        frame.tkraise()

class StartPage(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="Start Page")
        label.pack(pady=10,padx=10)
        button = tk.Button(self, text="Go to Video Page",
                           command=lambda: controller.show_frame(VideoPage))
        button.pack()

class VideoPage(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.controller = controller
        label = tk.Label(self, text="Video Page")
        label.pack(pady=10,padx=10)

        # Create a label to display the video
        self.video_label = tk.Label(self)
        self.video_label.pack()

        # Open the video file
        self.cap = cv2.VideoCapture('./Videos/bikes.mp4')

        # Start displaying the video
        self.display_video()

    def display_video(self):
        ret, frame = self.cap.read()
        model = YOLO("./train_pt/24.11/best.pt")

        classNames = ['F_G1', 'F_G2', 'F_G3', 'F_G4', 'M_G1', 'M_G2', 'M_G3', 'M_G4']
        results = model(frame, stream = True)
        for r in results:
            boxes = r.boxes
            # print(len(r.boxes.data))
            # print(r.boxes.cls)
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
            
            # for class_name, count in class_count.items():
            #     print(f"{count} {class_name}")

            # count += 1
            for box in boxes:
                x1, y1, x2, y2 = box.xyxy[0]
                x1, y1, x2, y2 = int(x1), int(y1), int(x2), int(y2)

                w,h = x2 - x1, y2 - y1

                conf = math.ceil((box.conf[0]*100.0))/100

                cls = int(box.cls[0])

                currentClass = classNames[cls]
                cvzone.putTextRect(frame,f'{classNames[cls]} {conf}',(max(0,x1),max(0,y1)), scale = 0.6, thickness = 1, offset = 3)
                cvzone.cornerRect(frame,(x1,y1,w,h), l = 9,  rt = 1)
            # if count > 100:
            #     aws_publish(TOPIC, message_object, class_count)
            #     count = 0
        if ret:
            frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            frame = Image.fromarray(frame)
            photo = ImageTk.PhotoImage(image=frame)
            self.video_label.config(image=photo)
            self.video_label.image = photo
            self.after(30, self.display_video)  # Display the next frame after 30 milliseconds
        else:
            # Video has ended, go back to the start page
            self.controller.show_frame(StartPage)
            self.cap.release()

app = VideoApp()
app.mainloop()