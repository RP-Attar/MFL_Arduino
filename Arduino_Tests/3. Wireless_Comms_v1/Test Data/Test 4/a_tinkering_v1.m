clear all; close all; clc;

%% 2nd round testing 

% This code is designed to analyse the very first recording of data from
% MFL v2.

%% Define all parameters for text file under consideration 
motor_speed = 80;           % RPM - note: was a bit jolty 
motor_radius = 0.015/2;     % m - varies as string winds up
motor_step = 1.8;           % degrees - motor angle change per step
motor_lengthPerStep = (motor_step/360)*(2*3.14*motor_radius);

%% Load data from file being looked at
file_path = "C:\Users\Nicholas Lamanna\Desktop\MFL Code\Arduino Tests\3. Wireless_Comms_v1\Test Data\Test 4\file_3.txt";
data = table2array(readtable(file_path));

%% Extract useful info from loaded data set
start = 350;                                        % 453 for \Test 4\file_2.txt
                                                    % 350 for \Test 4\file_3.txt

time_step = round(mean(data(start,2)));             % average time beween each reading

Fpass = 4;
Fs = 1/(time_step/1000000);                         % Sampling frequency
hall_1 = data(start:end,3);% - mean(data(start:end,3));  
%hall_1 = highpass(hall_1,Fpass,Fs); 
hall_2 = data(start:end,4);% - mean(data(start:end,4));
%hall_2 = highpass(hall_2,Fpass,Fs); 

time_end = time_step*(length(hall_1)-1);            % final value of time vector
time = 0:time_step:time_end;
time = time';                                       % transposing to same dimensions as the other data

data(start-1, 1) = 0;
for i = start:length(data)
    data(i,1) = data(i-1,1) + (data(i,1)*motor_lengthPerStep);
end
distance = data(start:end,1);

%% Time plots of signals
figure
xlabel("time (us)")
ylabel("raw ADC reading (0-1024)")
title("ADC value vs time")
plot(time, hall_1, time, hall_2)
legend("hall 1 - collector ring", "hall 2 - normal sensor")

%% Finding FFT of mean signal 
Fs = 1/(time_step/1000000); % Sampling frequency
T = 1/Fs;                   % Sampling period
L = length(time);           % Length of signal
t = (0:L-1)*T;              % Time vector

X1 = hall_1; %- mean(hall_1);
X2 = hall_2; %- mean(hall_2);

Y1 = fft(X1);               % FFT 
Y2 = fft(X2);               % FFT 

% Compute two sided spectrum, then one sided spectrum based on this 
P21 = abs(Y1/L);
P22 = abs(Y2/L);
P11 = P21(1:L/2+1);
P12 = P22(1:L/2+1);
P11(2:end-1) = 2*P11(2:end-1);
P12(2:end-1) = 2*P12(2:end-1);

% Define the freqency domain and plot 
f = Fs*(0:(L/2))/L;

figure;
plot(f, P11);
title("Single-Sided Amplitude Spectrum of Hall 1");
% axis([0 20 0 0.7])
xlabel('f (Hz)');
ylabel('|P1(f)|');

figure;
plot(f, P12);
title("Single-Sided Amplitude Spectrum of Hall 2");
% axis([0 20 0 0.7])
xlabel('f (Hz)');
ylabel('|P1(f)|');

%% Power spectrum
figure
pspectrum(hall_1,Fs,'spectrogram','FrequencyResolution',2,'OverlapPercent',90)
figure
pspectrum(hall_2,Fs,'spectrogram','FrequencyResolution',2,'OverlapPercent',90)
