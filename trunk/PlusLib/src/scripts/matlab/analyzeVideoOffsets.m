% Analyze video offsets acquired by DiagDataCollectionScript.bat 
videoOffsetSummary=[];

trackerMhaFiles=['02_SNXTCH_NDICertusTracker_Buffer_01.mha'; '02_SNXTCH_NDICertusTracker_Buffer_02.mha'; '02_SNXTCH_NDICertusTracker_Buffer_03.mha']; 
videoOffset=getVideoOffset('02_VideoPositions.csv', trackerMhaFiles)
videoOffsetSummary=[videoOffsetSummary; videoOffset];
pause
trackerMhaFiles=['03_SNXTCH_NDICertusTracker_Buffer_01.mha'; '03_SNXTCH_NDICertusTracker_Buffer_02.mha'; '03_SNXTCH_NDICertusTracker_Buffer_03.mha']; 
videoOffset=getVideoOffset('03_VideoPositions.csv', trackerMhaFiles)
videoOffsetSummary=[videoOffsetSummary; videoOffset];
pause
trackerMhaFiles=['04_SNXTCH_NDICertusTracker_Buffer_01.mha'; '04_SNXTCH_NDICertusTracker_Buffer_02.mha'; '04_SNXTCH_NDICertusTracker_Buffer_03.mha']; 
videoOffset=getVideoOffset('04_VideoPositions.csv', trackerMhaFiles)
videoOffsetSummary=[videoOffsetSummary; videoOffset];
pause
trackerMhaFiles=['05_SNXTCH_NDICertusTracker_Buffer_01.mha'; '05_SNXTCH_NDICertusTracker_Buffer_02.mha'; '05_SNXTCH_NDICertusTracker_Buffer_03.mha']; 
videoOffset=getVideoOffset('05_VideoPositions.csv', trackerMhaFiles)
videoOffsetSummary=[videoOffsetSummary; videoOffset];
pause
%trackerMhaFiles=['06_SNXTCH_NDICertusTracker_Buffer_01.mha'; '06_SNXTCH_NDICertusTracker_Buffer_02.mha'; '06_SNXTCH_NDICertusTracker_Buffer_03.mha']; 
%videoOffset=getVideoOffset('06_VideoPositions.csv', trackerMhaFiles)
%videoOffsetSummary=[videoOffsetSummary; videoOffset];
%pause
%trackerMhaFiles=['07_SNXTCH_NDICertusTracker_Buffer_01.mha'; '07_SNXTCH_NDICertusTracker_Buffer_02.mha'; '07_SNXTCH_NDICertusTracker_Buffer_03.mha']; 
%videoOffset=getVideoOffset('07_VideoPositions.csv', trackerMhaFiles)
%videoOffsetSummary=[videoOffsetSummary; videoOffset];
%pause
trackerMhaFiles=['08_SNXTCH_NDICertusTracker_Buffer_01.mha'; '08_SNXTCH_NDICertusTracker_Buffer_02.mha'; '08_SNXTCH_NDICertusTracker_Buffer_03.mha'; '08_SNXTCH_NDICertusTracker_Buffer_04.mha']; 
videoOffset=getVideoOffset('08_VideoPositions.csv', trackerMhaFiles)
videoOffsetSummary=[videoOffsetSummary; videoOffset];
pause
%trackerMhaFiles=['09_SNXTCH_NDICertusTracker_Buffer_01.mha'; '09_SNXTCH_NDICertusTracker_Buffer_02.mha'; '09_SNXTCH_NDICertusTracker_Buffer_03.mha'; '09_SNXTCH_NDICertusTracker_Buffer_04.mha']; 
%videoOffset=getVideoOffset('09_VideoPositions.csv', trackerMhaFiles)
%videoOffsetSummary=[videoOffsetSummary; videoOffset];
%pause
trackerMhaFiles=['10_SNXTCH_NDICertusTracker_Buffer_01.mha'; '10_SNXTCH_NDICertusTracker_Buffer_02.mha'; '10_SNXTCH_NDICertusTracker_Buffer_03.mha']; 
videoOffset=getVideoOffset('10_VideoPositions.csv', trackerMhaFiles)
videoOffsetSummary=[videoOffsetSummary; videoOffset];

meanVideoOffset=mean(videoOffsetSummary)
stdVideoOffset=std(videoOffsetSummary)
