function G = calculateGradientHelper(image,x,y,GradientMatrix)

    %GradientMatrix = [-1,-2,-1;0,0,0;1,2,1];

    % indexing will start from 1, then add offset to get to desired
    % coordinate
    offsetX = x - floor(size(GradientMatrix,1)/2);
    offsetY = y - floor(size(GradientMatrix,2)/2);
    width = size(image,1);
    height = size(image,2);

    % Assume gradient starts at zero
    G = 0;

    for (i = 1:1:size(GradientMatrix,1))
        XCoordinate = i + offsetX;
        for (j = 1:1:size(GradientMatrix,2))
            YCoordinate = j + offsetY;
            % TODO: Better handling of out of bounds data
            if (XCoordinate < width && XCoordinate > 0 && ...
                YCoordinate <height && YCoordinate > 0)
                G = G + GradientMatrix(i,j)*image(XCoordinate,YCoordinate);
            else
                G = 0; return;
            end
        end
    end

end

