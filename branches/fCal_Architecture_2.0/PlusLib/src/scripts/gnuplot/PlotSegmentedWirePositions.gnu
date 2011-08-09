## PlotSegmentedWirePositions
## --------------------------
## Create an LRE error plot for each wire 
##
## gnuplot.exe -e "f='ValidationSegWirePos.txt'; o='ValidationSegWirePos'; w=6" PlotSegmentedWirePositions.gnu
## 
## Input arguments: 
##    f='SegWirePosition.txt'    => auto generated input file
##	  o='ValidationSegWirePos' => Output file name prefix
##    w=1 						 => wire number
##--------------------------------------------------------

## Set the terminal mode 
set terminal jpeg
## Output name
# w1x_ValidationSegWirePos.jpg
# 'w'.w."x_".o.".jpg"
# w1y_ValidationSegWirePos.jpg
# 'w'.w."y_".o.".jpg" 

## Set the plot properties 
set key bmargin vertical Right noreverse autotitle nobox
set datafile missing '#'
set grid 

## Set the x axis range
set xrange [-80:-10]

## Plot the histogram  
if (w==1) \
	set title "Wire#1 X error" ; \
	set output 'w'.w."x_".o.".jpg"; \
	plot f using 2:28 with points t columnhead, f using 2:40 with lines t columnhead; \
	pause -1; \
	set title "Wire#1 Y error" ; \
	set output 'w'.w."y_".o.".jpg"; \
	plot f using 2:29 with points t columnhead, f using 2:41 with lines t columnhead ; \
else if (w==2) \
	set title "Wire#2 X error" ; \
	set output 'w'.w."x_".o.".jpg"; \
	plot f using 2:30 with points t columnhead, f using 2:42 with lines t columnhead; \
	pause -1; \
	set title "Wire#2 Y error" ; \
	set output 'w'.w."y_".o.".jpg"; \
	plot f using 2:31 with points t columnhead, f using 2:43 with lines t columnhead ; \
else if (w==3) \
	set title "Wire#3 X error" ; \
	set output 'w'.w."x_".o.".jpg"; \
	plot f using 2:32 with points t columnhead, f using 2:44 with lines t columnhead; \
	pause -1; \
	set title "Wire#3 Y error" ; \
	set output 'w'.w."y_".o.".jpg"; \
	plot f using 2:33 with points t columnhead, f using 2:45 with lines t columnhead ; \
else if (w==4) \
	set title "Wire#4 X error" ; \
	set output 'w'.w."x_".o.".jpg"; \
	plot f using 2:34 with points t columnhead, f using 2:46 with lines t columnhead; \
	pause -1; \
	set title "Wire#4 Y error" ; \
	set output 'w'.w."y_".o.".jpg"; \
	plot f using 2:35 with points t columnhead, f using 2:47 with lines t columnhead ; \
else if (w==5) \
	set title "Wire#5 X error" ; \
	set output 'w'.w."x_".o.".jpg"; \
	plot f using 2:36 with points t columnhead, f using 2:48 with lines t columnhead; \
	pause -1; \
	set title "Wire#5 Y error" ; \
	set output 'w'.w."y_".o.".jpg"; \
	plot f using 2:37 with points t columnhead, f using 2:49 with lines t columnhead ; \
else if (w==6) \
	set title "Wire#6 X error" ; \
	set output 'w'.w."x_".o.".jpg"; \
	plot f using 2:38 with points t columnhead, f using 2:50 with lines t columnhead; \
	pause -1; \
	set title "Wire#6 Y error" ; \
	set output 'w'.w."y_".o.".jpg"; \
	plot f using 2:39 with points t columnhead, f using 2:51 with lines t columnhead ; \
else \
	set title "Wire#1 X error" ; \
	set output 'w'.w."x_".o.".jpg"; \
	plot f using 2:28 with points t columnhead, f using 2:40 with lines t columnhead; \
	pause -1; \
	set title "Wire#1 Y error" ; \
	set output 'w'.w."y_".o.".jpg"; \
	plot f using 2:29 with points t columnhead, f using 2:41 with lines t columnhead ; \

	
unset multiplot     

## This will keep the graph on the screen and wait for return after
pause -1