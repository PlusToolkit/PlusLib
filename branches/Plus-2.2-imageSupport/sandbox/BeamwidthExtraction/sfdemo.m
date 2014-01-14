% http://www.mathworks.de/matlabcentral/fileexchange/14862-sticks-filter
% Version from 03 May 2007

% Sticks filtering demo for speckle noise reduction
% 08/07 T. Mervin

I = imread('N0000030.png');
I = I(:,:,1);
J = I;%imnoise(I,'speckle');

figure(1);

subplot(2,2,1); 
imshow(I);
title('Original');

subplot(2,2,2); 
imshow(J);
title('Speckles');

G = sf(J,3,1);

subplot(2,2,3); 
imshow(G);title('Image filtered by a n = 3, k = 1 sticks filter');

G = sf(J,11,3);

subplot(2,2,4); 
imshow(G);title('n = 11, k = 3');