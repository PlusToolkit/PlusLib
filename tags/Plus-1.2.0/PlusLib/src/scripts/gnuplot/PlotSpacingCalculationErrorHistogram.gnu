## PlotSpacingCalculationErrorHistogram
## -------------------------------------
## Create an LRE error histogram for spacing calculation 
##
## gnuplot.exe -e "f='SpacingCalculationError.txt'; o='SpacingCalculationErrorHistogram';" PlotSpacingCalculationErrorHistogram.gnu 
## 
## Input arguments: 
##    f='SpacingCalculationError.txt'    => auto generated file
##	  o='SpacingCalculationErrorHistogram' => Output file name prefix
##--------------------------------------------------------

## Set the terminal mode 
set terminal jpeg
# SpacingCalculationErrorHistogram.jpg
set output o.".jpg"

## Set the histogram bin width size
binwidth=0.1

## Set the plot properties 
set key bmargin vertical Right noreverse noautotitle nobox
set boxwidth binwidth absolute  
set style data histograms  
set style fill solid 1.0 border -1 
set grid 

## Set the x axis range
set xrange [-1:1]

## Enable multiplot mode 
set multiplot
set size 1,0.5; 

# Function to generate histogram  
bin(x,width)=width*floor(x/width)

## Plot the histogram  
set title "X Spacing Error Histogram" ; \
set origin 0.0,0.5; \
set xlabel "Computed and Measured Wire Distance Difference - X (mm)"; \
plot f using (bin(($1 - $2),binwidth)):(1.0) smooth freq with boxes; \
set title "Y Spacing Error Histogram"; \
set origin 0.0,0.0; \
set xlabel "Computed and Measured Wire Distance Difference - Y (mm)"; \
plot f using (bin(($3 - $4),binwidth)):(1.0) smooth freq with boxes; \

unset multiplot     

## This will keep the graph on the screen and wait for return after
pause -1
