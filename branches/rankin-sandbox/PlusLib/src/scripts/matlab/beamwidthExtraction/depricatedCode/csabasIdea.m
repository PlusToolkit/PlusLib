% == PARAMETERS ==
% Cleanest sample image seems to be N0000228.png
ImageFile = './Data/GEv730_LinearArray/1FocalPointTop/N0000016.png';
N = 15; % Size of N*N median filter, should be odd
%GradientMatrix = [-1 -2 -1; 0 0 0 ; 1 2 1]; % Sobel
GradientMatrix = [-3 -10 -3; 0 0 0; 3 10 3]; % Scharr
NumLines = 7;
ThresholdMult = 0.25;

% == EXECUTION CODE ==
image0 = imread(ImageFile);
image = image0;
%image = medfilt2(image,[N N],'symmetric');
image = medianFilter(image,N);
image = calculateGradient(image,GradientMatrix);
%image = imfilter(image,GradientMatrix);
image = abs(image);
[houghTransform theta rho] = hough(image);
[r c houghPs] = houghpeaks(houghTransform,NumLines,...
                ThresholdMult*max(houghTransform(:)));
houghLs = houghlines(image0,theta,rho,r,c);

% Select the best lines
houghLs = lineSelector(image0,houghLs,4,10);

% == VISUALIZATION ==
%figure;
%imshow(image,[min(image(:)),max(image(:))]);
Colors = [1 0 0; 1 0.5 0; 1 1 0; 0 1 0; 0 1 1; 0 0 1; 1 0 1];
figure;
imshow(image0);
hold on;
for k = 1:1:length(houghLs)
    xy = [houghLs(k).point1; houghLs(k).point2];
    plot(xy(:,2), xy(:,1), 'LineWidth', 1, 'Color',Colors(k,:));
end
hold off;

% figure;
% imshow(houghTransform,[min(houghTransform(:)),max(houghTransform(:))]);
% figure;
% imshow(hnew,[min(houghTransform(:)),max(houghTransform(:))]);


