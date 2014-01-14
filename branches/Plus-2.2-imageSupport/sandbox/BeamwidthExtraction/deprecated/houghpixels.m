    % Rafael C. Gonzalez
    % Richard E. Woods
    % Steven L. Eddubs
    % Digital Image Processing Using MATLAB, Pearson/Prentice Hall 2004

function [r, c] = houghpixels(f, theta, rho, rbin, cbin)

    [x,y,yval] = find(f);
    x = x - 1; y = y - 1;

    theta_c = theta(cbin) * pi / 180;
    rho_xy = x*cos(theta_c) + y*sin(theta_c);
    nrho = length(rho);
    slope = (nrho - 1)/(rho(end) - rho(1));
    rho_bin_index = round(slope*(rho_xy-rho(1)) +1);

    idx = find(rho_bin_index == rbin);
    r = x(idx) + 1; c = y(idx) + 1;

end
