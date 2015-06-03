function iscore = lineSelectorIntScore( line1, line2, image )

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
    
    Pixels = double([]);
    n = size(image,2);
    
    for x = 1:1:n
        if v2(2) ~= 0 && v1(2) ~= 0
            y1 = round((v1(1)/v1(2))*x + b1);
            y2 = round((v2(1)/v2(2))*x + b2);
            if y2 > y1
                Pixels = [ Pixels; image(y1:y2,x) ];
            else
                Pixels = [ Pixels; image(y2:y1,x) ];
            end
        end
    end
    
    if size(Pixels,2) == 0
        Pixels = [0];
    end
    
    iscore = mean(double(Pixels))/double(max(image(:))/2.5);
    if iscore > 1
        iscore = 1;
    elseif iscore < 0
        iscore = 0; % shouldn't happen, but...
    end

end

