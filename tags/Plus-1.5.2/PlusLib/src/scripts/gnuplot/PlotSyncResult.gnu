## PlotSyncResult
## --------------------------
## 
##
## gnuplot.exe -e "f='SyncResult.txt' o='SyncResult'"  PlotSyncResult.gnu
## 
##    f='SyncResult.txt'    => auto generated input file
##	  o='SyncResult' => Output file name prefix
##--------------------------------------------------------

## Set the terminal mode 
set terminal jpeg
# SyncResult.jpg
set output o.".jpg"

## Set the plot properties 
set key bmargin vertical Right noreverse autotitle nobox
set datafile missing '#'
set grid 
set yrange [-2:]

## Plot the histogram  
set title "Synchronization result" ; \
plot f using 1:2 with linespoints t "TransformDifference", \
	 f using 3:4 with linespoints t "FrameDifference"

## This will keep the graph on the screen and wait for return after
pause -1