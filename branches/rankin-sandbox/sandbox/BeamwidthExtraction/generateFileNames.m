function [ lname sname tname ] = generateFileNames( path, index )
% lname for raw lines
% sname for filtered lines (segmentation)
% tname for text file description

    lname = strcat('L',index);
    lname = strcat(path,lname);
    lname = strcat(lname,'.png');

    sname = strcat('S',index);
    sname = strcat(path,sname);
    sname = strcat(sname,'.png');

    tname = strcat('T',index);
    tname = strcat(path,tname);
    tname = strcat(tname,'.txt');

end
