## PlotProbeRotationEncoderCalibrationError
## -------------------------------------
## Create an LRE error graph for stepper rotation encoder calibration  
##
## gnuplot.exe -e "f='ProbeRotationEncoderCalibrationError.txt'; o='ProbeRotationEncoderCalibrationError';" PlotProbeRotationEncoderCalibrationError.gnu 
## 
## Input arguments: 
##    f='ProbeRotationEncoderCalibrationError.txt'     => auto generated file
##	  o='ProbeRotationEncoderCalibrationError'         => Output file name prefix
##--------------------------------------------------------

## Set the terminal mode 
set terminal jpeg

## Output name
# ProbeRotationEncoderCalibrationError.jpg
# o.".jpg"

## Set the plot properties 
set key bmargin vertical Right noreverse autotitle nobox
set datafile missing '#'
set grid 


## Plot the errors  
set title "Stepper rotation encoder calibration" ; \
set output o.".jpg"; \
plot f using 1:($1-$2) with points t "Measured and computed angle difference", f using 1:($1-$3) with lines t "Measured and compensated angle difference"; \
pause -1; \

## This will keep the graph on the screen and wait for return after
pause -1