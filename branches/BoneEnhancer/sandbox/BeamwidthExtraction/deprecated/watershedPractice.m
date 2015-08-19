
image = imread('new.png');
h = fspecial('sobel');
fd = double(image);
g = sqrt(imfilter(fd,h,'replicate') .^ 2 + imfilter(fd,h','replicate') .^ 2);
L = watershed(g);
wr = L == 0;
rm = imregionalmin(image);
im = imextendedmin(image,2);
% for (i = 1:1:3)
%     fim(:,:,i) = image;
% end
% for i = 1:1:size(fim,1)
%     for j = 1:1:size(fim,2)
%         if (im(i,j))
%             fim(i,j,:) = [255;0;0];
%         end
%     end
% end

Lim = watershed(bwdist(im));
em = Lim == 0;
g2 = imimposemin(g, im | em);
g2modified = g2;
for i = 1:1:size(g2modified,1)
    for j = 1:1:size(g2modified,2)
        if g2modified(i,j) < 0
            g2modified(i,j) = 0;
        end
    end
end
g2modified = g2modified ./ max(g2modified(:));

L2 = watershed(g2);
for (i = 1:1:3)
    image2(:,:,i) = image;
end
for i = 1:1:size(image2,1)
    for j = 1:1:size(image2,2)
        if (L2(i,j) == 0)
            image2(i,j,:) = [255;0;0];
        end
    end
end
imshow(image2)


