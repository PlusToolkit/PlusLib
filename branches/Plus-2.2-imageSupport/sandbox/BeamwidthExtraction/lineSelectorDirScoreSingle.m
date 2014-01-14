function dscore = lineSelectorDirScoreSingle(line,image,N,multiplier)
% line is the line struct from the houghLs
% image is the image
% check N slopes in both directions, for both points

    l1_x1 = line.point1(1); %X
    l1_y1 = line.point1(2); %Y
    l1_x2 = line.point2(1); %X
    l1_y2 = line.point2(2); %Y
    
    v1 = [ (l1_y2-l1_y1) (l1_x2-l1_x1) ]; %rise/run
    v1 = v1/norm(v1);

    % We want all y values between the two lines.
    % Basic equation of a line is y = mx + b
    % For any x, the slope is v1(1)/v1(2)
    % Do this for both lines to get a range
    % We will still need b however. Fortunately we have a set of points
    % already.
    
    b1 = l1_y1 - ((v1(1)/v1(2)) * l1_x1);
    
    scores = [];
    n = size(image,2);
    highestPossible = 2*N + 1; % for each line
    
    for x = 1:1:n
        score = 0;
        y1 = round((v1(1)/v1(2))*x + b1);
        for i = y1-N:1:y1+N
            if i-1 > 0 && i+1 < size(image,1)
                slope = double(double(image(i+1,x)) - double(image(i-1,x)));
                if slope > 0
                    score = score + 1;
                elseif slope < 0
                    score = score - 1;
                end
            end
        end
        % Transform to a sinusoidal graph
        scores(x) = sin((score/highestPossible)*(pi/2)); %#ok<AGROW>
    end
    
    dscore = mean(scores);
    
    % Thresholding
    dscore = dscore * multiplier;
    if dscore > 1
        dscore = 1;
    elseif dscore < -1
        dscore = -1;
    end

end
