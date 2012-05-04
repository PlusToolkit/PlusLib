function dscore = lineSelectorDirScore(dsingle, i, j)
% line# is the line struct from the houghLs
% imageMeans is a 1D array as the mean of image rows
% check N slopes in both directions, for both points

    dscore = -1 * dsingle(i) * dsingle(j);

end
