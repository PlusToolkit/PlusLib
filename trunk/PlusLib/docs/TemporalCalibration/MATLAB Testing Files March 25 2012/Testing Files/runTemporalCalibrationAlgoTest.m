rootDirectory = 'S:\data\TemporalTrackedUSCalibration\CARS_DATA';
testExecutableFullpathName = 'C:\devel\PlusExperimental-bin\bin\Release\TemporalCalibrationAlgoTest.exe';
verboseLevel = 3;
samplingResolutionSec = 0.001;
intermediateFileOutputRootDirectory = 'C:\Documents and Settings\moult\My Documents\CalibrationResults_March_23_2012';

%testFolders = dlmread('C:\Documents and Settings\moult\My Documents\TemporalCalibrationTestNames.txt');
fid = fopen('C:\Documents and Settings\moult\My Documents\TemporalCalibrationTestNames.txt');
testDirectories = textscan(fid, '%q');
fclose(fid);


TemporalCalibrationAlgoTest(rootDirectory, testExecutableFullpathName, verboseLevel, samplingResolutionSec,testDirectories,intermediateFileOutputRootDirectory);