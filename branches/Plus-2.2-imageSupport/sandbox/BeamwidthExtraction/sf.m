% http://www.mathworks.de/matlabcentral/fileexchange/14862-sticks-filter
% Version from 03 May 2007

function G = sf(I,n,k);
% SF Sticks Filtering
%    SF(I,N,K) Sticks filtering for speckle noise reduction
%    This filter works for greyscale images
%
%    I is the image to be filtered
%    N is the size of the filter
%    K is the sticks thickness

% Using sticks filters to reduce speckle noise : exemples.
%   Increasing �stick� length leads to a more smoothly filtered image, 
%   at the expense of weakly highlighting tightly bound curves - a
%   result of the �stick� being longer than some of the boundary edges. 
%   Similarly, thicker �sticks� suppress more noise the expense of making
%   thin boundaries less visible. A thick �stick� can be used to smooth 
%   noise similar to a low-pass filter, with the addition of highlighting 
%   broad region differences.
%
%   Source : "Prostate Ultrasound Image Processing" by Deian Stefan 
%   and Pr. Hong Man

% 08/07 T. Mervin

S = sticks(n,k);  % generate a set of sticks filters
m = 2*n-2;        % number of filters

[mi,ni] = size(I);
mh = mi+n-1;  % size of filtered image
nh = ni+n-1;  % after convolution

H = zeros(m,mh,nh);  
G = zeros(mh,nh); 

% H = {Hi | i = 1 . . . 2n - 2}, where Hi = I * Si
for i = 1:m,
    H(i,:,:) = conv2(double(I),double(S(:,:,i)));
end;

% G(x,y) = max{Hi(x,y)}, for i = 1 . . . 2n - 2.
for i = 1:mh,
    for j = 1:nh,
        G(i,j) = max(H(:,i,j));
    end;
end;

G = uint8(round(G));  % get back to int
