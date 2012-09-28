function score = lineSelectorHrzScore(line)

    x1 = line.point1(1); %X
    y1 = line.point1(2); %Y
    x2 = line.point2(1); %X
    y2 = line.point2(2); %Y
    
    slope = abs((y2-y1)/(x2-x1));
    
    score = 1/(log(slope+1)+1);

end
