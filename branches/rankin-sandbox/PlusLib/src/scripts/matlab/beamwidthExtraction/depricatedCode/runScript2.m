
Path = './';

OutputPath = strcat(Path,'output-');
OutputPath = strcat(OutputPath,getTimeInStr);
OutputPath = strcat(OutputPath,'/');
mkdir(OutputPath);

% Begin parameters
  % Preprocessing parameters
    MSize = 15; % Median Filter Size
    %G = [-1 -2 -1; 0 0 0; 1 2 1]; % Gradient matrix Sobel
    G = [-3 -10 -3; 0 0 0; 3 10 3]; % Gradient matrix Scharr
    NumPeaks = 10; % Number of lines to search for before refining
  % Hough Transform and Detection parameters
    Hdrho = 0.2;   % Hough transform parameter
    Hdtheta = 0.2; % Hough transform parameter
    TMult = 0.05; % Threshold - multiplier to the max value
    PNHood = [25 3]; % Neighborhood for suppression in hough peak detection
    FGap = 20; % Hough lines parameter
    MinL = 40; % Hough lines parameter - minimum line length
  % Line Selection Parameters
    WSize = 4; % Radius to search from in computing the Direction Score
    TArea = 10; % Area from the transducer in which to ignore lines
    MLines = 2; % Minimum number of detected lines for validity
    ScoreThreshold = -0.5; % Scoring threshold, seg lines have bad properties
% End parameters
    
writeParamData(OutputPath,MSize,G,NumPeaks,Hdrho,Hdtheta,TMult,PNHood,FGap,MinL,WSize,TArea,MLines,ScoreThreshold);

errorCount = 0;
successCount = 0;
for j = 2:1:2
    Index = num2str(j);
    for i = size(Index,2)+1:1:7
        Index = strcat('0',Index);
    end
    fileName = 'input.png';
    try
        image = imread(fileName);
        image = image(:,:,1);
        imshow(image);
        figure;
    catch
        fprintf('File not found: %s\n',fileName);
        continue;
    end
    %try
        segmentBeamwidth(image,OutputPath,Index,MSize,G,NumPeaks,Hdrho,Hdtheta,TMult,PNHood,FGap,MinL,WSize,TArea,MLines,ScoreThreshold);
        successCount = successCount + 1;
        fprintf('Successfully segmented %s\n',fileName);
    %catch % Presumably a file open error
        errorCount = errorCount + 1;
        fprintf('Could not segment %s\n',fileName);
    %end
end

reportF = strcat(OutputPath,'report');
fid = fopen(reportF,'w+');
if fid ~= -1
    fprintf(fid,'Successful termination with %d bad image(s) and %d successes\n',errorCount,successCount);
    fclose(fid);
end
fprintf('Successful termination with %d bad image(s) and %d successes\n',errorCount,successCount);
