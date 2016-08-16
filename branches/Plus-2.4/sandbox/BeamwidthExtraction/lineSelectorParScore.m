function pscore = lineSelectorParScore( line1, line2 )

    l1_x1 = line1.point1(1); %X
    l1_y1 = line1.point1(2); %Y
    l1_x2 = line1.point2(1); %X
    l1_y2 = line1.point2(2); %Y
    
    l2_x1 = line2.point1(1); %X
    l2_y1 = line2.point1(2); %Y
    l2_x2 = line2.point2(1); %X
    l2_y2 = line2.point2(2); %Y
    
    v1 = [ (l1_y2-l1_y1) (l1_x2-l1_x1) ];
    v2 = [ (l2_y2-l2_y1) (l2_x2-l2_x1) ];

    v1 = v1/norm(v1);
    v2 = v2/norm(v2);
    
    pscore = dot(v1,v2);

end
