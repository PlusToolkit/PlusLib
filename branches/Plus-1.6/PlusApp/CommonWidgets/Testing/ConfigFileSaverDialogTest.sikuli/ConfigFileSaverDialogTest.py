import sys

#------------------------------------------------------------------------------

# Assembles the application executable command properly according to the script's arguments
def getCommandFromArguments(plusDir=''):
  if len(sys.argv) > 1:
    exe = sys.argv[1]
    
    # Concatenate the command line arguments to the executable name
    for argnum in range(2,len(sys.argv)):
      exe = exe + ' "' + sys.argv[argnum] + '"'
  else:
    # Temporary setting of the executable (for trying in Sikuli IDE)
    if plusDir == '':
      exe = ''

    exe = '"' + plusDir + '/bin/Release/SegmentationParameterDialogTest" "--device-set-configuration-dir=' + plusDir + '/PlusLib/data/ConfigFiles/Testing" "--input-config-file-name=' + plusDir + '/PlusLib/data/ConfigFiles/Testing/PlusConfiguration_SegmentationParameterDialog_SavedDataset_fCal_1.0_VideoOnly.xml" "--image-dir=' + plusDir + '/PlusLib/data/TestImages" --verify-saved-configuration-file'
  
  return exe

#------------------------------------------------------------------------------

from os import getenv
from shutil import copyfile
from datetime import datetime

# Define function that is run when error occurs
def captureScreenAndExit():
  capturedFileName = capture(SCREEN)
  newFileName = getenv('APPDATA') + '/' + appTitle + datetime.now().strftime("_%Y%m%d_%H%M%S") + '.png'
  copyfile(capturedFileName, newFileName)
  print "The screen capture of the application in time of the error was saved to: " + newFileName
  closeApp(appTitle)
  sys.exit(1)

#------------------------------------------------------------------------------

# Tries to find application on all monitors
def findApplicationOnAllMonitors(pattern, maxNumberOfTries=20):
  for count in range (maxNumberOfTries):
    for screenId in range(SCREEN.getNumberScreens()):
      screen = Screen(screenId)

      try:
        foundRegion = screen.find(pattern)
      except FindFailed:
        continue

      print "Application found on screen ", screenId, " after ", count, " tries"
      return foundRegion

  print "[ERROR] Application cannot be found on any screen after ", maxNumberOfTries , "tries!"
  captureScreenAndExit()

#------------------------------------------------------------------------------
# Main program

exe = getCommandFromArguments("c:/devel/Plus/Plus-bin")

appTitle = "SegmentationParameterDialogTest"

testApp = App.open(exe)

connectButton = findApplicationOnAllMonitors("ConnectButton.png")

App.focus(appTitle)

# Get the region of the segmentation parameter dialog window
applicationTopLeft = connectButton.getTopLeft()
applicationTopLeft = applicationTopLeft.left(205).above(295)
windowRegion = Region(applicationTopLeft.x, applicationTopLeft.y, 495, 355)
messageBoxRegion = Region(applicationTopLeft.x - 15, applicationTopLeft.y - 195, 510, 550)

# Connect to the device set (it is already selected because of the command line arguments)
click(connectButton)

try:
  increaseLinePairDistanceButton = wait(Pattern("LinePairDistanceParameterRow.png").targetOffset(96,-3), 10)
except FindFailed:
  print "[ERROR] Segmentation parameter dialog did not appear (cannot find Line pair distance parameter)!"
  captureScreenAndExit()

# Change a parameter and close the window
click(increaseLinePairDistanceButton)
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
  descriptionField = windowRegion.find(Pattern("DescriptionLabel.png").targetOffset(-21,16))
except FindFailed:
  print "[ERROR] Cannot find description field!"
  captureScreenAndExit()

# Select the whole description and type its new value
click(descriptionField)
type('a', KeyModifier.CTRL)

type("ConfigFileSaverDialogTest result with changed line pair distance tolerance value 11.0")

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
wait(5)

# Check if errors occurred
import java.awt.Robot as JRobot
colorPickerRobot = JRobot()

try:
  statusIcon = windowRegion.find("GreenStatusIcon.png")
except FindFailed:
  print "[ERROR] Cannot find green StatusIcon!"
  captureScreenAndExit()

statusIconCenter = statusIcon.getCenter()
color = colorPickerRobot.getPixelColor(statusIconCenter.x, statusIconCenter.y)

if color.getGreen() < color.getRed() or color.getGreen() < color.getBlue():
  print "[ERROR] StatusIcon is not green (", color, ")"
  captureScreenAndExit()
  
closeApp(appTitle) # close the window - stop the process
