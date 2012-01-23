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
    
    imageMeans = mean(image,2);
    for i = 1:1:length(lines)
        dscores(i) = lineSelectorDirScore(lines(i),imageMeans,N);
        hscores(i) = lineSelectorHrzScore(lines(i));
        iscores(i) = lineSelectorIntScore(lines(i),imageMeans);
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

