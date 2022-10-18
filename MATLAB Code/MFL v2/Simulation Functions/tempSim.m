%% Magnetic Leakage Field Equations (temporal)
% Hx - x component of magnetic leakage field
% Hy - y component of magnetic leakage field
% H0 - magnetic field strength within the gap

function [Hx_H0, Hy_H0] = tempSim(h, two_b, y, x_low, x_high, n)
    % Define region of interest 
    if ~exist('n','var')
        % n does not exist, so default it to something
        n = 1000;
    end
    x = linspace(x_low, x_high, n);
    
    % Hx/H0 terms
    t1 = h.*(x+two_b);
    t2 = (x+two_b).^2;
    t3 = y*(y+h);
    t4 = h.*(x-two_b);
    t5 = (x-two_b).^2;
    t6 = t3;
    
    Hx_H0 = (1/pi).*( atan((t1./(t2+t3))) - atan((t4./(t5+t6))) );
    
    % Hy/H0 terms
    t1 = (x+two_b).^2;
    t2 = (y+h)^2;
    t3 = (x-two_b).^2;
    t4 = y^2;
    t5 = (x-two_b).^2;
    t6 = t2;
    t7 = (x+two_b).^2;
    t8 = t4;
    
    Hy_H0 = (1/(2*pi)).*log(((t1+t2).*(t3+t4)) ./ ((t5+t6).*(t7+t8)));
end