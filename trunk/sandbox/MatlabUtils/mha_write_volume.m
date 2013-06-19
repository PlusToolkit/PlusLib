function mha_write_volume(outputFilename, pixelWithMetaStruct, writePixels)
% Function for writing an array struct to an Insight Meta-Image (.mha, .mhd) file
% 
% Input parameters:
% - outputFilename: output path for the .mha or .mhd meta image
% - pixelWithMetaStruct: array struc of pixel and meta data (output from mha_read_volume)
% - writePixels: if true then pixels are written to the output file, otherwise only the header is written (optional, default: true)
%
% Examples:
%
% 1. Using output from mha_read_volume: 
%
%   vol = mha_read_volume('MetaImageIO\testData\MR-head_compressed.mha')
%   mha_write_volume('testOutput.mha', vol)
%    
% 2. Creating volume from scratch
%
%   HEADER:
%   Need to define parameters in the header of the mha or mhd.
%   Please see mha_read_header within mha_read_volume.m for details on
%   these parameters.
%   The minimal structure of the MetaImage header is the following:
%   metaData.NumOfDimensions = 3;
%   metaData.ObjectType = 'Image';
%   metaData.Origin = [0 0 0];
%   metaData.Size = [512 512 61];
%   metaData.ElementType = 'MET_DOUBLE';
%   metaData.Spacing = [0.3066 0.3066 2.5000];
%   metaData.ElementByteOrderMSB = 'False';             
%   metaData.ElementDataFile = 'LOCAL'; 
%   metaData.CompressedData = 'False';
%   PIXELDATA:
%   pixelData should be: pixelData<512x512x61 double> % this should be the same type as
%                                                       metaData.ElementType
%
%   VOLUME STRUCTURE TO WRITE OUT:
%   pixelWithMetaStruct.metaData = metaData;
%   pixelWithMetaStruct.pixelData = pixelData;
%
%   WRITE OUT VOLUME
%   mha_write_volume(pixelWithMetaStruct, true, 'testOutput.mha')

if isempty(writePixels)
  writePixels=1
end

% meta data structure stored in the array structure
metaData = pixelWithMetaStruct.metaData;

% open file for writing
fid=fopen(outputFilename, 'w');
if(fid<=0) 
  fprintf('Could not open file: %s\n', outputFilename);
end

if(writePixels)
  % pixel data matrix stored in the array structure
  pixelData = pixelWithMetaStruct.pixelData;

  % get pixel type
  DataType = class(pixelData);

  % compress pixel data
  switch(lower(metaData.CompressedData(1)))
    case 't'
      pixelData = zlib_compress(pixelData,DataType);
      pixelDataInfo = whos('pixelData');
      bytes = pixelDataInfo.bytes;
      metaData.CompressedDataSize = bytes; 
      DataType = 'uchar';
  end        

end
metaDataCellArr = struct2cell(metaData);

% get string names for each field in the meta data
fields = fieldnames(metaData);

