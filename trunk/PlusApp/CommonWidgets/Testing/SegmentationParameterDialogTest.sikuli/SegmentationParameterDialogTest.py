import sys

if len(sys.argv) > 1:
  exe = sys.argv[1]
  
  # Concatenate the command line arguments to the executable name
  for argnum in range(2,len(sys.argv)):
    if argnum == 2:
      exe = exe + ' ' + sys.argv[argnum]
    else:
      exe = exe + ' "' + sys.argv[argnum] + '"'
else:
  # Temporary setting of the executable (for trying in Sikuli IDE)
  plusDir = "D:/devel/Plus-bin_Assembla"
  exe = '"' + plusDir + '/PlusApp-bin/DevEnv_Release.bat" "--launch" "' + plusDir + '/PlusApp-bin/bin/Release/SegmentationParameterDialogTest" "--device-set-configuration-directory-path=' + plusDir + '/PlusLib/data/ConfigFiles" "--input-config-file-name=' + plusDir + '/PlusLib/data/ConfigFiles/Test_PlusConfiguration_SegmentationParameterDialog_SavedDataset_fCal_1.0_VideoOnly.xml" "--image-directory-path=' + plusDir + '/PlusLib/data/TestImages"'

appTitle = "SegmentationParameterDialogTest"

testApp = App.open(exe)
wait(2) # Wait for the application to initialize (else the next wait check does not not run properly)

try:
  wait("1318969886531.png")
except FindFailed:
  print "[ERROR] Application did not start!"
  closeApp(appTitle)
  exit

# Connect to the device set (it is already selected because of the command line arguments)
click("1318969886531.png")

try:
  wait("APPlYdCk7se.png", 10)
except FindFailed:
  print "[ERROR] Connection failed!"
  closeApp(appTitle)
  exit

try:
  wait("1319144734747.png", 20)
except FindFailed:
  print "[ERROR] Cannot segment!"
  closeApp(appTitle)
  exit

click("APPlYdCk7se.png")

closeApp(appTitle) # close the window - stop the process
