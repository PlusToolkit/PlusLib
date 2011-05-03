## PlotSegmentedWirePositions
## --------------------------
## Create an LRE error histogram for each wire 
##
## gnuplot.exe -e "f='ValidationSegWirePos.txt'; o='ValidationSegWirePosHistogram'; w=6" PlotSegmentedWirePositionsHistogram.gnu
## 
## Input arguments: 
##    f='SegWirePosition.txt'    => auto generated input file
##	  o='ValidationSegWirePosHistogram' => Output file name prefix
##    w=1 						 => wire number
##--------------------------------------------------------

## Set the terminal mode 
set terminal jpeg
# w1_ValidationSegWirePosHistogram.jpg
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
	plot f using (bin(($28 - $40),binwidth)):(1.0) smooth freq with boxes; \
	set title "Wire#1 Y histogram" ; \
	set origin 0.0,0.0; \
	plot f using (bin(($29 - $41),binwidth)):(1.0) smooth freq with boxes; \
else if (w==2) \
	set title "Wire#2 X histogram" ; \
	set origin 0.0,0.5; \
	plot f using (bin(($30 - $42),binwidth)):(1.0) smooth freq with boxes; \
	set title "Wire#2 Y histogram" ; \
	set origin 0.0,0.0; \
	plot f using (bin(($31 - $43),binwidth)):(1.0) smooth freq with boxes; \
else if (w==3) \
	set title "Wire#3 X histogram" ; \
	set origin 0.0,0.5; \
	plot f using (bin(($32 - $44),binwidth)):(1.0) smooth freq with boxes; \
	set title "Wire#3 Y histogram" ; \
	set origin 0.0,0.0; \
	plot f using (bin(($33 - $45),binwidth)):(1.0) smooth freq with boxes; \
else if (w==4) \
	set title "Wire#4 X histogram" ; \
	set origin 0.0,0.5; \
	plot f using (bin(($34 - $46),binwidth)):(1.0) smooth freq with boxes; \
	set title "Wire#4 Y histogram" ; \
	set origin 0.0,0.0; \
	plot f using (bin(($35 - $47),binwidth)):(1.0) smooth freq with boxes; \
else if (w==5) \
	set title "Wire#5 X histogram" ; \
	set origin 0.0,0.5; \
	plot f using (bin(($36 - $48),binwidth)):(1.0) smooth freq with boxes; \
	set title "Wire#5 Y histogram" ; \
	set origin 0.0,0.0; \
	plot f using (bin(($37 - $49),binwidth)):(1.0) smooth freq with boxes; \
else if (w==6) \
	set title "Wire#6 X histogram" ; \
	set origin 0.0,0.5; \
	plot f using (bin(($38 - $50),binwidth)):(1.0) smooth freq with boxes; \
	set title "Wire#6 Y histogram" ; \
	set origin 0.0,0.0; \
	plot f using (bin(($39 - $51),binwidth)):(1.0) smooth freq with boxes; \
else \
	set title "Wire#1 X histogram" ; \
	set origin 0.0,0.5; \
	plot f using (bin(($28 - $40),binwidth)):(1.0) smooth freq with boxes; \
	set title "Wire#1 Y histogram" ; \
	set origin 0.0,0.0; \
	plot f using (bin(($29 - $41),binwidth)):(1.0) smooth freq with boxes; 

	
unset multiplot     
