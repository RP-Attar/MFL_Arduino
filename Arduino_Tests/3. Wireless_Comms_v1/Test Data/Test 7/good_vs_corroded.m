% clear all; 
close all; clc;

%% 7th round testing 

% This code is designed to analyse the very first recording of data from
% MFL v2.

%% Define all parameters for text file under consideration 
motor_speed = 380;          % step/sec - note: was a bit jolty 
motor_radius = 0.015/2;     % m - varies as string winds up
motor_step = 1.8;           % degrees - motor angle change per step
motor_lengthPerStep = (motor_step/360)*(2*3.14*motor_radius);

%% Creating list of file names
fileNames = [
        "file_1_bad.txt", ...
        "file_2_bad.txt", ...
        "file_3_bad.txt", ...
        "file_4_bad.txt", ...
        "file_5_bad.txt"
];

%% Load data from files
%  Find longest file
lengths = zeros(1,length(fileNames));
for i = 1:length(fileNames) 
        lengths(i) = length(table2array(readtable(fileNames(i))));
end

data = zeros(max(lengths), 4, length(fileNames));                           % makes an array of zeros that is the longest data set long, with 4 columns, and the number of files amount of pages
for i = 1:length(fileNames) 
        data(1:lengths(i),:,i) = table2array(readtable(fileNames(i)));
end

%% Extract useful info from loaded data set
start = 1;                                          % 360 for file 1

time_step = round(mean(data(:,2,:), [1, 2, 3]));    % average time beween each reading
time_end = time_step*(length(data)-1);              % final value of time vector
time = 0:time_step:time_end;
time = time';                                       % transposing to same dimensions as the other data
time = time/1000/1000;                              % converting to seconds


%% Time plots of signals
%  Subplots of all individual data sets
figure
cropLength = 1800;
croppedData = zeros(cropLength+1,4,length(fileNames));
for i = 1:length(fileNames)
        subplot(2,3,i)
        %[maxVal,maxLoc] = max(data(:,3,i));
        maxLoc = 1;
        croppedData(:,:,i) = data(maxLoc:maxLoc+cropLength,:,i);
        croppedData(:,3,i) = croppedData(:,3,i) - mean(croppedData(:,3,i));             % centering around 0
        croppedData(:,4,i) = (croppedData(:,4,i) - mean(croppedData(:,4,i)))*-1;        % centering around 0 and flipping
        xlabel("time (s)")
        ylabel("raw ADC reading (0-32,768)")
        title("ADC value vs time")
        plot(time(1:cropLength+1), croppedData(:,3,i), time(1:cropLength+1), croppedData(:,4,i))
        legend("hall 1 - top", "hall 2 - bottom")
end

% Average of all signals
hall_1 = mean(croppedData(:,3,:), [2,3]);
hall_2 = mean(croppedData(:,4,:), [2,3]);

figure
xlabel("time (s)")
ylabel("raw ADC reading (0-32,768)")
title("ADC value vs time")
plot(time(1:cropLength+1), hall_1, time(1:cropLength+1), hall_2)
legend("hall 1 - top", "hall 2 - bottom")

%% Finding FFT of mean signal 
Fs = 1/(time_step/1000000); % Sampling frequency
T = 1/Fs;                   % Sampling period
L = length(time);           % Length of signal
t = (0:L-1)*T;              % Time vector

Y1 = fft(hall_1);               % FFT 
Y2 = fft(hall_2);               % FFT 

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
%axis([0 20 0 10])
xlabel('f (Hz)');
ylabel('|P1(f)|');

figure;
plot(f, P12);
title("Single-Sided Amplitude Spectrum of Hall 2");
%axis([0 20 0 4])
xlabel('f (Hz)');
ylabel('|P1(f)|');

%% Power spectrum
figure
pspectrum(hall_1,Fs,'spectrogram','FrequencyResolution',2,'OverlapPercent',90)
figure
pspectrum(hall_2,Fs,'spectrogram','FrequencyResolution',2,'OverlapPercent',90)
