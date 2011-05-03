## PlotStepperCalibrationErrorHistogram
## -------------------------------------
## Create an LRE error histogram for stepper calibration  
##
## gnuplot.exe -e "f='TranslationAxisCalibrationError.txt'; o='TranslationAxisCalibrationErrorHistogram'; w=1" PlotStepperCalibrationErrorHistogram.gnu 
## 
## Input arguments: 
##    f='TranslationAxisCalibrationError.txt'    => auto generated file
##	  o='TranslationAxisCalibrationErrorHistogram' => Output file name prefix
##    w=1 						                 => wire number
##--------------------------------------------------------

## Set the terminal mode 
set terminal jpeg
# w1_TranslationAxisCalibrationErrorHistogram.jpg
set output "w".w."_".o.".jpg"

## Set the histogram bin width size
binwidth=0.1

## Set the plot properties 
set key bmargin vertical Right noreverse noautotitle nobox
set boxwidth binwidth absolute  
set style data histograms  
set style fill solid 1.0 border -1 
set grid 

## Set the x axis range
set xrange [-2:2]

## Enable multiplot mode 
set multiplot
set size 1,0.5; 

# Function to generate histogram  
bin(x,width)=width*floor(x/width)

## Plot the histogram  
if (w==1) \
	set title "Wire#1 X histogram" ; \
	set origin 0.0,0.5; \
	plot f using (bin(($2 - $10),binwidth)):(1.0) smooth freq with boxes; \
	set title "Wire#1 Y histogram" ; \
	set origin 0.0,0.0; \
	plot f using (bin(($3 - $11),binwidth)):(1.0) smooth freq with boxes; \
else if (w==3) \
	set title "Wire#3 X histogram" ; \
	set origin 0.0,0.5; \
	plot f using (bin(($4 - $12),binwidth)):(1.0) smooth freq with boxes; \
	set title "Wire#3 Y histogram" ; \
	set origin 0.0,0.0; \
	plot f using (bin(($5 - $13),binwidth)):(1.0) smooth freq with boxes; \
else if (w==4) \
	set title "Wire#4 X histogram" ; \
	set origin 0.0,0.5; \
	plot f using (bin(($6 - $14),binwidth)):(1.0) smooth freq with boxes; \
	set title "Wire#4 Y histogram" ; \
	set origin 0.0,0.0; \
	plot f using (bin(($7 - $15),binwidth)):(1.0) smooth freq with boxes; \
else if (w==6) \
	set title "Wire#6 X histogram" ; \
	set origin 0.0,0.5; \
	plot f using (bin(($8 - $16),binwidth)):(1.0) smooth freq with boxes; \
	set title "Wire#6 Y histogram" ; \
	set origin 0.0,0.0; \
	plot f using (bin(($9 - $17),binwidth)):(1.0) smooth freq with boxes; \
else \
	set title "Wire#1 X histogram" ; \
	set origin 0.0,0.5; \
	plot f using (bin(($2 - $10),binwidth)):(1.0) smooth freq with boxes; \
	set title "Wire#1 Y histogram" ; \
	set origin 0.0,0.0; \
	plot f using (bin(($3 - $11),binwidth)):(1.0) smooth freq with boxes; 

	
unset multiplot     

## This will keep the graph on the screen and wait for return after
pause -1