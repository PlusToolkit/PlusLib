function TemporalCalibrationAlgoTest(rootDirectory, testExecutableFullpathName, verboseLevel, samplingResolutionSec, testDirectories,intermediateFileOutputRootDirectory)

sz = size(testDirectories{1});
for i = 1 : sz(1)
    currTestLocalDirectory = char(testDirectories{1}(i));
    disp(strcat('Running temporal calibration test on: ', currTestLocalDirectory));
    currTestDirectory = horzcat(rootDirectory, '\', currTestLocalDirectory);
    currFileOutputRootDirectory = horzcat(intermediateFileOutputRootDirectory, '\', currTestLocalDirectory);
    mkdir(currFileOutputRootDirectory)
    dosTestCommand = horzcat(testExecutableFullpathName, ' '); 
    dosTestCommand = horzcat(dosTestCommand, '--input-video-sequence-metafile=', currTestDirectory, '\RawVideoBuffer.mha ');
    dosTestCommand = horzcat(dosTestCommand, '--input-tracker-sequence-metafile=', currTestDirectory, '\RawTrackerBuffer.mha ');
    dosTestCommand = horzcat(dosTestCommand, '--verbose=', num2str(verboseLevel));
    dosTestCommand = horzcat(dosTestCommand, '--sampling-resolution-sec=', num2str(samplingResolutionSec), ' ');
    dosTestCommand = horzcat(dosTestCommand, '--intermediate-file-output-directory=','"', currFileOutputRootDirectory, '" ');
    dosTestCommand = horzcat(dosTestCommand, '--save-intermediate-images ');
    dosTestCommand = horzcat(dosTestCommand, '--plot-results');
    dos(dosTestCommand);
end

