15/11/2022 - 10:41
Receiver_v1 and Transmitter_v2
Two way communication works
Transmitter sends random numbers as struct every 1000ms
Receiver randomly sends a "hello" whenever the random number meets a certain criteria
Transmitter can receive the random signal from the receiver when it is randomly sent

15/11/2022 - 15:56
Receiver_v1 and Transmitter_v2
Two way communication works
Transmitter sends ADC conversion data struct and recevies instructional struct 
Receiver sends instructional struct and receives ADC conversion data struct
Transmitter can recognise and act on the various instructions it can be sent
Receiver needs an IR or Bluetooth controller attached to it so that we can tell it what instructions to send