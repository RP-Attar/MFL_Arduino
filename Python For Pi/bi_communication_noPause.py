#!/usr/bin/env python3

# Import all the modules we need
import serial
import time
from pynput.keyboard import Listener, Key


# Detects key press(es) and sends it to Arduino
def on_press(key):
    if hasattr(key, 'char'):
        ser.write(key.char.encode('utf-8'))        

# Set up serial comms
ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1.0)
time.sleep(3)
ser.reset_input_buffer()
print("Serial OK")

# Set up listener to detect key presses
# This acts as an interrupt, interrupting the code momentarily
listener = Listener(on_press=on_press)
listener.start()
    
# Body of program     
try:
    while True:
        time.sleep(0.01)     
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').strip()
            print(line)
except KeyboardInterrupt: # triggered by <ctrl>+c
    print("Closing serial comms...")
    ser.close()
        