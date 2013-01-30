## PlotProbeCalibrationErrorHistogram3D
## -------------------------------------
## Create a 3D reprojection error histogram for probe calibration  
##
## gnuplot.exe -e "f='ProbeCalibrationReport.txt'; o='ProbeCalibrationErrorHistogram3D'; w=2" PlotProbeCalibrationErrorHistogram3D.gnu 
## 
## Input arguments: 
##    f='ProbeCalibrationReport.txt'    => auto generated file
##	  o='ProbeCalibrationErrorHistogram3D' => Output file name prefix
##    w=1 						                 => wire number
##--------------------------------------------------------

## Set the terminal mode 
set terminal jpeg
# w1_ProbeCalibrationErrorHistogram3D.jpg
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
set xrange [0:10]

# Function to generate histogram  
bin(x,width)=width*floor(x/width)
	
## Plot the histogram  
if (w==2) \
	set title "Wire#2 3D error histogram (px)" ; \
	plot f using (bin(($8),binwidth)):(1.0) smooth freq with boxes; \
else if (w==5) \
	set title "Wire#5 3D error histogram (px)" ; \
	plot f using (bin(($15),binwidth)):(1.0) smooth freq with boxes; \
else if (w==8) \
	set title "Wire#8 3D error histogram (px)" ; \
	plot f using (bin(($22),binwidth)):(1.0) smooth freq with boxes; \
else \
	exit -1;
	
## This will keep the graph on the screen and wait for return after
pause -1