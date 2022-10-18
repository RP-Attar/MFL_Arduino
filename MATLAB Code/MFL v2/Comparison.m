%% MFL Simulation

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
r2_specimen = 5/2;          % New 'corroded' specimen radius

%% Model and show sensor and specimen when centred 
[x_sensor, y_sensor] = circle(0, 0, r1_sensor);
[x_specimen, y_specimen] = circle(0, 0, r2_specimen);

figure
hold on
axis equal
xlim([-3.5 3.5])
ylim([-3.5 3.5])
plot(x_sensor, y_sensor, x_specimen, y_specimen)
title('Steel conductor specimen inside collector ring')
xlabel('x (mm)')
ylabel('y (mm)')
legend('Collector Ring', 'Specimen')
hold off

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

Hy_centred = zeros(1, n);

for i = 1:n
    [Hx_H0, Hy_H0] = tempSim(h, two_b, y(i), -20, 20);
    Hy_centred(i) = max(Hy_H0);
end

%% Model and show sensor and specimen when not centred 
[x_sensor, y_sensor] = circle(0, 0, r1_sensor);
[x_specimen, y_specimen] = circle(.5, 0, r2_specimen);

figure
hold on
axis equal
xlim([-3.5 3.5])
ylim([-3.5 3.5])
plot(x_sensor, y_sensor, x_specimen, y_specimen)
title('Steel conductor specimen inside collector ring')
xlabel('x (mm)')
ylabel('y (mm)')
legend('Collector Ring', 'Specimen')
hold off
 
%% Calculating lift off for centered system
n = length(x_specimen);
y = zeros(1, n);

for i = 1:n
    y(i) = sqrt(abs(x_sensor(i)^2 + y_sensor(i)^2)) - sqrt(abs(x_specimen(i)^2 + y_specimen(i)^2));
    y(i) = round(y(i), 4);
end

%% Calculating measured MFL at each point around conductor for centered system
h = r1_specimen - r2_specimen;          % Depth of cut (mm)
two_b = 2;                              % Length of cut (mm)

Hy_offcentre = zeros(1, n);

for i = 1:n
    [Hx_H0, Hy_H0] = tempSim(h, two_b, y(i), -20, 20);
    Hy_offcentre(i) = max(Hy_H0);
end

%% Plot measured MFL for centred and non-centered systems
figure
hold on
x = 0:length(Hy_offcentre)-1;
plot(x, Hy_centred, x, Hy_offcentre)
title('MFL for centered vs non-centered')
xlabel('x')
ylabel('MFL (fraction of total field detected)')
legend('Centered', 'Non-Centered')
hold off

fprintf("Sum centred: %f\n", sum(Hy_centred))
fprintf("Sum non-centred: %f\n", sum(Hy_offcentre))