## PlotCenterOfRotationCalculationError
## -------------------------------------
## Create an LRE error graph for stepper rotation encoder calibration  
##
## gnuplot.exe -e "f='CenterOfRotationCalculationError.txt'; o='CenterOfRotationCalculationError';w=1" PlotCenterOfRotationCalculationError.gnu 
## 
## Input arguments: 
##    f='CenterOfRotationCalculationError.txt'     => auto generated file
##	  o='CenterOfRotationCalculationError'         => Output file name prefix
##    w=1   => wire number
##--------------------------------------------------------

## Set the terminal mode 
set terminal jpeg 
#transparent nocrop enhanced font arial 8 size 640,480 

## Output name
# CenterOfRotationCalculationError.jpg
# o.".jpg"
set output 'w'.w."_".o.".jpg"; 

## Set the plot properties 
set key bmargin vertical Right noreverse autotitle nobox
set datafile missing '#'
set grid 

#set yrange [40:60]

## Enable multiplot mode 
set multiplot
set size 1,0.5; 

# Create LUT for column position 
if (w==1) wireColumn = 4; \
else if (w==3) wireColumn = 8; \
else if (w==4) wireColumn = 12; \
else if (w==6) wireColumn = 16; \
else if (w==7) wireColumn = 20; \
else if (w==9) wireColumn = 24; 

set title "Wire #".w." Distance From Transducer" ;
set origin 0.0,0.5;
plot f using 1:wireColumn with points t "Distance in different positions"; 
set origin 0.0,0.0; 
plot f using 2:wireColumn with points t "Distance in different angles"; 

unset multiplot     

## This will keep the graph on the screen and wait for return after
pause -1
	
