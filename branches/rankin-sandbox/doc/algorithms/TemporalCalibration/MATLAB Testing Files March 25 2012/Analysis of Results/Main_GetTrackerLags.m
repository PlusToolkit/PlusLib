rootFolder = 'C:\Documents and Settings\moult\My Documents\TemporalCalibrationTests\May_13_2012';
close all;
[trackerLags, calibrationErrors, maxCalibrationErrors] = exploreFolders(rootFolder);

xlswrite('PertinentStatistics',[(1:length(trackerLags))', trackerLags']);
%% Get mean and standard deviation of the tracker lags and calibration
%% errors
stdTrackerLag = std(trackerLags);
meanTrackerLag = mean(trackerLags);

stdCalibrationError = std(calibrationErrors);
meanCalibrationError = mean(calibrationErrors);

%% Plot of tracker lags vs. trial number
hTrackerLagData  = line(1:length(trackerLags), trackerLags);
axisXRange = xlim;
set(gca,'XTick',axisXRange(1):1:axisXRange(2));
axisYRange = ylim;
set(gca,'YTick',axisYRange(1):0.005:axisYRange(2));
set(gca, 'YTickLabel', num2str(get(gca,'YTick')','%.3f')) 


hold on;

meanTrackerLagsX = 0:1:length(trackerLags);
for i = 1 : length(meanTrackerLagsX )
    meanTrackerLagsY(i) = meanTrackerLag;
end

meanLine = line(meanTrackerLagsX, meanTrackerLagsY);

lowerBoundX = 0:1:length(trackerLags);
for i = 1 : length(lowerBoundX)
    lowerBoundY(i) = meanTrackerLag - 0.005;
end

upperBoundX = 0:1:length(trackerLags);
for i = 1 : length(upperBoundX)
    upperBoundY(i) = meanTrackerLag + 0.005;
end

hBounds(1) = line(lowerBoundX, lowerBoundY);
hBounds(2) = line(upperBoundX, upperBoundY);

set(hTrackerLagData               , ...
  'LineStyle'       , 'none'      , ...
  'Marker'          , '.'         );
set(meanLine                      , ...
  'LineStyle'       , '--'        , ...
  'Color'           , 'r'         );
set(hBounds(1)                    , ...
  'LineStyle'       , '-.'        , ...
  'Color'           , [0 .5 0]    );
set(hBounds(2)                    , ...
  'LineStyle'       , '-.'        , ...
  'Color'           , [0 .5 0]    );

set(meanLine                      , ...
  'LineWidth'       , 1.5         );

set(hTrackerLagData               , ...
  'Marker'          , 'o'         , ...
  'MarkerSize'      , 5           , ...
  'MarkerEdgeColor' , 'none'      , ...
  'MarkerFaceColor' , [.3 .3 .3] );

for i = 1 : length(trackerLags)
    if(abs(trackerLags(i) - meanTrackerLag) > 0.005)
        st = horzcat('Time from mean: ', num2str(abs(trackerLags(i) - meanTrackerLag)));
%         str1(1) = {'Time from mean: ', num2str(abs(trackerLags(i) - meanTrackerLag))};
        %text(i,trackerLags(i) + 0.005,str1)        
        text(i,trackerLags(i),i,strcat(st,'\rightarrow'), 'HorizontalAlignment','right','FontSize',10);
    end
end

xlabel('Test Number');
ylabel('Tracker Lag [s]');
legend('Tracker Lag', 'Mean Tracker Lag', '+/- 0.005 [s] Boundary', 'Location', 'NorthEastOutside');
titleString = horzcat('Tracker Lag vs. Trial Number (BaselineStudy; ', date, ' )');
title(titleString ,'FontWeight','bold')

set(gcf, 'Units', 'Normalized', 'OuterPosition', [0 0 1 1]); 
set(gcf, 'PaperPositionMode', 'auto');
print -depsc2 TrackerLagVsTrialNumber.eps

%% Plot of tracker lag distance (time) from mean tracker lag vs. mean
%% correlation error
trackerLagsDifferenceFromMean = abs(meanTrackerLag - trackerLags);
figure; subplot(1,2,1); hDistFromMeanAndCorrError = line(calibrationErrors, trackerLagsDifferenceFromMean);

set(hDistFromMeanAndCorrError     , ...
  'LineStyle'       , 'none'      , ...
  'Marker'          , '.'         );

set(hDistFromMeanAndCorrError     , ...
  'Marker'          , 'o'         , ...
  'MarkerSize'      , 5           , ...
  'MarkerEdgeColor' , 'none'      , ...
  'MarkerFaceColor' , [.3 .3 .3] );
   
title('Tracker Lag Difference from Mean Tracker Lag vs. Correlation Error','FontWeight','bold')

set(gcf, 'Units', 'Normalized', 'OuterPosition', [0 0 1 1]); 
set(gcf, 'PaperPositionMode', 'auto');
print -depsc2 DistanceFromMeanTrackerLagVsCorrelationError.eps

xlabel('Correlation Error [mm]');
ylabel('Tracker Lag (Time) Distance From Mean Tracker Lag [s]');

%% Plot of tracker lag distance (time) from mean tracker lag vs max
%% correlation error
trackerLagsDifferenceFromMean = abs(meanTrackerLag - trackerLags);
subplot(1,2,2); hDistFromMeanAndMaxCorrError = line(maxCalibrationErrors, trackerLagsDifferenceFromMean);

for i = 1 : length(trackerLags)
    st = horzcat('Trial #', num2str(i));   
    text(maxCalibrationErrors(i), trackerLagsDifferenceFromMean(i),strcat(st,'\rightarrow'), 'HorizontalAlignment','right','FontSize',10);
end

set(hDistFromMeanAndMaxCorrError     , ...
  'LineStyle'       , 'none'      , ...
  'Marker'          , '.'         );

set(hDistFromMeanAndMaxCorrError     , ...
  'Marker'          , 'o'         , ...
  'MarkerSize'      , 5           , ...
  'MarkerEdgeColor' , 'none'      , ...
  'MarkerFaceColor' , [.3 .3 .3] );


title('Tracker Lag Difference from Mean Tracker Lag vs. Max Correlation Error','FontWeight','bold')

set(gcf, 'Units', 'Normalized', 'OuterPosition', [0 0 1 1]); 
set(gcf, 'PaperPositionMode', 'auto');
print -depsc2 DistanceFromMeanTrackerLagVsMaxCorrelationError.eps

xlabel('Max Correlation Error [mm]');
ylabel('Tracker Lag (Time) Distance From Mean Tracker Lag [s]');

%% Scatter plot for the tracker lags
xlabel('Test No.');
ylabel('Tracker Lag [s]');


rootFolder = 'C:\Documents and Settings\moult\My Documents\CalibrationResults_March_28_2012\SensitivityStudyDay2\BaselineStudy';
trackerLags2 = exploreFolders(rootFolder);

std(trackerLags2)
mean(trackerLags2)

hold on;
scatter(1:length(trackerLags2), trackerLags2, 'r');
meanTrackerLag = mean(trackerLags2);
hold on;
plot([0,length(trackerLags)], [ meanTrackerLag, meanTrackerLag], 'r');


legend('Baseline Study Day One','Baseline Study Day One (Mean)','Baseline Study Day Two','Baseline Study Day Two (Mean)')

title('Temporal Calibration March 28 2012')