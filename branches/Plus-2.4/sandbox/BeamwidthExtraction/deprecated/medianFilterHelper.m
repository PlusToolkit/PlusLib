function values = medianFilterHelper(x, y, image, n, width, height)
% Obtains the n * n surrounding voxel values, assuming they are within the
% bounds of the image

values = [];
offset = floor(n/2);
for i = x-offset:1:x+offset
    for j = y-offset:1:y+offset
        if i > 0 && j > 0 && i < width && j < height
            values(size(values,2)+1) = image(i,j);
        end
    end
end


end

