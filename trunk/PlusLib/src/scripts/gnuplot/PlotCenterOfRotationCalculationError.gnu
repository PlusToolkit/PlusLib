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

## Plot the errors  
if (w==1) \
	set title "Wire #1 Distance From Transducer" ; \
    set origin 0.0,0.5; \
	plot f using 1:4 with points t "Distance on different positions"; \
    set origin 0.0,0.0; \
	plot f using 2:4 with points t "Distance on different angles"; \
else if (w==3) \
	set title "Wire #3 Distance From Transducer" ; \
    set origin 0.0,0.5; \
	plot f using 1:5 with points t "Distance on different positions"; \
    set origin 0.0,0.0; \
	plot f using 2:5 with points t "Distance on different angles"; \
else if (w==4) \
	set title "Wire #4 Distance From Transducer" ; \
    set origin 0.0,0.5; \
	plot f using 1:6 with points t "Distance on different positions"; \
    set origin 0.0,0.0; \
	plot f using 2:6 with points t "Distance on different angles"; \
else if (w==6) \
	set title "Wire #6 Distance From Transducer" ; \
    set origin 0.0,0.5; \
	plot f using 1:7 with points t "Distance on different positions"; \
    set origin 0.0,0.0; \
	plot f using 2:7 with points t "Distance on different angles"; \

unset multiplot     

## This will keep the graph on the screen and wait for return after
pause -1

