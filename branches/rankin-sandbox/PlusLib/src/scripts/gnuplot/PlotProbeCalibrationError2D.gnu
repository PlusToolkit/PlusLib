## PlotProbeCalibrationError2D
## ---------------------------
## Create a 2D reprojection error plot for probe calibration  
##
## gnuplot.exe -e "f='ProbeCalibrationReport.txt'; o='ProbeCalibrationError2D'; w=1" PlotProbeCalibrationError2D.gnu 
## 
## Input arguments: 
##    f='ProbeCalibrationReport.txt'    => auto generated file
##	  o='ProbeCalibrationError2D' => Output file name prefix
##    w=1 						                 => wire number
##--------------------------------------------------------

## Set the terminal mode 
set terminal jpeg

## Output name
# w1x_ProbeCalibrationError2D.jpg
# 'w'.w."x_".o.".jpg"
# w1y_ProbeCalibrationError2D.jpg
# 'w'.w."y_".o.".jpg" 


## Set the plot properties 
set key bmargin vertical Right noreverse autotitle nobox
set datafile missing '#'
set grid 

## Set the x axis range
#set xrange [-80:-10]

# Create LUT for column position 
p = 2; 
if (w==1) c = 4; \
else if (w==2) c = 6; \
else if (w==3) c = 9; \
else if (w==4) c = 11; \
else if (w==5) c = 13; \
else if (w==6) c = 16; \
else if (w==7) c = 18; \
else if (w==8) c = 20; \
else if (w==9) c = 23; \
else exit -1; 


## Plot the error
set title "Wire#".w." X error (px)" ; \
set output 'w'.w."x_".o.".jpg"; \
plot f using p:c with points t columnhead; \
pause -1; \
set title "Wire#".w." Y error (px)" ; \
set output 'w'.w."y_".o.".jpg"; \
plot f using p:c+1 with points t columnhead; 
	
unset multiplot     

## This will keep the graph on the screen and wait for return after
pause -1