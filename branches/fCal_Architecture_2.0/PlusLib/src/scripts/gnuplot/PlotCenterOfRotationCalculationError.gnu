## PlotCenterOfRotationCalculationError
## -------------------------------------
## Create an LRE error graph for stepper rotation encoder calibration  
##
## gnuplot.exe -e "f='CenterOfRotationCalculationError.txt'; o='CenterOfRotationCalculationError';" PlotCenterOfRotationCalculationError.gnu 
## 
## Input arguments: 
##    f='CenterOfRotationCalculationError.txt'     => auto generated file
##	  o='CenterOfRotationCalculationError'         => Output file name prefix
##--------------------------------------------------------

## Set the terminal mode 
set terminal jpeg transparent nocrop enhanced font arial 8 size 640,480 

## Output name
# CenterOfRotationCalculationError.jpg
# o.".jpg"

## Set the plot properties 
#set key bmargin vertical Right noreverse autotitle nobox
set datafile missing '#'
#set grid 
set parametric
set hidden3d offset 1 trianglepattern 3 undefined 1 altdiagonal bentover
set ztics border in scale 1,0.5 nomirror norotate  offset character 0, 0, 0 -1.00000,0.25,1.00000


## Plot the errors  
set title "Stepper rotation encoder calibration" ; \
set output o.".jpg"; \
splot f using 1:2:4 
pause -1; \

## This will keep the graph on the screen and wait for return after
pause -1

