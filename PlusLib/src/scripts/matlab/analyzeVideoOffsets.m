% Analyze video offsets acquired by DiagDataCollectionScript.bat 

videoOffsetSummary=[];

videoOffset=getVideoOffset('01_VideoPositions.csv', '01_SNXTCH_AscensionTracker_Buffer.mha')
videoOffsetSummary=[videoOffsetSummary; videoOffset];
pause
videoOffset=getVideoOffset('02_VideoPositions.csv', '02_SNXTCH_AscensionTracker_Buffer.mha')
videoOffsetSummary=[videoOffsetSummary; videoOffset];
pause
videoOffset=getVideoOffset('03_VideoPositions.csv', '03_SNXTCH_AscensionTracker_Buffer.mha')
videoOffsetSummary=[videoOffsetSummary; videoOffset];
pause
videoOffset=getVideoOffset('04_VideoPositions.csv', '04_SNXTCH_AscensionTracker_Buffer.mha')
videoOffsetSummary=[videoOffsetSummary; videoOffset];
pause
videoOffset=getVideoOffset('05_VideoPositions.csv', '05_SNXTCH_AscensionTracker_Buffer.mha')
videoOffsetSummary=[videoOffsetSummary; videoOffset];
pause
videoOffset=getVideoOffset('06_VideoPositions.csv', '06_SNXTCH_AscensionTracker_Buffer.mha')
videoOffsetSummary=[videoOffsetSummary; videoOffset];
pause
videoOffset=getVideoOffset('07_VideoPositions.csv', '07_SNXTCH_AscensionTracker_Buffer.mha')
videoOffsetSummary=[videoOffsetSummary; videoOffset];
pause
videoOffset=getVideoOffset('08_VideoPositions.csv', '08_SNXTCH_AscensionTracker_Buffer.mha')
videoOffsetSummary=[videoOffsetSummary; videoOffset];
pause
videoOffset=getVideoOffset('09_VideoPositions.csv', '09_SNXTCH_AscensionTracker_Buffer.mha')
videoOffsetSummary=[videoOffsetSummary; videoOffset];
pause
videoOffset=getVideoOffset('10_VideoPositions.csv', '10_SNXTCH_AscensionTracker_Buffer.mha')
videoOffsetSummary=[videoOffsetSummary; videoOffset];

meanVideoOffset=mean(videoOffsetSummary)
stdVideoOffset=std(videoOffsetSummary)
