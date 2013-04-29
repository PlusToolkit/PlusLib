rootFolder = '<folder containing the tests for a given imaging parameter set>';
rootCodeFolder = pwd;

%   Get the tracker lags
[trackerLags, calibrationErrors, maxCalibrationErrors] = exploreFolders(rootFolder, rootCodeFolder);

%   Write the tracker lags, the mean tracker lags, and the standard
%   deviation of the tracker lags to file
xlswrite('PertinentStatistics',[(1:length(trackerLags))', trackerLags']);
xlswrite('PertinentStatistics',mean(trackerLags),'C1:C1');
xlswrite('PertinentStatistics',std(trackerLags),'C2:C2'); 

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
titleString = horzcat('Tracker Lag vs. Trial Number (Depth: 6.0; ', date, ' )');
title(titleString ,'FontWeight','bold')

set(gcf, 'Units', 'Normalized', 'OuterPosition', [0 0 1 1]); 
set(gcf, 'PaperPositionMode', 'auto');
print -depsc2 TrackerLagVsTrialNumber.eps

