clear all; close all; clc;

%% TXT file analyser 
% This code is used to analyse the data produced by the MFL testing rig, run using Arduino.
%
%
% This code was created by Nick Lamanna as part of the MFL project for ATTAR.

%% Creating file names to iterate through 
% All 'XX.txt' files are n rows by 1 column 
prefix = ["N_x80_", "N_x80_c60_", "N_x80_c20L_", "N_x80_corr_"];
suffix = ["1.txt", "2.txt", "3.txt"];
fileNames = [];

for i = 1:length(prefix)
    for j = 1:length(suffix)
        fileName = strcat(prefix(i), suffix(j));
        fileNames = [fileNames, fileName]; 
    end
end

%% Creating matrix with all data
% Columns are [N1, N2, N3, S1, S2, S3]
formatSpec = "%d,%d";
maxColLength = 135;                         % found by looking at list of sizes
allData = zeros(maxColLength, length(fileNames));
allColLength = [];

for i = 1:length(fileNames)
    col = table2array(readtable(fileNames(i)));
    col = col(:,2);
    first = find(col,1,"first")-1;          % start of non-zero values
    last = first + 1;
    
    while col(last)>0                       % find first zero value after first non-zero value
        last = last + 1;
    end
    
    col = col(first:last);                  % creating array of non-zero data
    allData(1:length(col),i) = col;         % assigning to larger matrix
    allData(:,i) = circshift(allData(:,i), floor((maxColLength-length(col))/2)); % centering data 
    allColLength(i) = length(col);
end

%% Converting ADC value to voltage  
zeroVoltage = 3.2;
maxVoltage = 5;
allData = allData/1023*maxVoltage;

%% Creating distance vector 
distance = linspace(0, 80, maxColLength);

%% Creating plots of all data individually 
for i = 1:length(fileNames)
    subplot(4, 3, i);
    plot(distance, allData(:,i))
    title(fileNames(i))
    xlabel('distance from first magnet (mm)')
    ylabel('voltage (V)')
    axis([0 80 0 3.2])
end

%% Creating one plot with average of each type of test
averageData(:,1) = mean(allData(:,1:3),2);
averageData(:,2) = mean(allData(:,4:6),2);
averageData(:,3) = mean(allData(:,7:9),2);
averageData(:,4) = mean(allData(:,10:12),2);

figure
plot(distance, averageData(:,1), distance, averageData(:,2), distance, averageData(:,3), distance, averageData(:,4))
title('all averages superimposed')
xlabel('distance from first magnet (mm)')
ylabel('voltage (V)')
legend('no cut', 'small cut (~30%)', 'large cut (~50%)', 'corrosion');

%% Integrating each mean curve
% dx must equal recording step size, haven't written code to interpolate
sums = zeros(size(averageData(1,:)));

for i = 1:length(fileNames)/3
    for j = 1:length(distance)-1
        y1 = averageData(j, i);
        y2 = averageData(j+1, i);
        x1 = distance(j);
        x2 = distance(j+1);
        dx = x2-x1;
        y = (((y2-y1)/(x2-x1))*dx)+y1;
        sums(i) = sums(i) + (dx*y);
    end
end

sums

%% Finding FFT of mean signal 
% totalMean is full mean signal
% allData[:,1,1] is time array
for i = 1:4
    Fs = 1/0.009;               % Sampling frequency
    T = 1/Fs;                   % Sampling period
    t = distance;               % Time vector (not actually)
    L = length(t);              % Length of signal
    X = averageData(:, i) - mean(averageData(:, i)); % Original signal
    %X = highpass(X, 9, Fs);
    
    Y = fft(X);                 % FFT of original signal
    
    % Compute two sided spectrum, then one sided spectrum based on this 
    P2 = abs(Y/L);
    P1 = P2(1:L/2+1);
    P1(2:end-1) = 2*P1(2:end-1);
    allFFT(:,i) = P1;
end

% Define the freqency domain and plot 
f = Fs*(0:(L/2))/L;
figure;
plot(f, allFFT(:,1), f, allFFT(:,2), f, allFFT(:,3), f, allFFT(:,4)) 
title('Single-Sided Amplitude Spectrum of X(t)');
axis([0 20 0 0.7])
xlabel('f (Hz)');
ylabel('|P1(f)|');
legend('no cut', 'small cut (~30%)', 'large cut (~50%)', 'corrosion');
