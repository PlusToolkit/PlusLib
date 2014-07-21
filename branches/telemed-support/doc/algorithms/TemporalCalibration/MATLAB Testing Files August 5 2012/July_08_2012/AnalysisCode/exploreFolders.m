function [trackerLags, calibrationErrors, maxCalibrationErrors] = exploreFolders(rootFolder, rootCodeFolder)

cd(rootFolder);
CARS_folders = dir();
trackerLags = [];
calibrationErrors = [];
maxCalibrationErrors = [];

for t = 3 : length(CARS_folders)
    cd(rootCodeFolder)
    if(isdir([rootFolder, '\', CARS_folders(t).name]))
        [tempTrackerLags, tempCalibrationErrors, tempMaxCalibrationErrors] = exploreFolders([rootFolder, '\', CARS_folders(t).name], rootCodeFolder);
        trackerLags = [trackerLags, tempTrackerLags];
        calibrationErrors = [calibrationErrors, tempCalibrationErrors];
        maxCalibrationErrors = [maxCalibrationErrors, tempMaxCalibrationErrors];
    else
        [isValid, trackerLagX, calibrationErrorX, maxCalibrationErrorX] = ReadTrackerLag(rootFolder, rootCodeFolder);
        if(isValid)
            trackerLags = trackerLagX;
            calibrationErrors = calibrationErrorX;
            maxCalibrationErrors = maxCalibrationErrorX;
        end
        break
    end
end


