clear all; close all; clc;

%% TXT file analyser 
% This code is used to analyse the data produced by the MFL testing rig, 
% run using Arduino. 
% This file focuses on integrating 4mm sections and comparing them across
% conditions.
%
% NOTE: it adjusts the time vector for each test, maybe don't do this?
% NOTE: no outlies are removed in this file
%
% This code was created by Nick Lamanna as part of the MFL project for ATTAR.

%% Creating file names to iterate through 
% All 'XX.txt' files are n rows by 2 columns (format "num,num") 
prefix = ["r2_noCut_x80_", "r2_smallCut_x80_", "r2_largeCut_x80_"];
suffix = linspace(1, 30, 30);
conditions = length(prefix);
numTests = length(suffix);
fileNames = [];

for i = 1:conditions
    for j = 1:numTests
        fileName = strcat(prefix(i), int2str(suffix(j)));
        fileNames = [fileNames, fileName]; 
    end
end

%% Creating matrix with all data
% Columns are [noCut 1-30, smallCut 1-30, largeCut 1-30]
formatSpec = "%d,%d";
maxColLength = 200;                                                         % found 482 by using "longestLengthFinder.m", longest stretch of non-zeros is <200
allData = zeros(maxColLength, length(fileNames));
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

%% Calculating area integral for each area section 
% rearrange allData matrix
condition1 = allData(:,1:30);
condition2 = allData(:,31:60);
condition3 = allData(:,61:90);

allData = [];
allData(:,:,1) = condition1;
allData(:,:,2) = condition2;
allData(:,:,3) = condition3;

% isolate region of interest from data, extracted data from the same x value (d>22mm & d<54mm)
roiMask = (distance>22) & (distance<54);
distanceOfInterest = distance(roiMask);
distanceOfInterest = distanceOfInterest(any(distanceOfInterest),:);         % removes columns that are all zeros
start = find(roiMask,1);
finish = find(roiMask,1,'last');
regionOfInterest = allData(start:finish,:,:);                               % removes rows that are all zeros
roiLength = length(distanceOfInterest);

% figure
% plot(distanceOfInterest, mean(regionOfInterest(:,:,1),2), distanceOfInterest, mean(regionOfInterest(:,:,2),2), distanceOfInterest, mean(regionOfInterest(:,:,3),2))
% title('all averages superimposed')
% xlabel('distance from first magnet (mm)')
% ylabel('voltage (V)')
% legend('no cut', 'small cut (~30%)', 'large cut (~50%)');

% calculate all the small section integrals
sections = length(regionOfInterest)-1;
smallSectionIntegral = zeros(sections, numTests, conditions);

for i = 1:conditions
    for j = 1:numTests
        for k = 1:sections-1
            smallSectionIntegral(k,j,i) = (regionOfInterest(k,j,i)+regionOfInterest(k+1,j,i))/2*meandt;
        end
    end
end

%% Checking if each row for each page of smallSectionIntegral belongs to a normal distribution
% https://au.mathworks.com/help/stats/adtest.html
adTestResults = zeros(roiLength,conditions);
adTestResults = num2cell(adTestResults);

for i = 1:roiLength-1
    for j = 1:conditions
        [h,p] = adtest(smallSectionIntegral(i,:,j));
        cell = {h,p};
        adTestResults{i,j} = cell;
%         figure
%         bar(smallSectionIntegral(i,:,j))
%         xlabel('test')
%         ylabel('voltage (V)')
%         title("row: " + num2str(i) + ", condition: " + num2str(j))
    end
end

% Performing one way ANOVA on data (one ANOVA test per row,  including all columns and pages) 
anTestResults = zeros(roiLength);
anTestResults = num2cell(adTestResults);

for i = 1:roiLength-1
    row1 = smallSectionIntegral(i,:,1);
    row2 = smallSectionIntegral(i,:,2);
    row3 = smallSectionIntegral(i,:,3);

    data = [row1', row2', row3'];

    [p,tbl,stats] = anova1(data);
    cell = {p,tbl,stats};
    anTestResults{i,j} = cell;
    if (p<=0.005) 
        figure 
        results = multcompare(stats);
    end
end




