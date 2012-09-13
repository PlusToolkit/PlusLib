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

    exe = '"' + plusDir + '/bin/Release/SegmentationParameterDialogTest.exe " "--device-set-configuration-directory-path=' + plusDir + '/PlusLib/data/ConfigFiles" "--image-directory-path=' + plusDir + '/PlusLib/data/TestImages"'

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
print exe
appTitle = "SegmentationParameterDialogTest"

testApp = App.open(exe)

connectButton = findApplicationOnAllMonitors("ConnectButton.png")

App.focus(appTitle)

# Get the region of the segmentation parameter dialog window
applicationTopLeft = connectButton.getTopLeft()
applicationTopLeft = applicationTopLeft.left(205).above(295)
windowRegion = Region(applicationTopLeft.x, applicationTopLeft.y, 495, 355)
comboBoxRegion = Region(applicationTopLeft.x, applicationTopLeft.y, 495, 600)

# Select a configuration that cannot connect
try:
  comboBoxDropDown = windowRegion.find("DropDownArrow.png")
except FindFailed:
  print "[ERROR] Cannot find device set combobox!"
  captureScreenAndExit()

click(comboBoxDropDown)

try:
  sonixTouchWithAscensionItem = comboBoxRegion.find("DeviceSetItemToFail.png")
except FindFailed:
  print "[ERROR] Cannot find SonixTouch Ascension3DG item!"
  captureScreenAndExit()

click(sonixTouchWithAscensionItem)
click(connectButton)

# Verify failed connection
try:
  windowRegion.wait("ConnectionFailedText.png", 10)
except FindFailed:
  print "[ERROR] Cannot find connection failed error text!"
  captureScreenAndExit()

# Connect to device set that is supposed to work
click(comboBoxDropDown)
wait(1)

try:
  segmentationParameterDialogTestItem = comboBoxRegion.find("DeviceSetItemToConnect.png")
except FindFailed:
  print "[ERROR] Cannot find SegmentationParameterDialogTest item!"
  captureScreenAndExit()

click(segmentationParameterDialogTestItem)
click(connectButton)

try:
  freezeButton = wait("FreezeButton.png", 10)
except FindFailed:
  print "[ERROR] Connection failed!"
  captureScreenAndExit()

closeApp(appTitle) # close the window - stop the process
