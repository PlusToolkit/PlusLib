function isInside = isInBounds( pos, image )

    [n m] = size(image);
    if (pos(1) > 0 && pos(1) <= n && pos(2) > 0 && pos(1) <= m)
        isInside = 1;
    else
        isInside = 0;
    end

end

