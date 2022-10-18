%% This code was created by Nicholas Lamanna on behalf of ATTAR to
%% simulate the magnetic flux leakage from cuts of various dimensions.

clear all; close all; clc;

%% Define defect parameters 
h = 1;              % defect depth (mm)
b = 0.5;            % defect width (mm) (b is actually 2b - so divide actual cut size by 2)
y = 1;              % lift off (mm)

%% Define region of interest 
x = linspace(-20, 20, 1000);
w = linspace(0, 10, 1000);

%% Magnetic Leakage Field Equations (temporal)
% Hx - x component of magnetic leakage field
% Hy - y component of magnetic leakage field
% H0 - magnetic field strength within the gap

% Hx/H0 terms
t1 = h.*(x+b);
t2 = (x+b).^2;
t3 = y*(y+h);
t4 = h.*(x-b);
t5 = (x-b).^2;
t6 = t3;

Hx_H0 = (1/pi).*( atan((t1./(t2+t3))) - atan((t4./(t5+t6))) );

% Hy/H0 terms
t1 = (x+b).^2;
t2 = (y+h)^2;
t3 = (x-b).^2;
t4 = y^2;
t5 = (x-b).^2;
t6 = t2;
t7 = (x+b).^2;
t8 = t4;

Hy_H0 = (1/(2*pi)).*log(((t1+t2).*(t3+t4)) ./ ((t5+t6).*(t7+t8)));

% Plot Hx/H0 and Hy/H0
hold on
plot(x, Hx_H0)
plot(x, Hy_H0)
title('Modelled MFL signal of 2D defect in infinite plane')
legend('Hx/H0', 'Hy/H0')
xlabel('Distance from center of defect (mm)')
ylabel('Fraction of H0 detectable')
hold off

%% Magnetic Leakage Field Equations (spacial)
% Hx - x component of magnetic leakage field
% Hy - y component of magnetic leakage field
% H0 - magnetic field strength within the gap
% w - spacial frequency (m^-1)

% Terms
t1 = 2.*b.*exp(-y.*w);
t2 = 1 - exp(-h.*w);
t3 = sin(b.*w)./(w.*b);

Hx_H0 = abs(t1.*t2.*t3);
Hy_H0 = Hx_H0;

% Terms 
t1 = 2.*exp(-y.*w);
t2 = 1 - exp(-h.*w);
t3 = abs(sin(b.*w));

dHx_H0_dx = t1.*t2.*t3;
dHy_H0_dx = dHx_H0_dx;

% Plot Hx/H0 and dHx/H0/dx/H0
figure
hold on
plot(w, Hx_H0)
plot(w, dHx_H0_dx)
title('Modelled MFL signal of 2D defect in infinite plane')
legend('Hx/H0', 'dHx/H0/dx')
xlabel('Spacial Frequency (/m)')
ylabel('Fraction of H0 detectable (Hx/H0 = Hy/H0)')
hold off 

figure
hold on
plot(w, log10(Hx_H0))
plot(w, log10(dHx_H0_dx))
title('Modelled MFL signal of 2D defect in infinite plane')
legend('log(Hx/H0)', 'log(dHx/H0/dx)')
xlabel('Spacial Frequency (/m)')
ylabel('Fraction of H0 detectable (log(Hx/H0) = log(Hy/H0))')
hold off 

