function uscore = lineSelectorUniScore( line1, line2, image, toleranceZone )

    l1_x1 = line1.point1(1); %X
    l1_y1 = line1.point1(2); %Y
    l1_x2 = line1.point2(1); %X
    l1_y2 = line1.point2(2); %Y
    
    l2_x1 = line2.point1(1); %X
    l2_y1 = line2.point1(2); %Y
    l2_x2 = line2.point2(1); %X
    l2_y2 = line2.point2(2); %Y
    
    v1 = [ (l1_y2-l1_y1) (l1_x2-l1_x1) ]; %rise/run
    v2 = [ (l2_y2-l2_y1) (l2_x2-l2_x1) ]; %rise/run
    v1 = v1/norm(v1);
    v2 = v2/norm(v2);

    % We want all y values between the two lines.
    % Basic equation of a line is y = mx + b
    % For any x, the slope is v1(1)/v1(2)
    % Do this for both lines to get a range
    % We will still need b however. Fortunately we have a set of points
    % already.
    
    b1 = l1_y1 - ((v1(1)/v1(2)) * l1_x1);
    b2 = l2_y1 - ((v2(1)/v2(2)) * l2_x1);
    
    Pixels = [];
    n = size(image,2);
    
    for x = 1:1:n
        if v2(2) ~= 0 && v1(2) ~= 0
            y1 = round((v1(1)/v1(2))*x + b1);
            y2 = round((v2(1)/v2(2))*x + b2);
            if y2 > y1
                if (y1+toleranceZone < y2-toleranceZone)
                    Pixels = [ Pixels; image(y1+toleranceZone:y2-toleranceZone,x) ];
                end
            else
                if (y2+toleranceZone < y1-toleranceZone)
                    Pixels = [ Pixels; image(y2+toleranceZone:y1-toleranceZone,x) ];
                end
            end
        end
    end
    
    if size(Pixels,2) == 0
        uscore = 0; % Not enough data, assume badness
        return;
    end
    
    uscore = 1/(std(double(Pixels))/20+1);

end

