clear all; close all; clc;

%% TXT file analyser 
% This code is used to analyse the data produced by the MFL testing rig,
% run using Arduino. Specifically, it finds outliers by integrating the
% signal of each test for each condition.
%
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
period = floor(mean(dt));
time = linspace(1, maxColLength, maxColLength)*period;

%% Integrating each signal
integral = zeros(1,length(fileNames));

for i = 1:length(fileNames)
    for j = 1:length(allData(:,i))-1
        integral(i) = integral(i) + round(((allData(j,i) + allData(j+1,i))/2*period)); % finding integral by sum of trapeziums
    end
end

%% Finding details for each condition
conditions = length(prefix);
mu = zeros(1,conditions);
stdev = zeros(1,conditions);
iqrange = zeros(1,conditions);
integral_noOutliers = zeros(conditions, 50);

for i = 1:conditions 
    set = integral((((i-1)*50)+1):(i*50));
    mu(i) = mean(set);
    stdev(i) = std(set);
    pd = makedist('Normal', 'mu', mu(i), 'sigma', stdev(i));
    iqrange(i) = iqr(pd); 
    q1 = mu(i) - (1.5*iqrange(i));
    q3 = mu(i) + (1.5*iqrange(i));
    
    set = set(set>q1);
    set = set(set<q3);
    integral_noOutliers(i,1:length(set)) = set;
end




