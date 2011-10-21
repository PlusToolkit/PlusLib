import sys

# Define function that is run when error occurs
def captureScreenAndExit():
  message = "The screen capture of the application in time of the error was saved to: "
  message += capture(SCREEN)
  print message
  closeApp(appTitle)
  exit
  

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
wait(5) # Wait for the application to initialize (else the next wait check does not not run properly)
    
try:
  connectButton = wait("ConnectButton.png", 60)
except FindFailed:
  print "[ERROR] Application did not start!"
  captureScreenAndExit()

# Connect to the device set (it is already selected because of the command line arguments)
click(connectButton)

try:
  freezeButton = wait("FreezeButton.png", 10)
except FindFailed:
  print "[ERROR] Connection failed!"
  captureScreenAndExit()

# Get the region of the segmentation parameter dialog window
applicationTopLeft = freezeButton.getTopLeft()
applicationTopLeft = applicationTopLeft.left(70).above(35)
windowRegion = Region(applicationTopLeft.x, applicationTopLeft.y, 1000, 600)
capture(windowRegion)

windowRegion.click(freezeButton)

try:
  windowRegion.wait("GreenDot.png", 20)
except FindFailed:
  print "[ERROR] Cannot segment!"
  captureScreenAndExit()

closeApp(appTitle) # close the window - stop the process
