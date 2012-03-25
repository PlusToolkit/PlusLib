function [isValid, trackerLag] = ReadTrackerLag(rootFolder)

cd(rootFolder);
av_files = dir();
trackerLag = 0;
isValid = false;

trackerLagFilename = 'TrackerLag.txt';
for i = 1 : length(av_files)
    if(~isempty(strfind(av_files(i).name, trackerLagFilename)))
        TrackerFileName = av_files(i).name;
        isValid = true;
        break;
    end
end

if(isValid)
    trackerLag = dlmread(trackerLagFilename);
end






