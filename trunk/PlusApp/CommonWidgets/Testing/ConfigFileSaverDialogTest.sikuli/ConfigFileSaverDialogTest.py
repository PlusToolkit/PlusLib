import sys

#------------------------------------------------------------------------------

# Assembles the application executable command properly according to the script's arguments
def getCommandFromArguments(plusDir=''):
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
    if plusDir == '':
      exe = ''

    exe = '"' + plusDir + '/PlusApp-bin/DevEnv_Release.bat" "--launch" "' + plusDir + '/PlusApp-bin/bin/Release/SegmentationParameterDialogTest" "--device-set-configuration-directory-path=' + plusDir + '/PlusLib/data/ConfigFiles" "--input-config-file-name=' + plusDir + '/PlusLib/data/ConfigFiles/Test_PlusConfiguration_SegmentationParameterDialog_SavedDataset_fCal_1.0_VideoOnly.xml" "--image-directory-path=' + plusDir + '/PlusLib/data/TestImages" --verify-saved-configuration-file'
  
  return exe

#------------------------------------------------------------------------------

# Define function that is run when error occurs
def captureScreenAndExit():
  message = "The screen capture of the application in time of the error was saved to: "
  message += capture(SCREEN)
  print message
  closeApp(appTitle)
  sys.exit(1)
  
#------------------------------------------------------------------------------
# Main program

exe = getCommandFromArguments("D:/devel/Plus-build-bin")

appTitle = "SegmentationParameterDialogTest"

testApp = App.open(exe)
wait(10) # Wait for the application to initialize (else the next wait check does not not run properly)
App.focus(appTitle)

try:
  connectButton = wait("ConnectButton.png", 60)
except FindFailed:
  print "[ERROR] Application did not start!"
  captureScreenAndExit()

# Get the region of the segmentation parameter dialog window
applicationTopLeft = connectButton.getTopLeft()
applicationTopLeft = applicationTopLeft.left(205).above(295)
windowRegion = Region(applicationTopLeft.x, applicationTopLeft.y, 495, 355)
messageBoxRegion = Region(applicationTopLeft.x - 15, applicationTopLeft.y - 195, 510, 550)

# Connect to the device set (it is already selected because of the command line arguments)
click(connectButton)

try:
  increaseLineLengthButton = wait(Pattern("LineLengthParameterRow.png").targetOffset(96,-3), 10)
except FindFailed:
  print "[ERROR] Segmentation parameter dialog did not appear!"
  captureScreenAndExit()

# Change a parameter and close the window
click(increaseLineLengthButton)
click("ApplyAndCloseButton.png")

try:
  saveConfigurationButton = windowRegion.wait("SaveConfigurationButton.png", 10)
except FindFailed:
  print "[ERROR] Main dialog did not re-appear!"
  captureScreenAndExit()

click(saveConfigurationButton)
wait(2)

# Find device set name field (wait for it to appear)
try:
  deviceSetNameField = windowRegion.wait(Pattern("DeviceSetNameLabel.png").targetOffset(36,17), 10)
except FindFailed:
  print "[ERROR] Config file saver dialog did not appear!"
  captureScreenAndExit()

# Select the whole name and type its new value
click(deviceSetNameField)
type('a', KeyModifier.CTRL)

type("TEST ConfigFileSaverDialogTest Result")

# Find description field
try:
  descriptionField = windowRegion.find(Pattern("DescriptionLabel.png").targetOffset(15,14))
except FindFailed:
  print "[ERROR] Cannot find description field!"
  captureScreenAndExit()

# Select the whole description and type its new value
click(descriptionField)
type('a', KeyModifier.CTRL)

type("ConfigFileSaverDialogTest result with changed line length tolerance value 11.0")

# Click save button, enter the new file name and save the file
try:
  saveButton = windowRegion.wait("SaveButton.png", 10)
except FindFailed:
  print "[ERROR] Save button in config file saver dialog cannot be found in original window boundaries!"
  captureScreenAndExit()

click(saveButton)
wait(5)
type("TEST_ConfigFileSaverDialogTest_Result")
type(Key.ENTER)

# Check if errors occurred
windowRegion.click("GreenStatusIcon.png") # Checking the red icon was not enough, because it found it even if it was green
wait(2)
if messageBoxRegion.exists("ErrorText.png", 5):
  print "[ERROR] Saved config file does not contain the expected values or other error occurred!"
  captureScreenAndExit()
  
closeApp(appTitle) # close the window - stop the process
