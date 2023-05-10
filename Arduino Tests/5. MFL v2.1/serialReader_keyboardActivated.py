import serial
import keyboard

def writeToFile(filename):
    filename = filename + ".txt"    # create full file name
    f = open(filename, 'a')         # create new file to write to

    while True:
        cc = str(ser.readline())    # gets the string from the COM port
        
        if keyboard.is_pressed('q'):  # if key 'q' is pressed 
            f.close()               # close file
            return                  # exit function

        f.write(cc[2:][:-5] + "\n") # append to file

# Set up to read serial monitor
cc = "hi"
testNum = 1
ser = serial.Serial("COM3", 115200) # <---- this line needs to be updated/correct for this to work

# Continually reading and acting 
while True:            
    str = input("Enter file name (number will be inserted automatically): ")
    writeToFile(str + "_" + str(testNum))
    testNum = testNum + 1
    print("\nData saved, ready to go again...\n")