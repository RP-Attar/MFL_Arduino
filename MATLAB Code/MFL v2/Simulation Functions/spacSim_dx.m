%% Magnetic Leakage Field Equations (spacial)
% Hx - x component of magnetic leakage field
% Hy - y component of magnetic leakage field
% H0 - magnetic field strength within the gap
% w - spacial frequency (m^-1)

function [dHx_H0_dx, dHy_H0_dx] = spacSim_dx(h, two_b, y, x_low, x_high, n)
    % Define region of interest 
    if ~exist('n','var')
        % n does not exist, so default it to something
        n = 1000;
    end
    w = linspace(x_low, x_high, n);

    % Terms 
    t1 = 2.*exp(-y.*w);
    t2 = 1 - exp(-h.*w);
    t3 = abs(sin(two_b.*w));
    
    dHx_H0_dx = t1.*t2.*t3;
    dHy_H0_dx = dHx_H0_dx;
end

