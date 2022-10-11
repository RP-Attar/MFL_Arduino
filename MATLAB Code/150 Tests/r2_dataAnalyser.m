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
prefix = ["r2_noCut_x80_", "r2_smallCut_x80_", "r2_largeCut_x80_"];
suffix = linspace(1, 30, 30);
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
maxColLength = 200;                         % found 482 by using "longestLengthFinder.m", longest stretch of non-zeros is <200
allData = zeros(maxColLength, length(fileNames));
colLengths = zeros(length(fileNames));
dt = zeros(1, length(fileNames));
nonZeroLength = [];

for i = 1:length(fileNames) % perhaps don't set all values less than 9 to zero and just look at raw data - will require different extraction/processing here
    col = table2array(readtable(fileNames(i)));
    
    dt(i) = (col(end,1) - col(1,1))/length(col(:,1));

    col = col(:,2);                         % get only ADC value from each column 
    col(col<9) = 0;                         % sets all values below 9 to 0
    first = find(col,1,"first");            % find first 0
    last = first;                           
    
    while col(last) ~= 0                    % find first 0 after curve
        last = last + 1;
    end

    col = col(first-1:last);                % extracts all non-zero values
    
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
    for j = 1:length(suffix)
        subplot(3, 10, j);
        plot(distance, allData(:,j+(i-1)*length(suffix)))
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
tests = length(suffix);
mu = zeros(1,conditions);                               % mean    
stdev = zeros(1,conditions);                            % standard deviation
iqrange = zeros(1,conditions);                          % inter-quartile range
outliers = zeros(length(suffix), conditions);           % nx3 matrix, [noCutMask smallCutMask largeCutMask]

for i = 1:conditions 
    set = integral((((i-1)*length(suffix))+1):(i*length(suffix))); % extracts set of integrals relevant to condition
    mu(i) = mean(set);                                  % calculates mean of the set
    stdev(i) = std(set);                                % calculates standard deviation of the set
    pd = makedist('Normal', 'mu', mu(i), 'sigma', stdev(i)); % creates normal distribution of the set
    iqrange(i) = iqr(pd);                               % finds IQR of the normal distribution
    q1 = mu(i) - (1.5*iqrange(i));                      % lower bound of outliers
    q3 = mu(i) + (1.5*iqrange(i));                      % upper bound of outliers
    
    outliers(:,i) = (set>q1)&(set<q3);                  % creates matrix where 1 passes both conditions and 0 fails at least one condition
end
%outliers(23,3) = 0;                                     % remove 23rd test for large cut, based on visual inspection

%% Creating one plot with average of each type of test
for i = 1:conditions
    condData = allData(:,(((i-1)*length(suffix))+1):(i*length(suffix))); % extracts data relevant only to this condition
    outlierMask = repmat(outliers(:,i)',maxColLength,1);            % transposes column into row, then stacks it 166 times 
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
% averageData is a [200x3] where the columns are [noCut, smallCut, largeCut]
% time is time array
for i = 1:conditions
    Fs = 1/meandt;              % Sampling frequency
    T = 1/Fs;                   % Sampling period
    t = time/1000;              % Time vector (sec)
    L = length(t);              % Length of signal
    X = averageData(:, i) - mean(averageData(:, i)); % Original signal
    % X = highpass(X, 0.6, Fs);
    Y = fft(X, 512);            % FFT of original signal (increasing second argument (n) increases the 'curvieness' of the line, bigger n = more points in FFT) - n = 2^x (x is int)
    
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
    pspectrum(averageData(:,i),Fs,'spectrogram','FrequencyResolution',3,'OverlapPercent',90) 
    % decreasing overlap percentage: widens columns that stretch up from the time axis
    % increasing frequency resolution decreases time resolution
end

% %% Check for normality 
% integral_cond1 = integral(1:30);
% integral_cond2 = integral(31:60);
% integral_cond3 = integral(61:90);
% integral = [integral_cond1' integral_cond2' integral_cond3'];
% 
% adTestResults = zeros(conditions);
% adTestResults = num2cell(adTestResults);
% 
% for i = 1:conditions
%     [h,p] = adtest(integral(:,i));
%     cell = {h,p};
%     adTestResults{i,j} = cell;
%     if (h == 1) 
%         fprintf("condition " + num2str(i) + " is a normal distribution.\n")
%     end
% end
% 
% %% Performing one way ANOVA on data 
% [p,tbl,stats] = anova1(integral);

%% Calculating Spearman's Correlation Coefficient (should not do this)
% https://geographyfieldwork.com/SpearmansRank.htm#:~:text=Spearman's%20Rank%20correlation%20coefficient%20is%20a%20technique%20which%20can%20be,between%201%20and%20minus%201.
dpow2 = [0 0];
Rs = [0 0];
diff = zeros(length(averageData));

for i = 2:3
    for j = 1:length(averageData)
        diff(j) = ((averageData(j,1)-averageData(j,i))^2);
        dpow2(i-1) = dpow2(i-1) + ((averageData(j,1)-averageData(j,i))^2);
    end
    Rs(i-1) = 1 - ((6*dpow2(i-1))/(length(averageData)^3-length(averageData)));
end

figure
plot(1:length(diff),diff)
title('Difference between the no cut and cut signal');
xlabel('Data Point');
ylabel('Difference (V)');
