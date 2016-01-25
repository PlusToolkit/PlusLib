function videoOffset=getVideoOffset(videoCsvFile, trackerMhaFiles)

% Display file name
disp(videoCsvFile)

% Computes video offset from video and tracker data acquired from a
% calibration phantom, using a motorized positioner

videoOffset=0.0; % second
maxVideoOffset=2.0; % second
videoOffsetResolution=0.001; %second

videoTimestampPosition=csvread(videoCsvFile,2,1);
videoPos=videoTimestampPosition(:,2);
videoTimestamp=videoTimestampPosition(:,1);
videoPosNormalized=(videoPos-mean(videoPos))/std(videoPos);

[trackerFilteredTimestamp trackerUnfilteredTimestamp trackerFrameIndex trackerPosition]=readmhatimestamp(trackerMhaFiles(1,:)); 
for file=2:size(trackerMhaFiles, 1)
    [tfts tufts tfi tp]=readmhatimestamp(trackerMhaFiles(file,:));
    trackerFilteredTimestamp = [trackerFilteredTimestamp; tfts]; 
    trackerUnfilteredTimestamp = [ trackerUnfilteredTimestamp; tufts]; 
    trackerFrameIndex = [trackerFrameIndex; tfi]; 
    trackerPosition = [trackerPosition; tp]; 
end
trackerPos=trackerPosition(:,8);
trackerTimestamp=trackerFilteredTimestamp;
trackerPosNormalized=(trackerPos-mean(trackerPos))/std(trackerPos);

figure(1);
plot(trackerTimestamp, trackerPosNormalized, videoTimestamp-videoOffset, videoPosNormalized);
title([videoCsvFile, ': positions without video offset']);
xlabel('time (s)');

% Find alignment between the tracker and video

commonRangeMin=max([min(videoTimestamp); min(trackerTimestamp)]);
commonRangeMax=min([max(videoTimestamp); max(trackerTimestamp)]);

if (commonRangeMin+maxVideoOffset>=commonRangeMax-maxVideoOffset)
    error('Not enough overlapping position and video data for computing video offset');
end

videTimestampResampled = commonRangeMin:videoOffsetResolution:commonRangeMax;
% Need to use a shorter range for one of the signals to make sure that
% the signals overlap each other in the whole allowed time offset range
% (as Matlab would pad the non-overlapping parts of the signals with zeros,
% which would strongly influence the correlation results by favoring 0 offset)
trackerTimestampResampled = commonRangeMin+maxVideoOffset:videoOffsetResolution:commonRangeMax-maxVideoOffset;

% Need to resample the signals on a uniform grid to allow computation of
% cross correlation. Spline kernel is not stable (results in large fitting
% error) when timestamps are delayed.
videoPosNormalizedResampled=interp1(videoTimestamp, videoPosNormalized, videTimestampResampled, 'pchip');
trackerPosNormalizedResampled=interp1(trackerTimestamp, trackerPosNormalized, trackerTimestampResampled, 'pchip');
maxLag=maxVideoOffset/videoOffsetResolution;

figure(3);
plot(videTimestampResampled, videoPosNormalizedResampled, ':',  trackerTimestampResampled, trackerPosNormalizedResampled, '-');
title([videoCsvFile, ': Resampled video and tracker positions']);
xlabel('time (s)');

[C, lags] = xcorr(videoPosNormalizedResampled, trackerPosNormalizedResampled, maxLag*2); % this will center the plot

figure(4);
plot(lags*videoOffsetResolution-maxVideoOffset, C);
title([videoCsvFile, ': Cross-correlation']);
xlabel('video offset (s)');

loc = find(C==max(C));
videoOffset = lags(loc)*videoOffsetResolution-maxVideoOffset;

figure(2);
plot(trackerTimestamp, trackerPosNormalized, videoTimestamp-videoOffset, videoPosNormalized);
title([videoCsvFile, ': with video offset = ', num2str(videoOffset)]);
xlabel('time (s)');

end
