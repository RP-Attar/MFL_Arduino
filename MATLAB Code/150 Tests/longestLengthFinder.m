clear all; close all; clc;

%% Longest Length Finder 
% This code is used to find the longest data file from the data produced by the MFL testing rig, run using Arduino.
%
%
% This code was created by Nick Lamanna as part of the MFL project for ATTAR.

%% Creating file names to iterate through 
% All 'XX.txt' files are n rows by 1 column 
prefix = ["r2_noCut_x80_", "r2_smallCut_x80_", "r2_largeCut_x80_"];
suffix = linspace(1, 30, 30);
fileNames = [];

for i = 1:length(prefix)
    for j = 1:length(suffix)
        fileName = strcat(prefix(i), int2str(suffix(j)));
        fileNames = [fileNames, fileName]; 
    end
end

%% Creating matrix of all column lengths 
formatSpec = "%d,%d";
allColLengths = [];

for i = 1:length(fileNames)
    col = table2array(readtable(fileNames(i)));
    col = col(:,2);
    allColLengths(i) = length(col);
end

max(allColLengths)