function [goodLines, lines, ...
          scores, dscores, ...
          hscores, iscores] = ...
    lineSelector(image, lines, N, transducerDist, minNumLines,threshold)
% transducerDist specifies the minimum distance from transducer to be
% considered a line. Measured in pixels.
% N would be used in the direction score, see its description

    %minNumLines = 2; % Minimum number of lines required for further computations

    % ===== REMOVE LINES TOO CLOSE =====
    for i = length(lines):-1:1
        if lines(i).point1(2) < transducerDist || ...
           lines(i).point2(2) < transducerDist
            lines(i) = [];
        end
    end
    
    if (length(lines) < minNumLines)
        error('Error: Too few lines to make adequate comparisons');
    end

    
    % ===== CALCULATE SCORES =====
    scores = zeros(length(lines),1); % Total
    dscores = zeros(length(lines),1); % Direction
    hscores = zeros(length(lines),1); % Horizontal
    iscores = zeros(length(lines),1); % Intensity
    
% OLD CODE
%     imageMeans = mean(image,2);
% 
%     for i = 1:1:length(lines)
%         dscores(i) = lineSelectorDirScore(lines(i),imageMeans,N);
%         hscores(i) = lineSelectorHrzScore(lines(i));
%         iscores(i) = lineSelectorIntScore(lines(i),imageMeans);
%         
%         scores(i) = dscores(i) * hscores(i) * iscores(i);
%     end
    
% NEW CODE
    NUMITER = 16;
    imageMeans = zeros(size(image,1),NUMITER);
    for i = 1:1:NUMITER %TODO Parameter
        lower = round((i-1)*(size(image,2)/NUMITER));
        if lower <= 0
            lower = 1;
        end
        upper = round((i  )*(size(image,2)/NUMITER));
        if upper >  size(image,2)
            upper = size(image,2);
        end
        imageMeans(:,i) = mean(image(:,lower:upper),2);
    end
    for i = 1:1:length(lines)
        sumdscores = 0;
        for j = 1:1:NUMITER
            sumdscores = sumdscores + lineSelectorDirScore(lines(i),imageMeans(:,j),N);
        end
        dscores(i) = sumdscores/NUMITER;
        
        hscores(i) = lineSelectorHrzScore(lines(i));

        sumiscores = 0;
        for j = 1:1:NUMITER
            sumiscores = sumiscores + lineSelectorIntScore(lines(i),imageMeans(:,j));
        end
        iscores(i) = sumiscores/NUMITER;
        
        scores(i) = dscores(i) * hscores(i) * iscores(i);
    end
    
    % ===== FIND BEST LINES BASED ON SCORES =====
    minIndex = 0;
    minScore = Inf;
    maxIndex = 0;
    maxScore = -Inf;
    for i = 1:1:length(lines)
        if scores(i) > maxScore
            maxScore = scores(i);
            maxIndex = i;
        end
        if scores(i) < minScore
            minScore = scores(i);
            minIndex = i;
        end
    end
    
    if maxScore <= threshold || minScore >= (-threshold)
        error('Error: Line scores are insufficient');
    end
    
    goodLines = [lines(maxIndex), lines(minIndex)];

end

