% Segment processed images from the first dataset
% Make sure to run ConvertImages2 once before using this script
Path = './Data/GEv730_LinearArray/1FocalPointTop/';

OutputPath = strcat(Path,'output-');
OutputPath = strcat(OutputPath,getTimeInStr);
OutputPath = strcat(OutputPath,'/');
mkdir(OutputPath);

% Begin parameters
  % Preprocessing parameters
    % Enable/disable routines here:
      % 0          : unused
      % 0 < N <= 3 : apply as the Nth operation
      Use_Median = 1; % use a median filter
      Use_Blur   = 0; % use a gaussian filter
      Use_Sticks = 0; % use a sticks filter
    MSize = [5 5]; % Median Filter Size
    BSize = [15 15]; % Blur filter size (Gaussian Blur)
    BSigma = 2;    % Blur filter sigma (radial decay of Gaussian Blur)
    SFLength = 11; % Stick length for sticks filter (despeckling)
    SFWidth  = 1;  % Stick width for sticks filter (despeckling)
  % Gradient choice for gradient filter
    %G = [-1 -2 -1; 0 0 0; 1 2 1]; % Gradient matrix Sobel
    %G = [-3 -10 -3; 0 0 0; 3 10 3]; % Gradient matrix Scharr
    G = [0 1 0; 1 -4 1; 0 1 0];
    %G = [0 0 0; 0 1 0; 0 0 0]; % Gradient matrix Scharr
  % Hough Transform and Detection parameters
    NumPeaks = 10; % Number of lines to search for before refining
    Hdrho = 0.5;   % Hough transform parameter, angle (lower = higher res)
    Hdtheta = 0.9; % Hough transform parameter, offset
    TMult = 0.05; % Threshold - multiplier to the max value
    PNHood = [27 3]; % Neighborhood for suppression in hough peak detection
    FGap = 20; % Hough lines parameter
    MinL = 40; % Hough lines parameter - minimum line length
  % Line Selection Parameters
    WSize = 2; % Radius to search from in computing the Direction Score
    DirMult = 3; % Multiply the direction score by this amount. threshold for 1 is 1/DirMult.
    TArea = 10; % Area from the transducer in which to ignore lines
    MLines = 2; % Minimum number of detected lines for validity
    tolZone = 5; % Tolerance zone for uniform measure
% End parameters

writeParamData(OutputPath,Use_Median,Use_Blur,Use_Sticks,MSize,BSize,BSigma,SFLength,SFWidth,G,NumPeaks,Hdrho,Hdtheta,TMult,PNHood,FGap,MinL,WSize,DirMult,TArea,MLines,tolZone);

errorCount = 0;
successCount = 0;
for j = 1:1:200
    Index = num2str(j);
    for i = size(Index,2)+1:1:7
        Index = strcat('0',Index);
    end
    fileName = strcat('N',Index);
    fileName = strcat(Path,fileName);
    fileName = strcat(fileName,'.png');
    try
        image = imread(fileName);
    catch
        fprintf('File not found: %s\n',fileName);
        continue;
    end
    try
        segmentBeamwidth(image,OutputPath,Index,Use_Median,Use_Blur,Use_Sticks,MSize,BSize,BSigma,SFLength,SFWidth,G,NumPeaks,Hdrho,Hdtheta,TMult,PNHood,FGap,MinL,WSize,DirMult,TArea,MLines,tolZone);
        successCount = successCount + 1;
        fprintf('Successfully segmented %s\n',fileName);
    catch
        errorCount = errorCount + 1;
        fprintf('Could not segment %s\n',fileName);
    end
end

reportF = strcat(OutputPath,'report');
fid = fopen(reportF,'w+');
if fid ~= -1
    fprintf(fid,'Successful termination with %d bad image(s) and %d successes\n',errorCount,successCount);
    fclose(fid);
end
fprintf('Successful termination with %d bad image(s) and %d successes\n',errorCount,successCount);
