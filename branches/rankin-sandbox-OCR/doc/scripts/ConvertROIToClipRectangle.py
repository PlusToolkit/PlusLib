import os

def getInfos(currentDir):
  for root, dirs, files in os.walk(currentDir): # Walk directory tree
    #dirs[:] = [os.path.join(root, d) for d in dirs]
    files = [os.path.join(root, f) for f in files]
    for f in files:
      if f.find(".xml") != -1:
        os.rename( f, f+"~" )
        destination = open( f, "w" )
        source = open( f+"~", "r" )
        for line in source:
          if line.find("RegionOfInterest") != -1:
            whitespaces = len(line) - len(line.lstrip(' '))
            prepend = ""
            for w in range(whitespaces):
              prepend += " "
            first = line.find("\"")
            second = line.rfind("\"")
            nums = line[first+1:second]
            result = nums.split()
            xorig = int(result[0])
            yorig = int(result[1])
            xsize = int(result[2]) - int(result[0])
            ysize = int(result[3]) - int(result[1])
            
            destination.write( prepend + "ClipRectangleOrigin=\"" + str(xorig) + " " + str(yorig) + "\" \n" )
            destination.write( prepend + "ClipRectangleSize=\"" + str(xsize) + " " + str(ysize) + "\" \n" )
          else:
            destination.write( line )
        source.close()
        destination.close()
        os.unlink(f+"~")
  return

if __name__ == '__main__':
    getInfos("C:\devel\PlusBuild-bin-x32\PlusLib\Data\ConfigFiles")