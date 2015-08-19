function formatImage(file, xtnsn,minX,maxX,minY,maxY,newName,newExt)
% Format an image to a grayscale PNG file containing only
% the area defined by minX,maxX,minY,maxY.

    image = imread(file, xtnsn);
    image = image(minY:maxY,minX:maxX);
    imwrite(image,newName,newExt);

end
