#!/usr/bin/env python3
import serial
import time

ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1.0)
time.sleep(3)
ser.reset_input_buffer()
print("Serial OK")

try:
    while True:
        time.sleep(0.01)
        
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8')
            print(line)
            
        press = input("Enter a letter: ")
        if press == "a":
            print("Sending message to arduino...")
            ser.write("LEFT\n".encode('utf-8'))
except KeyboardInterrupt:
    print("Closing serial comms...")
    ser.close()
        

