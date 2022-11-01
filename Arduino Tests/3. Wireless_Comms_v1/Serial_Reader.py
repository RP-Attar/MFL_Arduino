import serial

def writeToFile(filename):
    filename = filename + "_bad" + ".txt"    # create full file name
    f = open(filename, 'a')         # create new file to write to
    while True:
        cc = str(ser.readline())    # gets the string from the COM port

        if "end file" in cc:        # checks to see if recording should end
            f.close()               # close file
            return                  # exit function

        f.write(cc[2:][:-5] + "\n") # append to file

# Set up to read serial monitor
cc = "hi"
ser = serial.Serial("COM3", 2000000)

# Continually reading and acting 
while True:
    cc = str(ser.readline())        # gets the string from the COM port
    print(cc + "\n")                
    if "new file" in cc:            # if it registers a "new file", call write to file function (only happens when record button on Arduino is pressed)
        writeToFile(cc[12:-5])