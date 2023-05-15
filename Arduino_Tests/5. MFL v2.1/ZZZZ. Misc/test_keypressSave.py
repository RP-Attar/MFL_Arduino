import serial
from pynput import keyboard
from time import sleep

# Set up to read serial monitor
cc = "hi"
testNum = 1
flag = 0

def on_press(key):
    global flag
    print(key)
    with keyboard.Events() as events:
        for event in events:
            if event.key == keyboard.Key.esc:                
                flag = 1
                print("flag changed")

def writeToFile():
    global flag
    global testNum
    while True:
        print(testNum)
        testNum = testNum + 1
        sleep(0.5)
        
        if flag == 1:               # if key 'q' is pressed 
            flag = 0                # reset flag
            print("end")
            return                  # exit function

listener = keyboard.Listener(on_press=on_press)

# Continually reading and acting 
while True:            
    fileName = input("Enter to start")
    listener.start()
    writeToFile()
    listener.stop()
    print("\nData saved, ready to go again...\n")
    
