function grad = calculateGradient(image,GradientMatrix)

    for (i = 1:1:size(image,1))
        for (j = 1:1:size(image,2))
            grad(i,j) = calculateGradientHelper(image,i,j,GradientMatrix);
        end
    end

end
