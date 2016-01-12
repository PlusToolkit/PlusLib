function result = isMinimum( image, x, y, range )
%ISMINIMUM Summary of this function goes here
%   Detailed explanation goes here

    minimumScore = 0;
    threshold = range*2;

    for i = (y-range):1:(y+range) % y values
        if isInBounds([i,x],image) == 1
            if image(i,x) > image(y,x)
                minimumScore = minimumScore + 1;
            end
        end
    end

    if minimumScore >= threshold
        result = 1;
    else
        result = 0;
    end

end

