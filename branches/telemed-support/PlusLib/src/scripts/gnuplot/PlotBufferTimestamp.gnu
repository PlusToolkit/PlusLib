## PlotSyncResult
## --------------------------
## 
##
## gnuplot.exe -e "f='BufferTimestamps.txt'; o='BufferTimestamps'"  PlotBufferTimestamp.gnu 
##
##    f='BufferTimestamps.txt'    => auto generated input file
##	  o='BufferTimestamps' => Output file name prefix
## 
##--------------------------------------------------------

## Set the terminal mode 
set terminal jpeg
# BufferTimestamps.jpg
set output o.".jpg"

## Set the plot properties 
set key bmargin vertical Right noreverse autotitle nobox
set datafile missing '#'
set grid 

## Plot frame period
set ylabel "Filtered and Unfiltered Timestamps"
set xlabel "Frame number"
plot f using 1:2 with lines t columnhead, f using 1:3 with lines t columnhead; 

pause -1