% Print the header data to the output file
for i=1:numel(fields)
  
  switch(lower(fields{i}))
    
    % Number of dimensions in the image
    case 'numofdimensions' 
      fprintf(fid,'NDims');
      writeDataByType(fid,metaDataCellArr{i});
      
    % Size: size of the volume in pixels along each direction
    case 'size' 
      fprintf(fid,'DimSize');
      writeDataByType(fid,metaDataCellArr{i});
            
    % Spacing: size of a pixel in physical space in mm along each dimension
    case 'spacing'   
      fprintf(fid,'ElementSpacing');
      writeDataByType(fid,metaDataCellArr{i});
              
    % byte order    
    case 'elementbyteordermsb'
      fprintf(fid,'ElementByteOrderMSB');
      writeDataByType(fid,metaDataCellArr{i});
            
     % image orientation
     % value from file ignored, overriden to LPS
     case 'anatomicalorientation' 
       fprintf(fid,'AnatomicalOrientation');
       writeDataByType(fid,metaDataCellArr{i});
            
     % point around which the image is rotated
     case 'centerofrotation' 
       fprintf(fid,'CenterOfRotation');
       writeDataByType(fid,metaDataCellArr{i});
            
     % Origin: the origin of the image in mm
     case 'origin' 
       fprintf(fid,'Offset');
       writeDataByType(fid,metaDataCellArr{i});
                  
     case 'binarydata'
       fprintf(fid,'BinaryData');
       writeDataByType(fid,metaDataCellArr{i});            
            
     case 'compresseddatasize'
       fprintf(fid,'CompressedDataSize');
       writeDataByType(fid,metaDataCellArr{i});
       
     case 'objecttype',
       fprintf(fid,'ObjectType');
       writeDataByType(fid,metaDataCellArr{i});            
            
     % Direction: orientation of the image axes
     % Format: <Rxx> <Rxy> <Rxz> <Ryx> <Ryy> <Ryz> <Rzx> <Rzy> <Rzz> 
     case 'imageaxesorientation' 
       fprintf(fid,'TransformMatrix');
       writeDataByType(fid,metaDataCellArr{i});             
            
     case 'compresseddata';
       fprintf(fid,'CompressedData');
       writeDataByType(fid,metaDataCellArr{i});             
        
     % byte order    
     case 'binarydatabyteordermsb'
       fprintf(fid,'BinaryDataByteOrderMSB');
       writeDataByType(fid,metaDataCellArr{i});           
            
     case 'elementdatafile'
       fprintf(fid,'ElementDataFile');
       writeDataByType(fid,metaDataCellArr{i});             
        
     % data type    
     case 'elementtype'
       fprintf(fid,'ElementType');
       writeDataByType(fid,metaDataCellArr{i});             

  end

end

if (~writePixels)
  fclose(fid);
  return;
end

% change file location of .zraw or .raw for .mhd
if (~strcmpi(metaData.ElementDataFile,'local'))
  if (strcmpi(metaData.CompressedData,'true'))
        metaData.ElementDataFile = strcat(outputFilename(1:end-3),'zraw');
  else
      metaData.ElementDataFile = strcat(outputFilename(1:end-3),'raw');
  end
end

% If mhd, write pixel data in .zraw file
% Otherwise, write pixel data within same file
switch(lower(metaData.ElementDataFile))
  case 'local'
    otherwise
      % Seperate file
      fclose(fid);
      fid=fopen(metaData.ElementDataFile, 'w');
      if(fid<=0) 
        fprintf('Could not open file: %s\n', metaData.ElementDataFile);
      end
end


fwrite(fid, pixelData,DataType);

fclose('all');

function writeDataByType(fid, data)
% Function that writes the header data to file based on the type of data
%
% params: - fid of file to write to
%         - data from header to write

  %write the fields out depending on their type
  if ischar(data)
    fprintf(fid,' = %s\n',data);  
  else
    fprintf(fid,' = ');
    fprintf(fid,'%d ',data);  
    fprintf(fid,'\n');
  end



function compressedPixelData = zlib_compress(pixelData,DataType)
% Function for compressing (.mha, .mhd) pixel data
% 
% params: - pixel data to be compressed
%         - data type of volume
%
% examples:
% 1.  compressedPixelData = zlib_compress(pixelData,int32)

Size = size(pixelData);

cn = strmatch(DataType,{'double','single','logical','char','int8','uint8',...
    'int16','uint16','int32','uint32','int64','uint64'});

if cn == 3 || cn == 4
  pixelData=uint8(pixelData);
end
pixelData=typecast(pixelData(:),'uint8');
a=java.io.ByteArrayOutputStream();
b=java.util.zip.DeflaterOutputStream(a);
b.write(pixelData);
b.close;
compressedPixelData=typecast(a.toByteArray,'uint8');
a.close;