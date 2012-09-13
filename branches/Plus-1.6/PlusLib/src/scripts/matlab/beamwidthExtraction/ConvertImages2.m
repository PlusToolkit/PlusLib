% This script automatically converts the images in the dataset
% "SonixRP_LinearArray" to the format needed by the algorithms

Path = './Data/SonixRP_LinearArray/DataBeamWidth3/';
FileCount = 0;

for j = 0:1:300
    Index = num2str(j);
    NIndex = Index;
    for i = size(Index,2)+1:1:4
        Index = strcat('0',Index);
    end
    for i = size(NIndex,2)+1:1:7
        NIndex = strcat('0',NIndex);
    end
    OldName = strcat('CapturedImageID_NO_',Index);
    OldName = strcat(OldName,'.bmp');
    NewName = strcat('N',NIndex);
    NewName = strcat(NewName,'.png');
    fileName = strcat(Path,OldName);
    fileExt = 'bmp';
    newFileName = strcat(Path,NewName);
    newFileExt = 'png';
    try
        formatImage(fileName,fileExt,180,462,36,445,newFileName,newFileExt);
        FileCount = FileCount + 1;
        fprintf('Wrote file %d\n',j);
    catch % Presumably a file open error
        fprintf('Could not find file %d\n',j);
    end
end

fprintf('\nSuccessfully written %d files\n',FileCount);
