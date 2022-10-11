clear all; close all; clc;

%% TXT file analyser 
% This code is used to analyse the data produced by the MFL testing rig, 
% run using Arduino. Specifically, the higher baud rate data.
%
%
% This code was created by Nick Lamanna as part of the MFL project for ATTAR.

%% Getting data
formatSpec = "%d,%d";
col = table2array(readtable("extendedCut_1.txt"));
maxColLength = length(col);

dt = (col(end,1) - col(1,1))/length(col(:,1));                              % this is in microseconds ([last time - first time]/samples)

col = col(:,2);                                                             % get only ADC value from each column 
col(col<=12) = 0;                                                           % sets all values below 12 to 0
first = find(col,1,"first");                                                % find first 0
last = first;                           

while col(last) ~= 0                                                        % find first 0 after curve
    last = last + 1;
end

col = col(first-1:last);                                                    % extracts all non-zero values

roi = col;                                                                  % assigning to larger matrix
roiLength = length(roi);
%roi = circshift(roi, floor(roiLength/2)); % centering data

%% Converting ADC value to voltage  
zeroVoltage = 3.2;
maxVoltage = 5;
roi = roi/1023*maxVoltage;

%% Creating distance vector 
distance = linspace(0, 80, roiLength);

%% Creating time vector 
time = linspace(1, roiLength, roiLength)*(dt/1000/1000);                    % this is a time vector in seconds

%% Creating distance vs voltage plot
figure
plot(distance, roi);
title('large cut - 2,000,000 BAUD')
xlabel('distance from first magnet (mm)')
ylabel('voltage (V)')
axis([0 max(distance) 0 max(roi)+0.1 ])

%% Creating FFT plot
Fs = 1/(dt/1000/1000);                                                      % Sampling frequency
T = 1/Fs;                                                                   % Sampling period
t = time;                                                                   % Time vector (sec)
L = length(t);                                                              % Length of signal
X = roi - mean(roi); % Original signal
% X = highpass(X, 0.6, Fs);
Y = fft(X);%, 256);                                                         % FFT of original signal (increasing second argument (n) increases the 'curvieness' of the line, bigger n = more points in FFT) - n = 2^x (x is int)

% Compute two sided spectrum, then one sided spectrum based on this 
P2 = abs(Y/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);

% Define the freqency domain and plot 
f = Fs*(0:(L/2))/L;
figure;
plot(f, P1) 
title('Single-Sided Amplitude Spectrum of X(t)');
axis([0 50 0 0.3])
xlabel('f (Hz)');
ylabel('|P1(f)|');

%% Creating a power spectrum
% This should help isolate the waves caused by MFL from the cut
figure
hold on
pspectrum(roi,Fs,'power','FrequencyResolution',2.5)                         % shows power at each frequency point
axis([0 80 -100 10])
hold off

%% Creating a time-frequency spectrum
% https://au.mathworks.com/help/wavelet/ug/practical-introduction-to-continuous-analysis.html
figure
pspectrum(roi,Fs,'spectrogram','FrequencyResolution',7,'OverlapPercent',90)
%xlim([0 max(time)])
ylim([0 80])
% decreasing overlap percentage: widens columns that stretch up from the time axis
% increasing frequency resolution decreases time resolution