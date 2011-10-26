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

    exe = '"' + plusDir + '/PlusApp-bin/DevEnv_Release.bat" "--launch" "' + plusDir + '/PlusApp-bin/bin/Release/SegmentationParameterDialogTest" "--device-set-configuration-directory-path=' + plusDir + '/PlusLib/data/ConfigFiles" "--input-config-file-name=' + plusDir + '/PlusLib/data/ConfigFiles/PlusConfiguration_SonixTouch_Ascension3DG_EC9_fCal1.xml" "--image-directory-path=' + plusDir + '/PlusLib/data/TestImages"'
  
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

exe = getCommandFromArguments("D:/devel/Plus-build-bin")

appTitle = "SegmentationParameterDialogTest"

testApp = App.open(exe)
wait(10) # Wait for the application to initialize (else the next wait check does not not run properly)
App.focus(appTitle)

import java.awt.Robot as JRobot
colorPickerRobot = JRobot()

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

# Look for green status icon
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

# Connect to the device set that will fail
click(connectButton)
wait(2)

try:
  windowRegion.wait("ConnectButton.png", 10)
except FindFailed:
  print "[ERROR] Connection did not fail!"
  captureScreenAndExit()

wait(5)

# Verify changed Status icon
try:
  statusIcon = windowRegion.find("RedStatusIcon.png")
except FindFailed:
  print "[ERROR] Cannot find red StatusIcon!"
  captureScreenAndExit()

color = colorPickerRobot.getPixelColor(statusIconCenter.x, statusIconCenter.y)

if color.getRed() < color.getGreen() or color.getRed() < color.getBlue():
  print "[ERROR] StatusIcon is not red (", color, ")"
  captureScreenAndExit()

# Bring up messagebox and verify error message
click(statusIcon)

try:
  messageBoxRegion.wait("ErrorText.png", 10)
except FindFailed:
  print "[ERROR] Cannot find error message in status message box!"
  captureScreenAndExit()

# Click status icon again and verify that it disappeared and it changed back to green
click(statusIcon)

try:
  windowRegion.wait("ConnectButton.png", 10)
except FindFailed:
  print "[ERROR] Message box did not disappear!"
  captureScreenAndExit()

try:
  windowRegion.find("GreenStatusIcon.png")
except FindFailed:
  print "[ERROR] Cannot find green StatusIcon!"
  captureScreenAndExit()

color = colorPickerRobot.getPixelColor(statusIconCenter.x, statusIconCenter.y)

if color.getGreen() < color.getRed() or color.getGreen() < color.getBlue():
  print "[ERROR] StatusIcon is not green (", color, ")"
  captureScreenAndExit()

closeApp(appTitle) # close the window - stop the process
