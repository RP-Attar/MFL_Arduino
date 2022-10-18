%% MFL Simulation v2

% This code was created by Nicholas Lamanna on behalf of ATTAR to
% simulate the magnetic flux leakage. It compares various cross sectional
% loss leakage fields to eachother, as well as their position within the
% collector ring.

% Imagine the 'cut' we are simulating is like a stripped wire. eg/ We are
% cutting a radius of r1 down to r2, where the radial loss is denoted h. 
% The length of the wire stripped is denoted two_b. The lift off of the
% sensor from the new r2 is denoted y.

clear all; close all; clc;

%% Define the sensor dimensions 
r1_sensor = 6.5/2;

%% Define the specimen dimensions
r1_specimen = 6/2;          % Original radius of conductor
r2_specimen = 5.4/2;          % New 'corroded' specimen radius

%% Define the cut parameters 
h = r1_specimen - r2_specimen;          % Depth of cut (mm)
two_b = 12;                              % Length of cut (mm)

%% Define the range of values off centre to be considered 
off = [0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7];

%% Main body 
figure
hold on
axis equal
xlim([-3.5 3.5])
ylim([-3.5 3.5])
title('Steel conductor specimen inside collector ring')
xlabel('x (mm)')
ylabel('y (mm)')
Legend = cell(length(off), 1);

Hy = zeros(101, length(off));       % x dim - max MFL reading for a given lift off, y dim - column corresponding to offset: eg/ Hy(12,2) is the 12th lift off measurement for the 2nd offset value

for j = 1:length(off)
    %% Model and show sensor and specimen
    [x_sensor, y_sensor] = circle(0, 0, r1_sensor);
    [x_specimen, y_specimen] = circle(off(j), 0, r2_specimen);
    
    plot(x_sensor, y_sensor, x_specimen, y_specimen)
    Legend{j} = strcat(num2str(off(j)), ' mm offset');

    %% Calculating lift off for centered system
    n = length(x_specimen);
    y = zeros(1, n);
    
    for i = 1:n
        y(i) = sqrt(abs(x_sensor(i)^2 + y_sensor(i)^2)) - sqrt(abs(x_specimen(i)^2 + y_specimen(i)^2));
        y(i) = round(y(i), 4);
    end
    
    %% Calculating measured MFL at each point around conductor for centered system
    % I think we only care about Hy here (the magnetic field perpendicular to
    % the surface of the defect... we shall see!
    h = r1_specimen - r2_specimen;          % Depth of cut (mm)
    two_b = 2;                              % Length of cut (mm)
    
    for i = 1:n
        [Hx_H0, Hy_H0] = tempSim(h, two_b, y(i), -20, 20);
        Hy(i, j) = max(Hy_H0);
    end
end
legend(Legend)
hold off

%% Plot measured MFL for centred and non-centered systems
figure
hold on
x = 0:length(Hy)-1;
title('MFL for centered vs non-centered')
xlabel('x')
ylabel('MFL (fraction of total field detected)')
Legend = cell(length(off), 1);
for i = 1:length(off)
    plot(x, Hy(:,i));
    Legend{i} = strcat(num2str(off(i)), ' mm offset');
end
legend(Legend)
hold off

%% Plot of integral of curve against offset
figure
hold on
title('Sum of MFL curve vs Offset from centred')
xlabel('offset (mm)')
ylabel('integral of MFL around collector ring')
sums = sum(Hy,1);
plot(off, sums)
hold off
