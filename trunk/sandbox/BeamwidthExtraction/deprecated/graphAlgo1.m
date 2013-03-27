% Segment processed images from the first dataset
% Make sure to run ConvertImages2 once before using this script
Path = './Data/GEv730_LinearArray/1FocalPointTop/';

% OutputPath = strcat(Path,'output-');
% OutputPath = strcat(OutputPath,getTimeInStr);
% OutputPath = strcat(OutputPath,'/');
% mkdir(OutputPath);

LineDir   = [1,0]; % [y x]
LineStart = [1,1];
currentPos= LineStart;
i = 1; % counter
value = [];

for j = 2:1:2
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
    image = double(image(:,:,1));
    image = image/255;
    image = image(200:end,200:end);
    
    %image = medfilt2(image,[3 3], 'symmetric');
    %colormap('gray');
    %surf(image(:,:,1),image(:,:,1));
    %axis equal;
    image2 = repmat(image,[1 1 3]);
    [m n] = size(image);
    range = 4;
    for y = 1:1:m
        for x = 1:1:n
            if (isMinimum(image,x,y,range))
                image2(y,x,:) = [0 0 1];
            elseif (isMaximum(image,x,y,range))
                image2(y,x,:) = [1 0 0];
            end
        end
    end
    figure;
    imshow(image);
    figure;
    imshow(image2);
end

% while isInBounds(currentPos,image)
%     value(i) = interp2(image,currentPos(2),currentPos(1));
%     i = i + 1;
%     currentPos = currentPos + LineDir;
% end

% plot(value)
