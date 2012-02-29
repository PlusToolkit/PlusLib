#include <Inventor/Win/SoWin.h>
#include <Inventor/Win/viewers/SoWinExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/SODB.h>
#include <Inventor/SoInput.h>
#include <iostream>

int main(int argc, char **argv)
{
  if(argc!=2) {
    std::cout<<"Usage: "<<argv[0]<<" sceneFileName.iv\n";
    return EXIT_FAILURE;
  }

                    //initialize window with an examiner viewer, 
                    //white background
  HWND window = SoWin::init(argv[0]);
  if (window==NULL) 
    return EXIT_FAILURE;

  SoWinExaminerViewer * viewer = new SoWinExaminerViewer(window);
  viewer->setBackgroundColor(SbColor(1.0,1.0,1.0));
                 //open the scene file and load it into the data base
  SoInput in;
  if(!in.openFile(argv[1])) {
    std::cerr<<"Error opening input file ("<<argv[1]<<"), exiting.\n";
    return EXIT_FAILURE;
  }

  SoSeparator * root = SoDB::readAll(&in);
  if(!root) { 
    std::cerr<<"Error reading input file ("<<argv[1]<<"), exiting.\n";
    return EXIT_FAILURE;
  }
                  //set the viewer's scene graph to the root of the graph
                  //we just read
  viewer->setSceneGraph(root);
  viewer->show();
             //show window and run the event loop
  SoWin::show(window);
  SoWin::mainLoop();
           //cleanup and exit
  delete viewer;
  root->unref();
  return EXIT_SUCCESS;
}