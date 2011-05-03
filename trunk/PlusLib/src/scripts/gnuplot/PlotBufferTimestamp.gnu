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
set yrange [-200:200]

## Plot frame period
set ylabel "Time difference (ms)"
set xlabel "Frame number"
set title "Sample Timestamps" ; 
plot f using 1:($5*1000) with lines t columnhead, \
	 f using 1:($6*1000) with lines t columnhead, \
	 f using 1:($7*1000) with lines t columnhead

