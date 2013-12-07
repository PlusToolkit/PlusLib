% Automatic test script
%

% Build a list of file names with full path from test directory
testDataDir = '.\testData';
dirData =  dir( fullfile(testDataDir,'*.mh*') );
fileList = {dirData.name}';
if ~isempty(fileList)
    fileList = cellfun(@(x) fullfile(testDataDir,x), fileList,'UniformOutput',false); % prepend path to files
end

% Output directories for any data that is written
outputVolDirName='testOutputFiles';
outputVolDir = strcat(testDataDir,'\',outputVolDirName);
% Create output directory if it does't exist
if ~exist(outputVolDir, 'dir')
    createOutputVolDirStatus = mkdir(testDataDir,outputVolDirName);
    if (~createOutputVolDirStatus)
        fprintf('Could not create output directory %s in %s\n',outputVolDirName,testDataDir);
        return
    end    
end

numOfErrors=0;

for i=1:length(fileList)
    for writePixelData=0:1    
        
        fprintf('TEST %i ',i);
        
        % prepare output file names to write to
        outputFileName = strcat(testDataDir,'\',outputVolDirName,fileList{i}(length(testDataDir)+1:end));        
        
        % read all test data
        fprintf('\tReading file: %s\n', fileList{i});
        volumeStruct = mha_read_volume(fileList{i},writePixelData);
        
        % write all test data
        fprintf('\tWriting file: %s\n', outputFileName);
        mha_write_volume(outputFileName,volumeStruct,writePixelData);
        
        % read written data
        fprintf('\tReading written file: %s\n', outputFileName);
        volumeStructAfterReadWrite = mha_read_volume(outputFileName,writePixelData);
        
        % check if data read in equivalent to data written out then reread
        if (isequal(volumeStruct,volumeStructAfterReadWrite))
            fprintf('\tContent Comparison Test: Passed\n');
        else
            fprintf('\tContent Comparison: Failed\n');
            numOfErrors=numOfErrors+1;
        end
    end
end

fprintf('--------------------------\n');
if (numOfErrors==0)
    fprintf('Overall test result: Passed\n');
else
    fprintf('Overall test result: Failed (%d error)\n',numOfErrors);
end
