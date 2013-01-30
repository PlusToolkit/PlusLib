## PlotStepperCalibrationError
## ---------------------------
## Create an LRE error histogram for stepper calibration  
##
## gnuplot.exe -e "f='TranslationAxisCalibrationError.txt'; o='TranslationAxisCalibrationError'; w=1" PlotStepperCalibrationError.gnu 
## 
## Input arguments: 
##    f='TranslationAxisCalibrationError.txt'    => auto generated file
##	  o='TranslationAxisCalibrationError' => Output file name prefix
##    w=1 						                 => wire number
##--------------------------------------------------------

## Set the terminal mode 
set terminal jpeg

## Output name
# w1x_TranslationAxisCalibrationError.jpg
# 'w'.w."x_".o.".jpg"
# w1y_TranslationAxisCalibrationError.jpg
# 'w'.w."y_".o.".jpg" 


## Set the plot properties 
set key bmargin vertical Right noreverse autotitle nobox
set datafile missing '#'
set grid 

## Set the x axis range
#set xrange [-80:-10]

# Create LUT for column position 
if (w==1) c = 2; \
else if (w==3) c = 6; \
else if (w==4) c = 10; \
else if (w==6) c = 14; \
else if (w==7) c = 18; \
else if (w==9) c = 22; \
else exit -1; 


## Plot the error
set title "Wire#".w." X error" ; \
set output 'w'.w."x_".o.".jpg"; \
plot f using 1:c with points t columnhead, f using 1:c+2 with lines t columnhead; \
pause -1; \
set title "Wire#".w." Y error" ; \
set output 'w'.w."y_".o.".jpg"; \
plot f using 1:c+1 with points t columnhead, f using 1:c+3 with lines t columnhead ; 
	
unset multiplot     

## This will keep the graph on the screen and wait for return after
pause -1