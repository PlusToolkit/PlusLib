function I = medianFilter(image, n)
% applied a median filter on an image
% n * n median filter
% I contains the output value

    [width, height] = size(image); % TODO Check dimensions

    for (i = 1:1:width)
        for (j = 1:1:height)
            pixels = medianFilterHelper(i,j,image,n,width,height);
            I(i,j) = median(pixels,2);
        end
    end


end
