testExecutableFullpathName = '<where the executable is located>';
intermediateFileOutputRootDirectory = horzcat('<where the intermediate files--e.g. images--should be written>');
rootDirectory = '<directory containing the trial folders to be tested>'

verboseLevel = 3
samplingResolutionSec = 0.001

% Folders that contain the calibration sequences to be tested
testDirectoriesBaseline={
'Trial01',
'Trial02',
'Trial03',
'Trial04',
'Trial05',
'Trial06',
'Trial07',
'Trial08',
'Trial09',
'Trial10'}';

TemporalCalibrationAlgoTest(rootDirectory, testExecutableFullpathName, verboseLevel, samplingResolutionSec,testDirectoriesBaseline,intermediateFileOutputRootDirectory);
