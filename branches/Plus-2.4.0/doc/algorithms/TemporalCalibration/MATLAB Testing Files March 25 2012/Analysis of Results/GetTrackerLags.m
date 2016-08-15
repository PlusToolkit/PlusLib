function trackerLags = GetTrackerLags(rootFolder)

cd(rootFolder);
CARS_folders = dir();
trackerLags = [];

for t = 3 : length(CARS_folders)
    if(isdir([rootFolder, '\', CARS_folders(t).name]))
        tempTrackerLags = exploreFolders([rootFolder, '\', CARS_folders(t).name]);
        trackerLags = horzcat(trackerLags,tempTrackerLags);
    else
        [isValid, trackerLag] = ReadTrackerLag(rootFolder);
        if(isValid)
            trackerLags = horzcat(trackerLags,trackerLag);
        end
        break
    end
end
