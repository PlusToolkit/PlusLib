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

## Plot the histogram  
if (w==1) \
	set title "Wire#1 X error" ; \
	set output 'w'.w."x_".o.".jpg"; \
	plot f using 1:2 with points t columnhead, f using 1:10 with lines t columnhead; \
	pause -1; \
	set title "Wire#1 Y error" ; \
	set output 'w'.w."y_".o.".jpg"; \
	plot f using 1:3 with points t columnhead, f using 1:11 with lines t columnhead ; \
else if (w==3) \
	set title "Wire#3 X error" ; \
	set output 'w'.w."x_".o.".jpg"; \
	plot f using 1:4 with points t columnhead, f using 1:12 with lines t columnhead; \
	pause -1; \
	set title "Wire#3 Y error" ; \
	set output 'w'.w."y_".o.".jpg"; \
	plot f using 1:5 with points t columnhead, f using 1:13 with lines t columnhead ; \
else if (w==4) \
	set title "Wire#4 X error" ; \
	set output 'w'.w."x_".o.".jpg"; \
	plot f using 1:6 with points t columnhead, f using 1:14 with lines t columnhead; \
	pause -1; \
	set title "Wire#4 Y error" ; \
	set output 'w'.w."y_".o.".jpg"; \
	plot f using 1:7 with points t columnhead, f using 1:15 with lines t columnhead ; \
else if (w==6) \
	set title "Wire#6 X error" ; \
	set output 'w'.w."x_".o.".jpg"; \
	plot f using 1:8 with points t columnhead, f using 1:16 with lines t columnhead; \
	pause -1; \
	set title "Wire#6 Y error" ; \
	set output 'w'.w."y_".o.".jpg"; \
	plot f using 1:9 with points t columnhead, f using 1:17 with lines t columnhead ; \
else \
	set title "Wire#1 X error" ; \
	set output 'w'.w."x_".o.".jpg"; \
	plot f using 1:2 with points t columnhead, f using 1:10 with lines t columnhead; \
	pause -1; \
	set title "Wire#1 Y error" ; \
	set output 'w'.w."y_".o.".jpg"; \
	plot f using 1:3 with points t columnhead, f using 1:11 with lines t columnhead ; \

	
unset multiplot     

## This will keep the graph on the screen and wait for return after
pause -1