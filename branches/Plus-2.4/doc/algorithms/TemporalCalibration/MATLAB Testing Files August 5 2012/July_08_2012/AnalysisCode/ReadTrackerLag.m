function [isValid, trackerLag, calibrationError, maxCalibrationError] = ReadTrackerLag(rootFolder, rootCodeFolder)

cd(rootFolder);
av_files = dir();
trackerLag = 0;
isValid = false;

trackerLagFilename = 'TemporalCalibrationResults.xml';
   
for i = 1 : length(av_files)
    if(~isempty(strfind(av_files(i).name, trackerLagFilename)))
        TrackerFileName = av_files(i).name;
        isValid = true;
        break;
    end
end

if(isValid)
    cd(rootCodeFolder);
    xmlstr = fileread([rootFolder, '\',  trackerLagFilename]);
    V = xml_parseany(xmlstr);
    trackerLag = str2num(V.ATTRIBUTE.TrackerLagSec);
    calibrationError = str2num(V.ATTRIBUTE.CalibrationError);
    maxCalibrationError = str2num(V.ATTRIBUTE.MaxCalibrationError);
end






