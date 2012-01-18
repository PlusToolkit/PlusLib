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

    exe = '"' + plusDir + 'bin/Release/SegmentationParameterDialogTest" "--device-set-configuration-directory-path=' + plusDir + '/PlusLib/data/ConfigFiles" "--input-config-file-name=' + plusDir + '/PlusLib/data/ConfigFiles/Test_PlusConfiguration_SegmentationParameterDialog_SavedDataset_fCal_1.0_VideoOnly.xml" "--image-directory-path=' + plusDir + '/PlusLib/data/TestImages"'
  
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

exe = getCommandFromArguments("D:/devel/PlusBuild-bin")

appTitle = "SegmentationParameterDialogTest"

testApp = App.open(exe)

connectButton = findApplicationOnAllMonitors("ConnectButton.png")

App.focus(appTitle)

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

windowRegion.click(freezeButton)

try:
  windowRegion.wait("GreenDot.png", 20)
except FindFailed:
  print "[ERROR] Cannot segment!"
  captureScreenAndExit()

closeApp(appTitle) # close the window - stop the process
