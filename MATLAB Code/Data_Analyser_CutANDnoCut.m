clear all; close all; clc;

%% CSV file analyser 
% This code is used to analyse the data produced by the MFL testing rig, run using Raspberry Pi 4.
%
%
% This code was created by Nick Lamanna as part of the MFL project for ATTAR.

%% Creating file names to iterate through 
% All 'noCutX_data.csv' files are 240x5 
% 'noCutX_data.csv' column order is [Time, Sensor 1, Sensor 2, Sensor 3, Distance]
prefix1 = 'noCut';
suffix1 ='_data.csv';
prefix2 = '3Cuts';
suffix2 = '_data.csv';
numFiles = 10;

for i = 1:numFiles
    noCutFileName = strcat(prefix1, string(i), suffix1);
    noCutFileNames(i) = noCutFileName; 
    cutFileName = strcat(prefix2, string(i), suffix2);
    cutFileNames(i) = cutFileName; 
end

%% Create 3D matrix of all data from files 
% index1: row
% index2: column of data [Time, Sensor 1, Sensor 2, Sensor 3, Distance]
% index3: test number 
csv = readtable(noCutFileNames(i));
noCutAllData = zeros(height(csv), width(csv)+1, numFiles);
cutAllData = zeros(height(csv), width(csv)+1, numFiles);

for i = 1:numFiles
    % For no cut data
    csv = readtable(noCutFileNames(i));     % converts CSV file into a table
    array = table2array(csv);               % converts table into an array
    array(:,5) = array(:,1)*3.9;            % creates column with distance
    array(:, 6) = mean(array(:,2:4), 2);    % adds 6th column of data to array that contains average of sensors 1-3
    array(:, 6) = array(:, 6) - mean(array(:, 6), 1);
    noCutAllData(:,:,i) = array;            % stacks all test data (as an array) into a 3D matrix 

    % For 3 cut data
    csv = readtable(cutFileNames(i));       % converts CSV file into a table
    array = table2array(csv);               % converts table into an array
    array(:,5) = array(:,1)*3.9;            % creates column with distance
    array(:, 6) = mean(array(:,2:4), 2);    % adds 6th column of data to array that contains average of sensors 1-3
    array(:, 6) = array(:, 6) - mean(array(:, 6), 1);
    cutAllData(:,:,i) = array;              % stacks all test data (as an array) into a 3D matrix 
end

%% Plot Time v Sensors for all 3 sensors for each test (and average of all 3 sensors)
for i = 1:numFiles 
    subplot(2, 5, i)
    noCutTest = noCutAllData(:,:,i);
    cutTest = cutAllData(:,:,i);
    time = noCutTest(:,1);
    distance = noCutTest(:,5);
    plot(distance, noCutTest(:,6), "k", distance, cutTest(:,6), "r")
    xlabel("Distance (cm)");
    ylabel("Adjusted Voltage (V)");
    ylim([-0.04 0.04])
    title(strcat("Test ", string(i)));
    if i == 1 
        legend("No Cut", "Cut");
    end
end

%% Plot average of all averages
noCutAllMeans = zeros(height(csv), numFiles);
cutAllMeans = zeros(height(csv), numFiles);
for i = 1:numFiles
    noCutAllMeans(:,i) = noCutAllData(:,6,i);
    cutAllMeans(:,i) = cutAllData(:,6,i);
end
noCutTotalMean = mean(noCutAllMeans, 2);
cutTotalMean = mean(cutAllMeans, 2);
figure;
plot(time, noCutTotalMean, time, cutTotalMean)
xlabel("Time (s)");
ylabel("Adjusted Voltage (V)");
legend("No Cut", "Cut");
title("Time vs Mean Voltage of all sensors across all tests (cuts and no cuts)");
