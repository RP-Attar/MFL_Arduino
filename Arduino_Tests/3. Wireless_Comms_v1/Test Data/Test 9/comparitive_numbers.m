clear all; close all; clc;

%% 9th round testing 

% This code is designed to analyse the very first recording of data from
% MFL v2.

%% Define all parameters for text file under consideration 
motor_speed = 380;          % step/sec - note: was a bit jolty 
motor_radius = 0.015/2;     % m - varies as string winds up
motor_step = 1.8;           % degrees - motor angle change per step
motor_lengthPerStep = (motor_step/360)*(2*3.14*motor_radius);

%% Creating list of file names
prefix = "file_";
suffixes = ["_Tbad_Bbad", "_Tgood_Bbad"];
fileNames = ["a", "b"];

for i = 1:10
    fileNames(i) = strcat(prefix, num2str(i), suffixes(1));
end
for i = 11:20
    fileNames(i) = strcat(prefix, num2str(i-10), suffixes(2));
end

%% Load data from files
%  Find lengths of files
cols = 3;                                               % number of columns in the raw datafile 
lengths = zeros(1,length(fileNames));
for i = 1:length(fileNames) 
        lengths(i) = length(table2array(readtable(fileNames(i))));
end

% extract data into one array
data = zeros(max(lengths), cols, length(fileNames));    % makes an array of zeros that is the longest data set long, with 3 columns, and the number of files amount of pages
for i = 1:length(fileNames) 
        data(1:lengths(i),:,i) = table2array(readtable(fileNames(i)));
end

%% Extract useful info from loaded data set
start = 1000;       

time_step = round(mean(data(:,2,:), [1, 2, 3]));    % average time beween each reading
time_end = time_step*(length(data)-1);              % final value of time vector
time = 0:time_step:time_end;
time = time';                                       % transposing to same dimensions as the other data
time = time/1000/1000;                              % converting to seconds


%% Cropping data
cropLength = 1800;
croppedData = zeros(cropLength+1, cols, length(fileNames));
for i = 1:length(fileNames)
        %[maxVal,maxLoc] = max(data(:,3,i));
        maxLoc = start;
        croppedData(:,:,i) = data(maxLoc:maxLoc+cropLength,:,i);
end

%% Average of each signal
averageVals = zeros(10,2);
for i = 1:length(fileNames)
        if (i < 11)
                averageVals(i,1) = mean(croppedData(:,3,i));
        elseif (i >= 11)
                averageVals(i-10,2) = mean(croppedData(:,3,i));
        end
end


