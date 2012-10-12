function score = lineSelectorIntScore(line, imageMeans)
% Intensity Score

    x1 = round(line.point1(2));
    x2 = round(line.point2(2));

    if (x1 < 0)
        x1 = 0;
    elseif (x1 > size(imageMeans,1))
        x1 = size(imageMeans,2);
    end

    if (x2 < 0)
        x2 = 0;
    elseif (x2 > size(imageMeans,1))
        x2 = size(imageMeans,2);
    end

    % TODO: Something needs to be implemented that is more robust to noise
    intensity = min([imageMeans(x2) imageMeans(x1)]);
    score = intensity/max(imageMeans(:));

end

