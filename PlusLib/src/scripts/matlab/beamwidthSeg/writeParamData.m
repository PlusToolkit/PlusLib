function writeParamData(Path,i1,i2,i3,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q)

    file = strcat(Path,'parameters');
    fid = fopen(file,'w+');
    if fid == -1
        fprintf('Error opening %s',file);
    else
        fprintf(fid,'META INFORMATION\n');
        fprintf(fid,'Date %s\n',date);
        cl = clock;
        fprintf(fid,'Time %d:%d:%d\n\n',cl(4),cl(5),round(cl(6)));
        fprintf(fid,'PARAMETERS\n');
        fprintf(fid,'Order of operations:\n');
        for index = 1:1:3
            if (index == i1)
                fprintf(fid,' Median Filter\n');
            elseif (index == i2)
                fprintf(fid,' Blur Filter\n');
            elseif (index == i3)
                fprintf(fid,' Sticks Filter\n');
            end
        end
        if (i1 > 0)
            fprintf(fid,'Median Filter Size: [%d %d]\n', a(1), a(2));
        end
        if (i2 > 0)
            fprintf(fid,'Blur Filter Size: [%d %d]\n', b(1), b(2));
            fprintf(fid,'Blur Filter Sigma: %d\n', c);
        end
        if (i3 > 0)
            fprintf(fid,'Sticks Filter Length: %d\n', d);
            fprintf(fid,'Sticks Filter Width: %d\n', e);
        end
        fprintf(fid,'Gradient matrix:\n');
        for index = 1:1:size(f,2)
            fprintf(fid,'    [ ');
            for jndex = 1:1:size(f,1)
                fprintf(fid,'%d ',f(index,jndex));
            end
            fprintf(fid,']\n');
        end
        fprintf(fid,'Number of lines to find for before refining: %d\n',g);
        fprintf(fid,'Hough theta: %.4f\n', h);
        fprintf(fid,'Hough rho: %.4f\n', i);
        fprintf(fid,'Threshold - multiplier to the max value: %.4f\n', j);
        fprintf(fid,'Hough peak neighborhood: [%d %d]\n', k(1), k(2));
        fprintf(fid,'Hough fillgap: %d\n', l);
        fprintf(fid,'Hough minimum line length: %d\n', m);
        fprintf(fid,'Radius to search from in line filtering: %d\n', n);
        fprintf(fid,'Area from transducer to ignore lines: %d\n', o);
        fprintf(fid,'Min # of lines detected for validity: %d\n', p);
        fprintf(fid,'Scoring threshold in line selection: %.4f\n', q);
        fprintf(fid,'Any parameters not included are default.\n\n\n');
        fclose(fid);
    end
    
end

