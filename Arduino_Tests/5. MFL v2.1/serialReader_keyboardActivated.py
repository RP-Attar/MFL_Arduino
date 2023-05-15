import serial
from pynput import keyboard
from time import sleep

# Set up to read serial monitor
cc = "hi"
testNum = 1
flag = 0
ser = serial.Serial("/dev/ttyACM0", 115200) # <---- this line needs to be updated/correct for this to work

def on_press(key):
    global flag
    #print(key)
    
    with keyboard.Events() as events:
        for event in events:
            if event.key == keyboard.Key.esc:                
                flag = 1
                #print("flag changed")

def writeToFile(filename):
    global flag
    filename = filename + ".txt"    # create full file name
    f = open(filename, 'a')         # create new file to write to

    while True:
        cc = str(ser.readline())    # gets the string from the COM port
        print(cc[2:][:-5] + "\n")

        if flag == 1:               # if key 'q' is pressed 
            f.close()               # close file
            flag = 0                # reset flag
            return                  # exit function

        f.write(cc[2:][:-5] + "\n") # append to file

listener = keyboard.Listener(on_press=on_press)

# Continually reading and acting 
while True:            
    fileName = input("Enter file name (number will be inserted automatically): ")
    listener.start()
    writeToFile(fileName + "_" + str(testNum))
    listener.stop()
    testNum = testNum + 1
    print("\nData saved, ready to go again...\n")
    