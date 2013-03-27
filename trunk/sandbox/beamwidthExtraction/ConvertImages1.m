% This script automatically converts the images in the dataset
% "GEv730_LinearArray" to the format needed by the algorithms

Path = './Data/GEv730_LinearArray/1FocalPointTop/';
FileCount = 0;

for j = 1:1:236
    Index = num2str(j);
    for i = size(Index,2)+1:1:7
        Index = strcat('0',Index);
    end
    OldName = strcat('I',Index);
    OldName = strcat(OldName,'.jpg');
    NewName = strcat('N',Index);
    NewName = strcat(NewName,'.png');
    fileName = strcat(Path,OldName);
    fileExt = 'jpg';
    newFileName = strcat(Path,NewName);
    newFileExt = 'png';
    try
        formatImage(fileName,fileExt,161,560,108,507,newFileName,newFileExt);
        FileCount = FileCount + 1;
        fprintf('Wrote file %d\n',j);
    catch % Presumably a file open error
        fprintf('Could not find file %d\n',j);
    end
end

fprintf('\nSuccessfully written %d files\n',FileCount);
