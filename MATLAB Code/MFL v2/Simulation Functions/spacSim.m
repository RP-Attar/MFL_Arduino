%% Magnetic Leakage Field Equations (spacial)
% Hx - x component of magnetic leakage field
% Hy - y component of magnetic leakage field
% H0 - magnetic field strength within the gap
% w - spacial frequency (m^-1)

function [Hx_H0, Hy_H0] = spacSim(h, two_b, y, x_low, x_high, n)
    % Define region of interest 
    if ~exist('n','var')
        % n does not exist, so default it to something
        n = 1000;
    end
    w = linspace(x_low, x_high, n);

    % Terms
    t1 = 2.*two_b.*exp(-y.*w);
    t2 = 1 - exp(-h.*w);
    t3 = sin(two_b.*w)./(w.*two_b);
    
    Hx_H0 = abs(t1.*t2.*t3);
    Hy_H0 = Hx_H0;
end