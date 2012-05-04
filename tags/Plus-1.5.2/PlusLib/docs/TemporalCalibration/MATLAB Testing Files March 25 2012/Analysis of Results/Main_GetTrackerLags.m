rootFolder = 'C:\Documents and Settings\moult\My Documents\CalibrationResults_March_23_2012\BaselineStudy';

trackerLags = exploreFolders(rootFolder);

scatter(1:length(trackerLags), trackerLags, 'b');
axis([0, length(trackerLags), 0.35 0.45]);
meanTrackerLag = mean(trackerLags);
hold on;
plot([0,length(trackerLags)], [ meanTrackerLag, meanTrackerLag], 'b');
std(trackerLags)
xlabel('Test No.');
ylabel('Tracker Lag [s]');


rootFolder = 'C:\Documents and Settings\moult\My Documents\CalibrationResults_March_23_2012\SensitivityStudyDay2\BaselineStudy';
trackerLags2 = exploreFolders(rootFolder);

hold on;
scatter(1:length(trackerLags2), trackerLags2, 'r');
meanTrackerLag = mean(trackerLags2);
hold on;
plot([0,length(trackerLags)], [ meanTrackerLag, meanTrackerLag], 'r');
std(trackerLags2)

legend('Baseline Study Day One','Baseline Study Day One (Mean)','Baseline Study Day Two','Baseline Study Day Two (Mean)')

title('Temporal Calibration March 23 2012')