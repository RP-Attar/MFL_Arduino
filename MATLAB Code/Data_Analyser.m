clear all; close all; clc;

%% CSV file analyser 
% This code is used to analyse the data produced by the MFL testing rig, run using Raspberry Pi 4.
%
%
% This code was created by Nick Lamanna as part of the MFL project for ATTAR.

%% Creating file names to iterate through 
% All 'noCutX_data.csv' files are 240x5 
% 'noCutX_data.csv' column order is [Time, Sensor 1, Sensor 2, Sensor 3, Distance]
prefix = 'noCut';
suffix ='_data.csv';
numFiles = 10;

for i = 1:numFiles
    fileName = strcat(prefix, string(i), suffix);
    fileNames(i) = fileName; 
end

%% Create 3D matrix of all data from files 
% index1: row
% index2: column of data [Time, Sensor 1, Sensor 2, Sensor 3, Distance]
% index3: test number 
csv = readtable(fileNames(i));
allData = zeros(height(csv), width(csv)+1, numFiles);

for i = 1:numFiles
    csv = readtable(fileNames(i));          % converts CSV file into a table
    array = table2array(csv);               % converts table into an array
    array(:, 6) = mean(array(:,2:4), 2);    % adds 6th column of data to array that contains average of sensors 1-3
    array(:, 6) = array(:, 6) - mean(array(:, 6), 1); % removes average of entire average signal to bring near 0
    allData(:,:,i) = array;                 % stacks all test data (as an array) into a 3D matrix 
end

%% Plot Time v Sensors for all 3 sensors for each test (and average of all 3 sensors)
for i = 1:numFiles 
    subplot(2, 5, i)
    test = allData(:,:,i);
    time = test(:,1);
    plot(time, test(:,2), "r", time, test(:,3), "g", time, test(:,4), "b", time, test(:,6), "k")
    xlabel("Time (s)");
    ylabel("Adjusted Voltage (V)");
    title(strcat("Test ", string(i)));
    if i == 1 
        legend("sensor 1", "sensor 2", "sensor 3", "mean of 1 to 3");
    end
end

%% Plot average of all averages
allMeans = zeros(height(csv), numFiles);
for i = 1:numFiles
    allMeans(:,i) = allData(:,6,i);
end
totalMean = mean(allMeans, 2);
figure;
plot(time, totalMean)
xlabel("Time (s)");
ylabel("Adjusted Voltage (V)");
title("Time vs Mean Voltage of all sensors across all tests");

%% Finding FFT of mean signal 
% totalMean is full mean signal
% allData[:,1,1] is time array
Fs = 1000;                  % Sampling frequency
T = 1/Fs;                   % Sampling period
t = allData(:,1,1);         % Time vector
L = length(t);              % Length of signal
X = totalMean;              % Original signal

Y = fft(X);                 % FFT of original signal

% Compute two sided spectrum, then one sided spectrum based on this 
P2 = abs(Y/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);

% Define the freqency domain and plot 
f = Fs*(0:(L/2))/L;
figure;
plot(f,P1) 
title('Single-Sided Amplitude Spectrum of X(t)');
xlabel('f (Hz)');
ylabel('|P1(f)|');
