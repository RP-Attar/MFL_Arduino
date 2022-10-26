Wireless_Comms_v1

Purpose: Transmit Hall Effect Sensor Data wirelessly, and write data to a text file

Files:
- Transmitter_v1 (.ino): to be uploaded to the Arduino Uno (reads Hall Effect Sensors, sends values between 0 - 1024)
- Receiver_v1 (.ino): to be uploaded to Arduino Mega (receivees transmitted data, writes to COM port for Serial_Reader to read)
- Serial_Reader (.py): reads serial monitor to store transmitted data in .txt file 