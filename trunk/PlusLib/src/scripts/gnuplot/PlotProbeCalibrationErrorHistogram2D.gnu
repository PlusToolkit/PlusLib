## PlotProbeCalibrationErrorHistogram2D
## -------------------------------------
## Create an LRE error histogram for stepper calibration  
##
## gnuplot.exe -e "f='ProbeCalibrationReport.txt'; o='ProbeCalibrationErrorHistogram2D'; w=1" PlotProbeCalibrationErrorHistogram2D.gnu 
## 
## Input arguments: 
##    f='ProbeCalibrationReport.txt'    => auto generated file
##	  o='ProbeCalibrationErrorHistogram2D' => Output file name prefix
##    w=1 						                 => wire number
##--------------------------------------------------------

## Set the terminal mode 
set terminal jpeg
# w1_ProbeCalibrationErrorHistogram2D.jpg
set output "w".w."_".o.".jpg"

## Set the histogram bin width size
binwidth=0.3

## Set the plot properties 
set key bmargin vertical Right noreverse noautotitle nobox
set boxwidth binwidth absolute  
set style data histograms  
set style fill solid 1.0 border -1 
set grid 

## Set the x axis range
set xrange [-3:3]

## Enable multiplot mode 
set multiplot
set size 1,0.5; 

# Function to generate histogram  
bin(x,width)=width*floor(x/width)
	
## Plot the histogram  
if (w==1) \
	set title "Wire#1 X 2D error histogram" ; \
	set origin 0.0,0.5; \
	plot f using (bin(($4),binwidth)):(1.0) smooth freq with boxes; \
	set title "Wire#1 Y 2D error histogram" ; \
	set origin 0.0,0.0; \
	plot f using (bin(($5),binwidth)):(1.0) smooth freq with boxes; \
else if (w==2) \
	set title "Wire#2 X 2D error histogram" ; \
	set origin 0.0,0.5; \
	plot f using (bin(($6),binwidth)):(1.0) smooth freq with boxes; \
	set title "Wire#2 Y 2D error histogram" ; \
	set origin 0.0,0.0; \
	plot f using (bin(($7),binwidth)):(1.0) smooth freq with boxes; \
else if (w==3) \
	set title "Wire#3 X 2D error histogram" ; \
	set origin 0.0,0.5; \
	plot f using (bin(($9),binwidth)):(1.0) smooth freq with boxes; \
	set title "Wire#3 Y 2D error histogram" ; \
	set origin 0.0,0.0; \
	plot f using (bin(($10),binwidth)):(1.0) smooth freq with boxes; \
else if (w==4) \
	set title "Wire#4 X 2D error histogram" ; \
	set origin 0.0,0.5; \
	plot f using (bin(($11),binwidth)):(1.0) smooth freq with boxes; \
	set title "Wire#4 Y 2D error histogram" ; \
	set origin 0.0,0.0; \
	plot f using (bin(($12),binwidth)):(1.0) smooth freq with boxes; \
else if (w==5) \
	set title "Wire#5 X 2D error histogram" ; \
	set origin 0.0,0.5; \
	plot f using (bin(($13),binwidth)):(1.0) smooth freq with boxes; \
	set title "Wire#5 Y 2D error histogram" ; \
	set origin 0.0,0.0; \
	plot f using (bin(($14),binwidth)):(1.0) smooth freq with boxes; \
else if (w==6) \
	set title "Wire#6 X 2D error histogram" ; \
	set origin 0.0,0.5; \
	plot f using (bin(($16),binwidth)):(1.0) smooth freq with boxes; \
	set title "Wire#6 Y 2D error histogram" ; \
	set origin 0.0,0.0; \
	plot f using (bin(($17),binwidth)):(1.0) smooth freq with boxes; \
else if (w==7) \
	set title "Wire#7 X 2D error histogram" ; \
	set origin 0.0,0.5; \
	plot f using (bin(($18),binwidth)):(1.0) smooth freq with boxes; \
	set title "Wire#7 Y 2D error histogram" ; \
	set origin 0.0,0.0; \
	plot f using (bin(($19),binwidth)):(1.0) smooth freq with boxes; \
else if (w==8) \
	set title "Wire#8 X 2D error histogram" ; \
	set origin 0.0,0.5; \
	plot f using (bin(($20),binwidth)):(1.0) smooth freq with boxes; \
	set title "Wire#8 Y 2D error histogram" ; \
	set origin 0.0,0.0; \
	plot f using (bin(($21),binwidth)):(1.0) smooth freq with boxes; \
else if (w==9) \
	set title "Wire#9 X 2D error histogram" ; \
	set origin 0.0,0.5; \
	plot f using (bin(($23),binwidth)):(1.0) smooth freq with boxes; \
	set title "Wire#9 Y 2D error histogram" ; \
	set origin 0.0,0.0; \
	plot f using (bin(($24),binwidth)):(1.0) smooth freq with boxes; \
else \
	exit -1;
	
unset multiplot     

## This will keep the graph on the screen and wait for return after
pause -1