## PlotProbeRotationAxisCalibrationError
## -------------------------------------
## Create an LRE error graph for stepper rotation axis calibration  
##
## gnuplot.exe -e "f='ProbeRotationAxisCalibrationError.txt'; o='ProbeRotationAxisCalibrationError';" PlotProbeRotationAxisCalibrationError.gnu 
## 
## Input arguments: 
##    f='ProbeRotationAxisCalibrationError.txt'     => auto generated file
##	  o='ProbeRotationAxisCalibrationError'         => Output file name prefix
##--------------------------------------------------------

## Set the terminal mode 
set terminal jpeg

## Output name
# x_ProbeRotationAxisCalibrationError.jpg
# "x_".o.".jpg"

## Set the plot properties 
set key bmargin vertical Right noreverse autotitle nobox
set datafile missing '#'
set grid 


## Plot the errors  
set title "Stepper rotation axis calibration error - X" ; \
set output "x_".o.".jpg"; \
plot f using 1:2 with points t columnhead, f using 1:4 with lines t columnhead; \
pause -1; \

set title "Stepper rotation axis calibration error - Y" ; \
set output "y_".o.".jpg"; \
plot f using 1:3 with points t columnhead, f using 1:5 with lines t columnhead ; \
	
unset multiplot     

## This will keep the graph on the screen and wait for return after
pause -1