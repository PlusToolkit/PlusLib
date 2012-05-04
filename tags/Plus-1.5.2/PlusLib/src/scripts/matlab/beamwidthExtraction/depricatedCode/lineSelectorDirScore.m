function score = lineSelectorDirScore(line,imageMeans,N)
% line is the line struct from the houghLs
% imageMeans is a 1D array as the mean of image rows
% dir can be +1 (first line) or -1 (second line)
% check N slopes in both directions, for both points

    point1 = round(line.point1(2));
    point2 = round(line.point2(2));
    
    score = 0;
    
    for p = [point1 point2]
        for i = -N:1:N
            if p+i-1 > 0 && p+i+1 < size(imageMeans,1)
                slope = (imageMeans(p+i+1) - imageMeans(p+i-1));
                if slope > 0
                    score = score + 1;
                elseif slope < 0
                    score = score - 1;
                end
            end
        end
    end
    
    % Transform to a sinusoidal graph
    highestPossible = 2*(2*N + 1);
    score = sin((score/highestPossible)*(pi/2));

end
