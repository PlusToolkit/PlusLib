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

    exe = '"' + plusDir + '/bin/Release/ToolStateDisplayWidgetTest" "--device-set-configuration-directory-path=' + plusDir + '/PlusLib/data/ConfigFiles" "--input-config-file-name=' + plusDir + '/PlusLib/data/ConfigFiles/Test_PlusConfiguration_VideoNone_FakeTracker_ToolState.xml"'
  
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
# Main program

exe = getCommandFromArguments("D:/devel/PlusBuild-bin")

appTitle = "ToolStateDisplayWidgetTest"

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
applicationTopLeft = applicationTopLeft.left(215).above(230)
windowRegion = Region(applicationTopLeft.x, applicationTopLeft.y, 495, 340)

# Connect to the device set (it is already selected because of the command line arguments)
click(connectButton)

# Wait for the status to be OK
try:
  windowRegion.wait("OKLabel.png", 60)
except FindFailed:
  print "[ERROR] Unable to find OK status!"
  captureScreenAndExit()

# Wait for the status to be Out of view
try:
  windowRegion.wait("OutOfViewLabel.png", 60)
except FindFailed:
  print "[ERROR] Unable to find Out of view status!"
  captureScreenAndExit()

# Wait for the status to be Missing
try:
  windowRegion.wait("MissingLabel.png", 60)
except FindFailed:
  print "[ERROR] Unable to find Missing status!"
  captureScreenAndExit()

closeApp(appTitle) # close the window - stop the process
