clear all; close all; clc;

%% TXT file analyser 
% This code is used to analyse the data produced by the MFL testing rig, 
% run using Arduino. 
%
% NOTE: it adjusts the time vector for each test, maybe don't do this?
%
% This code was created by Nick Lamanna as part of the MFL project for ATTAR.

%% Creating file names to iterate through 
% All 'XX.txt' files are n rows by 2 columns (format "num,num") 
prefix = ["noCut_x80_", "smallCut_x80_", "largeCut_x80_"];
suffix = linspace(1, 50, 50);
fileNames = [];

for i = 1:length(prefix)
    for j = 1:length(suffix)
        fileName = strcat(prefix(i), int2str(suffix(j)));
        fileNames = [fileNames, fileName]; 
    end
end

%% Creating matrix with all data
% Columns are [noCut 1-50, smallCut 1-50, largeCut 1-50]
formatSpec = "%d,%d";
maxColLength = 166;                         % found 313 by using "longestLengthFinder.m", longest stretch of non-zeros is 164
allData = zeros(maxColLength, length(fileNames));
dt = zeros(1, length(fileNames));
nonZeroLength = [];

for i = 1:length(fileNames)
    col = table2array(readtable(fileNames(i)));
    
    dt(i) = (col(end,1) - col(1,1))/length(col(:,1));

    col = col(:,2);                         % get only ADC value from each column 
    col(col<5) = 0;                         % sets all values below 5 to 0
    col = col(col>0);                       % extracts all non-zero values
    
    allData(1:length(col),i) = col;         % assigning to larger matrix
    allData(:,i) = circshift(allData(:,i), floor((maxColLength-length(col))/2)); % centering data 
end

%% Converting ADC value to voltage  
zeroVoltage = 3.2;
maxVoltage = 5;
allData = allData/1023*maxVoltage;

%% Creating distance vector 
distance = linspace(0, 80, maxColLength);

%% Creating time vector 
meandt = floor(mean(dt))/1000;
time = linspace(1, maxColLength, maxColLength)*meandt;

%% Creating plots of all data individually 
% looking at all plots, we can see some are broken, exclude?
titleNum = 1;
for i = 1:length(prefix)
    figure
    for j = 1:50
        subplot(5, 10, j);
        plot(distance, allData(:,j+(i-1)*50))
        title(fileNames(titleNum))
        xlabel('d along x (mm)')
        ylabel('voltage (V)')
        axis([0 80 0 3])
        titleNum = titleNum + 1;
    end
end

%% Integrating each signal
integral = zeros(1,length(fileNames));

for i = 1:length(fileNames)
    for j = 1:length(allData(:,i))-1
        integral(i) = integral(i) + ((allData(j,i) + allData(j+1,i))/2*meandt); % finding integral of each curve by summing of trapeziums
    end
end

%% Finding details for each condition and creating mask to exclude outliers
conditions = length(prefix);
mu = zeros(1,conditions);                               % mean    
stdev = zeros(1,conditions);                            % standard deviation
iqrange = zeros(1,conditions);                          % inter-quartile range
outliers = zeros(50, conditions);                       % 50x3 matrix, [noCut mask smallCut mask largeCut mask]

for i = 1:conditions 
    set = integral((((i-1)*50)+1):(i*50));              % extracts set of integrals relevant to condition
    mu(i) = mean(set);                                  % calculates mean of the set
    stdev(i) = std(set);                                % calculates standard deviation of the set
    pd = makedist('Normal', 'mu', mu(i), 'sigma', stdev(i)); % creates normal distribution of the set
    iqrange(i) = iqr(pd);                               % finds IQR of the normal distribution
    q1 = mu(i) - (1.5*iqrange(i));                      % lower bound of outliers
    q3 = mu(i) + (1.5*iqrange(i));                      % upper bound of outliers
    
    outliers(:,i) = (set>q1)&(set<q3);                  % creates matrix where 1 passes both conditions and 0 fails at least one condition
end
outliers(23,3) = 0;                                     % remove 23rd test for large cut, based on visual inspection

%% Creating one plot with average of each type of test
for i = 1:conditions
    condData = allData(:,(((i-1)*50)+1):(i*50));                    % extracts data relevant only to this condition
    outlierMask = repmat(outliers(:,i)',166,1);                     % transposes column into row, then stacks it 166 times 
    outliersExcuded = condData .* outlierMask;                      % element by element multiplication to remove outlier columns
    outliersExcuded = outliersExcuded(:,any(outliersExcuded));      % removes columns that are all zeros
    averageData(:,i) = mean(outliersExcuded,2);                     % calculates average signal from the signals that were not outliers
end

figure
plot(distance, averageData(:,1), distance, averageData(:,2), distance, averageData(:,3))
title('all averages superimposed')
xlabel('distance from first magnet (mm)')
ylabel('voltage (V)')
legend('no cut', 'small cut (~30%)', 'large cut (~50%)');

%% Finding FFT of mean signal 
% averageData is a [166x3] where the columns are [noCut, smallCut, largeCut]
% time is time array
for i = 1:conditions
    Fs = 1/meandt;              % Sampling frequency
    T = 1/Fs;                   % Sampling period
    t = time/1000;              % Time vector (sec)
    L = length(t);              % Length of signal
    X = averageData(:, i) - mean(averageData(:, i)); % Original signal
    % X = highpass(X, 0.6, Fs);
    Y = fft(X,512);            % FFT of original signal (increasing second argument (n) increases the 'curvieness' of the line, bigger n = more points in FFT) - n = 2^x (x is int)
    
    % Compute two sided spectrum, then one sided spectrum based on this 
    P2 = abs(Y/L);
    P1 = P2(1:L/2+1);
    P1(2:end-1) = 2*P1(2:end-1);
    allFFT(:,i) = P1;
end

% Define the freqency domain and plot 
f = Fs*(0:(L/2))/L;
figure;
plot(f, allFFT(:,1), f, allFFT(:,2), f, allFFT(:,3)) 
title('Single-Sided Amplitude Spectrum of X(t)');
axis([0 20 0 1.2])
xlabel('f (Hz)');
ylabel('|P1(f)|');
legend('no cut', 'small cut (~30%)', 'large cut (~50%)');

%% Creating a power spectrum
% This should help isolate the waves caused by MFL from the cut
figure
hold on
for i = 1:conditions
    pspectrum(averageData(:,i),Fs,'power','FrequencyResolution',2)      % shows power at each frequency point
end
legend('no cut', 'small cut', 'large cut')
hold off

%% Creating a time-frequency spectrum
% https://au.mathworks.com/help/wavelet/ug/practical-introduction-to-continuous-analysis.html
for i = 1:conditions
    figure
    pspectrum(averageData(:,i),Fs,'spectrogram','FrequencyResolution',10,'OverlapPercent',90) 
    % decreasing overlap percentage: widens columns that stretch up from the time axis
    % increasing frequency resolution decreases time resolution
end
