function TemporalCalibrationAlgoTest(rootDirectory, testExecutableFullpathName, verboseLevel, samplingResolutionSec, testDirectories,intermediateFileOutputRootDirectory, videoBufferFilename, trackerBufferFilename)

if nargin < 7 
	videoBufferFilename='BufferVideo.mha';
end
if nargin < 8
	trackerBufferFilename='BufferTracker.mha';
end

sz = size(testDirectories,2);
for i = 1 : sz(1)
    currTestLocalDirectory = char(testDirectories(i));
    disp('-----------------------------------');
    disp(['Running temporal calibration test on: ', currTestLocalDirectory]);
    currTestDirectory = horzcat(rootDirectory, '\', currTestLocalDirectory);
    currFileOutputRootDirectory = horzcat(intermediateFileOutputRootDirectory, '\', currTestLocalDirectory);
    mkdir(currFileOutputRootDirectory);
    dosTestCommand = horzcat(testExecutableFullpathName, ' '); 
    dosTestCommand = horzcat(dosTestCommand, '--input-video-sequence-metafile=', currTestDirectory, '\',videoBufferFilename,' '); 
    dosTestCommand = horzcat(dosTestCommand, '--input-tracker-sequence-metafile=', currTestDirectory, '\',trackerBufferFilename,' ');
    dosTestCommand = horzcat(dosTestCommand, '--verbose=', num2str(verboseLevel),' ');
    dosTestCommand = horzcat(dosTestCommand, '--sampling-resolution-sec=', num2str(samplingResolutionSec), ' ');
    dosTestCommand = horzcat(dosTestCommand, '--intermediate-file-output-directory=','"', currFileOutputRootDirectory, '" ');
%     dosTestCommand = horzcat(dosTestCommand, '--save-intermediate-images'); % Uncomment to write intermediate files
    dosTestCommand = horzcat(dosTestCommand, '--plot-results'); 
    dosTestCommand
    dos(dosTestCommand);
end

