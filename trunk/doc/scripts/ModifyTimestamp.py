import os

def modifyTimestamp(f):
  os.rename( f, f+"~" )
  destination = open( f, "w" )
  source = open( f+"~", "r" )
  for line in source:
    if line.find("_Timestamp = ") != -1:
      indexOfSeq = line.find("Seq_Frame")      
      frameNum = int(line[indexOfSeq+9:indexOfSeq+13])
      if frameNum < 10:
        frameNumStr = '000{0}'.format(str(frameNum))
      else:
        frameNumStr = '00{0}'.format(str(frameNum))
      newString = "Seq_Frame" + frameNumStr + "_Timestamp = " + str(frameNum*0.5) + "\n"
            
      destination.write(newString)
    else:
      destination.write(line)
  source.close()
  destination.close()
  os.unlink(f+"~")
  return

if __name__ == '__main__':
  modifyTimestamp("C:\devel\PlusBuild-bin-x32\PlusLib\data\TestImages\SingleWallCalibrationTest.mha")