function writeImage( fname, image, lines, f )
% f = 0 use colors
% f = 1 use green only

    if f == 0
        Colors(:,1) = [1 0 0];
        Colors(:,2) = [1 0.5 0];
        Colors(:,3) = [1 1 0];
        Colors(:,4) = [0 1 0];
        Colors(:,5) = [0 0.5 1];
        Colors(:,6) = [0 0 1];
        Colors(:,7) = [0.5 0 1];
        Colors(:,8) = [1 0 1];
        Colors(:,9) = [1 0.5 0.5];
        Colors(:,10) = [1 1 1];
        Colors(:,11) = [0 0 0];
        Colors(:,12) = [0 0 0];
        Colors(:,13) = [0 0 0];
        Colors(:,14) = [0 0 0];
        Colors(:,15) = [0 0 0];
    else
        Colors(:,1) = [0 1 0];
        Colors = repmat(Colors,1,15);
    end

    h = imshow(image);
    hold on;
    for k = 1:1:length(lines) % credit to the authors of the book
        xy = [lines(k).point1; lines(k).point2];
        plot(xy(:,1), xy(:,2), 'LineWidth', 1, 'Color',Colors(:,k));
    end
    saveas(h,fname);
    hold off;

end

