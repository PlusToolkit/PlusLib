function [goodLines, lines, scores, uscores, pscores, iscores, dscores dsinglescores] = ...
    lineSelector(image, lines, N, dirMult, transducerDist, minNumLines,tolZone)
% transducerDist specifies the minimum distance from transducer to be
% considered a line. Measured in pixels.
% N would be used in the direction score, see its description

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
    numPossiblePairs = length(lines) * (length(lines)-1) / 2;
    scores = zeros(numPossiblePairs,1); % Total
    uscores = zeros(numPossiblePairs,1); % Uniformity
    pscores = zeros(numPossiblePairs,1); % Parallel
    iscores = zeros(numPossiblePairs,1); % Intensity
    dscores = zeros(numPossiblePairs,1); % Direction
    dsinglescores = zeros(length(lines),1); % Direction Score (single line)
    
    for i = 1:1:length(lines)
        dsinglescores(i) = lineSelectorDirScoreSingle(lines(i),image,N,dirMult);
    end
    
    index = 1;
    for i = 1:1:length(lines)
        for j = (i+1):1:length(lines)
            uscores(index) = lineSelectorUniScore(lines(i), lines(j), image, tolZone);
            pscores(index) = lineSelectorParScore(lines(i), lines(j));
            iscores(index) = lineSelectorIntScore(lines(i), lines(j), image);
            dscores(index) = lineSelectorDirScore(dsinglescores, i, j);
            scores(index) = uscores(index) * pscores(index) * iscores(index) * dscores(index);
            index = index + 1;
        end
    end

    % ===== FIND BEST LINES BASED ON SCORES =====
    maxVal = -Inf;
    bestPair = [0 0];
    for i = 1:1:index-1
        if scores(i) > maxVal
            [bestPair(1) bestPair(2)] = interpretIndex(i,length(lines));
            maxVal = scores(i);
        end
    end
    
    goodLines = [lines(bestPair(1)), lines(bestPair(2))];

end
