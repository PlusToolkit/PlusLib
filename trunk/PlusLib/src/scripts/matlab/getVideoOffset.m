function videoOffset=getVideoOffset(videoCsvFile, trackerMhaFile)

% Computes video offset from video and tracker data acquired from a
% calibration phantom, using a motorized positioner

videoOffset=0.0; % second
maxVideoOffset=3.0; % second
videoOffsetResolution=0.001; %second

videoTimestampPosition=csvread(videoCsvFile,2,1);
videoPos=videoTimestampPosition(:,2);
videoTimestamp=videoTimestampPosition(:,1);
videoPosNormalized=(videoPos-mean(videoPos))/std(videoPos);

[trackerFilteredTimestamp trackerUnfilteredTimestamp trackerFrameIndex trackerPosition]=readmhatimestamp(trackerMhaFile);
trackerPos=trackerPosition(:,8);
trackerTimestamp=trackerFilteredTimestamp;
trackerPosNormalized=(trackerPos-mean(trackerPos))/std(trackerPos);

figure(1);
plot(trackerTimestamp, trackerPosNormalized, videoTimestamp-videoOffset, videoPosNormalized);
title('Without video offset');

% Find alignment between the tracker and video

commonRangeMin=max([min(videoTimestamp); min(trackerTimestamp)]);
commonRangeMax=min([max(videoTimestamp); max(trackerTimestamp)]);

correlationEvaluationTimePoints = commonRangeMin:videoOffsetResolution:commonRangeMax;

videoPosNormalizedResampled=spline(videoTimestamp, videoPosNormalized, correlationEvaluationTimePoints);
trackerPosNormalizedResampled=spline(trackerTimestamp, trackerPosNormalized, correlationEvaluationTimePoints);
maxLag=maxVideoOffset/videoOffsetResolution;

[C, lags] = xcorr(videoPosNormalizedResampled, trackerPosNormalizedResampled, maxLag); % this will center the plot

loc = find(C==max(C));
videoOffset = lags(loc)*videoOffsetResolution;

figure(2);
plot(trackerTimestamp, trackerPosNormalized, videoTimestamp-videoOffset, videoPosNormalized);
title('With video offset');

end
