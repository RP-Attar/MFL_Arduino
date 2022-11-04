Wireless_Comms_v1

Purpose: Transmit Hall Effect Sensor Data wirelessly, and write data to a text file

Files:
- Transmitter_v4 (.ino): to be uploaded to the Arduino Uno (reads comparison between Hall Effect Sensors A0 and A1, sends values between 0 - 2^14) [Transmitter_v3 does single shot reading, not comparison]
- Receiver_v3 (.ino): to be uploaded to Arduino Mega (receives transmitted data, writes to COM port for Serial_Reader to read)
- Serial_Reader (.py): reads serial monitor to store transmitted data in .txt file 