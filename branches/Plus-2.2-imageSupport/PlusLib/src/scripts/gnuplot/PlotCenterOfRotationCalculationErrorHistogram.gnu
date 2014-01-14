## PlotCenterOfRotationCalculationErrorHistogram
## -------------------------------------
## Create center of rotation error histogram for each wire 
##
## gnuplot.exe -e "f='CenterOfRotationCalculationError.txt'; o='CenterOfRotationCalculationErrorHistogram'; w=1" PlotCenterOfRotationCalculationErrorHistogram.gnu 
## 
## Input arguments: 
##    f='CenterOfRotationCalculationError.txt'    => auto generated file
##	  o='CenterOfRotationCalculationErrorHistogram' => Output file name prefix
##    w=1 						                 => wire number
##--------------------------------------------------------

## Set the terminal mode 
set terminal jpeg
# w1_CenterOfRotationCalculationErrorHistogram.jpg
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
set xrange [-2:2]

# Function to generate histogram  
bin(x,width)=width*floor(x/width)

## Plot the histogram  
if (w==1) \
	set title "Wire #1 Distance from Rotation Center Histogram" ; \
	plot f using (bin(($5),binwidth)):(1.0) smooth freq with boxes; \
else if (w==3) \
	set title "Wire #3 Distance from Rotation Center Histogram" ; \
	plot f using (bin(($9),binwidth)):(1.0) smooth freq with boxes; \
else if (w==4) \
	set title "Wire #4 Distance from Rotation Center Histogram" ; \
	plot f using (bin(($13),binwidth)):(1.0) smooth freq with boxes; \
else if (w==6) \
	set title "Wire #6 Distance from Rotation Center Histogram" ; \
	plot f using (bin(($17),binwidth)):(1.0) smooth freq with boxes; \
else if (w==7) \
	set title "Wire #7 Distance from Rotation Center Histogram" ; \
	plot f using (bin(($21),binwidth)):(1.0) smooth freq with boxes; \
else if (w==9) \
	set title "Wire #9 Distance from Rotation Center Histogram" ; \
	plot f using (bin(($25),binwidth)):(1.0) smooth freq with boxes; \
else \
	set title "Wire #1 Distance from Rotation Center Histogram" ; \
	plot f using (bin(($5),binwidth)):(1.0) smooth freq with boxes; 


## This will keep the graph on the screen and wait for return after
pause -1