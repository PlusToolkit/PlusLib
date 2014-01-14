function writeLineData( fname, lineScores, uniScores, parScores, intScores, dirScores, dirScoresSingle, lines )

    fid = fopen(fname,'w+');
    if fid == -1
        fprintf('Error opening text file for lines %s', fileBase);
    else
        for i = 1:1:length(lines)
            fprintf(fid,'======= LINE NUMBER %d =======\n',i);
            fprintf(fid,'X1: %.4f\n',lines(i).point1(2));
            fprintf(fid,'Y1: %.4f\n',lines(i).point1(1));
            fprintf(fid,'X2: %.4f\n',lines(i).point2(2));
            fprintf(fid,'Y2: %.4f\n',lines(i).point2(1));
            fprintf(fid,'Dir Score: %.4f\n',dirScoresSingle(i));
        end
        limit = length(lines)*(length(lines)-1)/2;
        for i = 1:1:limit
            [i1 i2] = interpretIndex(i,length(lines));
            fprintf(fid,'======= LINE COMBO - %d AND %d =======\n',i1,i2);
            fprintf(fid,'Uniform Score: %.4f\n',uniScores(i));
            fprintf(fid,'Parallel Score: %.4f\n',parScores(i));
            fprintf(fid,'Intensity Score: %.4f\n',intScores(i));
            fprintf(fid,'Direction Score: %.4f\n',dirScores(i));
            fprintf(fid,'Total Score: %.4f\n\n',lineScores(i));
        end
        fclose(fid);
    end

end
