## PlotProbeCalibrationError3D
## ---------------------------
## Create a 3D reprojection error plot for probe calibration  
##
## gnuplot.exe -e "f='ProbeCalibrationReport.txt'; o='ProbeCalibrationError3D'; w=2" PlotProbeCalibrationError3D.gnu 
## 
## Input arguments: 
##    f='ProbeCalibrationReport.txt'    => auto generated file
##	  o='ProbeCalibrationError3D' => Output file name prefix
##    w=1 						                 => wire number
##--------------------------------------------------------

## Set the terminal mode 
set terminal jpeg

## Output name
# w1_ProbeCalibrationError3D.jpg
# 'w'.w."_".o.".jpg"

## Set the plot properties 
set key bmargin vertical Right noreverse autotitle nobox
set datafile missing '#'
set grid 

## Set the x axis range
#set xrange [-80:-10]

# Create LUT for column position 
p = 2; 
if (w==2) c = 8; \
else if (w==5) c = 15; \
else if (w==8) c = 22; \
else exit -1; 


## Plot the error
set title "Wire#".w." 3D error (mm)" ; \
set output 'w'.w."_".o.".jpg"; \
plot f using p:c with points t columnhead;
	
## This will keep the graph on the screen and wait for return after
pause -1