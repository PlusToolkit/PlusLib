function [i1 i2] = interpretIndex(index,maxVal)

    if maxVal < 2 || index < 1
        error('Error: Invalid values passed into interpretIndex()');
    end
    
    i1 = 1;
    subtract = maxVal-1;
    while (index > subtract)
        index = index - subtract;
        subtract = subtract - 1;
        i1 = i1 + 1;
    end
    
    i2 = index+i1;

end
