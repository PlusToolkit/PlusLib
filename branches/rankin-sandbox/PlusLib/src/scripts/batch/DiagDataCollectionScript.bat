SET TIMELENGTH=30
SET CONFIGFILE=..\config\USDataCollectionConfig_SonixTouch_L14-5_NDICertus.xml
SET OUTPUTFOLDER=s:\images\TrackedUltrasound\SynchronizationTest\2011-07-27-raw-data_2
SET TRACKERBUFFER=Heart_NDI_Certus_Buffer
SET VIDEOBUFFER=Heart_SonixVideo_Buffer
SET NUMBEROFTESTS=10
SET VERBOSELEVEL=3

cd bin
@FOR /L %%i IN (1,1,%NUMBEROFTESTS%) DO (

@REM remove old log files 
@ECHO Y | DEL /Q *PlusLog.txt

@REM run data collection 
DiagDataCollection.exe --input-acq-time-length=%TIMELENGTH% --output-folder=%OUTPUTFOLDER% --output-tracker-buffer-seq-file-name=0%%i_%TRACKERBUFFER% --output-video-buffer-seq-file-name=0%%i_%VIDEOBUFFER% --input-config-file-name=%CONFIGFILE% --verbose=%VERBOSELEVEL%

@REM move log file to output folder
move *PlusLog.txt %OUTPUTFOLDER%\0%%i_PlusLog.txt
)

cd ..
