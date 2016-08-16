function [volumeStruct] = mha_read_volume(filename,readPixels)
% Function for reading the pixel and meta data of a Insight Meta-Image (.mha, .mhd) file
%
% Input parameters:
% - filename: file path of .mha or .mhd
% - readPixels: if true then the pixel data is loaded, otherwise just the header (optional, default: true)
% 
% Output parameters:
% - volumeStruct: structure containing the header and pixel data
%
% Examples:
% 
% 1. Read the volume
%
%   vol = mha_read_volume('testData/MR-head_uncompressed.mhd');
%
% 2. Access to metadata
%
%   imageOrigin = vol.metaData.Origin
%   imageSpacing = vol.metaData.Spacing
%   imageAxesOrientation = vol.metaData.ImageAxesOrientation
%
% 3. Access to pixel data
%
%   frame = vol.pixelData(:,:,50);
%   imshow(frame,[0 200]);
%
% Note: AnatomicalOrientation field in the volume is ignored and assumed to be LPS

% If user does not specify if they want the whole volume or just the header
% then return the whole volume
if nargin < 2
   readPixels = true;
end

% retrieve header meta data
VolumeHeader = mha_read_header(filename);

Filename = VolumeHeader.Filename;
BitDepth = VolumeHeader.BitDepth;
HeaderSize = VolumeHeader.HeaderSize;

% remove non meta data fields (fields added in mha_read_header)
VolumeHeader = rmfield(VolumeHeader, 'Filename');
VolumeHeader = rmfield(VolumeHeader, 'BitDepth');
VolumeHeader = rmfield(VolumeHeader, 'HeaderSize');

% structure to store the header and the pixel data
volumeStruct= [];

volumeStruct.metaData = VolumeHeader;

if (~readPixels)
    return;    
end

switch(lower(VolumeHeader.ElementDataFile))
    case 'local'
    otherwise
    % Seperate file
    Filename=fullfile(fileparts(Filename),VolumeHeader.ElementDataFile);
end
        
% byte order may be specified by two tag names in metaIO format
if (isfield(VolumeHeader,{'BinaryDataByteOrderMSB'})) 
   ByteOrder = VolumeHeader.BinaryDataByteOrderMSB; 
else
   ByteOrder = VolumeHeader.ElementByteOrderMSB;
end


% Open file
switch(lower(ByteOrder(1)))
    case ('true')
        fid=fopen(Filename','rb','ieee-be');
    otherwise
        fid=fopen(Filename','rb','ieee-le');
end

switch(lower(VolumeHeader.ElementDataFile))
    case 'local'
        % Skip header
        fseek(fid,HeaderSize,'bof');
    otherwise
        fseek(fid,0,'bof');
end

datasize=prod(VolumeHeader.Size)*BitDepth/8;

switch(lower(VolumeHeader.CompressedData(1)))
    case 'f'
        % Read the Data
        switch(lower(VolumeHeader.ElementType(5:end)))
            case 'char'
                pixelData = int8(fread(fid,datasize,'char')); 
            case 'uchar'
                pixelData = uint8(fread(fid,datasize,'uchar')); 
            case 'short'
                pixelData = int16(fread(fid,datasize,'short')); 
            case 'ushort'
                pixelData = uint16(fread(fid,datasize,'ushort')); 
            case 'int'
                pixelData = int32(fread(fid,datasize,'int')); 
            case 'uint'
                pixelData = uint32(fread(fid,datasize,'uint')); 
            case 'float'
                pixelData = single(fread(fid,datasize,'float'));   
            case 'double'
                pixelData = double(fread(fid,datasize,'double'));
        end
    case 't'
        switch(lower(VolumeHeader.ElementType(5:end)))
            case 'char', DataType='int8';
            case 'uchar', DataType='uint8';
            case 'short', DataType='int16';
            case 'ushort', DataType='uint16';
            case 'int', DataType='int32';
            case 'uint', DataType='uint32';
            case 'float', DataType='single';
            case 'double', DataType='double';
        end
        Z  = fread(fid,inf,'uchar=>uint8');
        try
            pixelData = zlib_decompress(Z,DataType);
        catch noMemory
            disp('not enough heap space (can increase in preferences)');
            return;
        end
end        
fclose('all');

pixelData = reshape(pixelData,VolumeHeader.Size);

volumeStruct.pixelData = pixelData;




function M = zlib_decompress(Z,DataType)
% Function for decompressing .mha or .mhd pixel data 
% 
% params: - pixel data to decompress (from uchar to uint8)
%         - data type of the volume
% returns: decompressed pixel data
%
% examples:
% 1.  pixelData = zlib_decompress(Z,int32);

import com.mathworks.mlwidgets.io.InterruptibleStreamCopier
a=java.io.ByteArrayInputStream(Z);
b=java.util.zip.InflaterInputStream(a);
isc = InterruptibleStreamCopier.getInterruptibleStreamCopier;
c = java.io.ByteArrayOutputStream;
isc.copyStream(b,c);
M=typecast(c.toByteArray,DataType);

function VolumeInfo = mha_read_header(filename)
% Function for reading the header of a Insight Meta-Image (.mha,.mhd) file
% 
% params: .mha or .mhd meta-image file path 
% returns: header info from filename
%
% examples:
% 1.  VolumeInfo=mha_read_header(filename);
%
% NOTE: Anatomical Orientation of volume ignored and assumed to be LPS

if(exist('filename','var')==0)
    [filename, pathname] = uigetfile('*.mha', 'Read mha-file');
    filename = [pathname filename];
end

fid=fopen(filename,'rb');
if(fid<0)
    fprintf('could not open file %s\n',filename);
    return
end

VolumeInfo.Filename=filename;
VolumeInfo.CompressedData='false';
readelementdatafile=false;

while(~readelementdatafile)
    str=fgetl(fid);
    s=find(str=='=',1,'first');
    if(~isempty(s))
        type=str(1:s-1); 
        data=str(s+1:end);
        while(type(end)==' '); type=type(1:end-1); end
        while(data(1)==' '); data=data(2:end); end
    else
        type=''; data=str;
    end
    
    switch(lower(type))
        % Number of dimensions in the image
        case 'ndims' 
            VolumeInfo.NumOfDimensions=sscanf(data, '%d')';
            
        % Size: size of the volume in pixels along each direction
        case 'dimsize' 
            VolumeInfo.Size=sscanf(data, '%d')';
            
        % Spacing: size of a pixel in physical space in mm along each dimension
        case 'elementspacing'   
            VolumeInfo.Spacing=sscanf(data, '%lf')';
            
        case 'elementsize'
            VolumeInfo.ElementSize=sscanf(data, '%lf')';
            if(~isfield(VolumeInfo,'Spacing'))
                VolumeInfo.Spacing=VolumeInfo.ElementSize;
            end
            
        % byte order indicates is the data is encoded in little or big endian order
        case 'elementbyteordermsb'
            VolumeInfo.ElementByteOrderMSB=data;
            
        % image orientation
        % value from file ignored, overriden to LPS
        case 'anatomicalorientation' 
            VolumeInfo.AnatomicalOrientation='LPS';
            
        % point around which the image is rotated
        case 'centerofrotation' 
            VolumeInfo.CenterOfRotation=sscanf(data, '%lf')';
            
        % Origin: the origin of the image in mm
        case 'offset' 
            VolumeInfo.Origin=sscanf(data, '%lf')';
            
        case 'binarydata'
            VolumeInfo.BinaryData=data;
            
        case 'compresseddatasize'
            VolumeInfo.CompressedDataSize=sscanf(data, '%d')';
            
        case 'objecttype',
            VolumeInfo.ObjectType=data;
            
        % Direction: orientation of the image axes
        % Format: <Rxx> <Rxy> <Rxz> <Ryx> <Ryy> <Ryz> <Rzx> <Rzy> <Rzz> 
        case 'transformmatrix' 
            VolumeInfo.ImageAxesOrientation=sscanf(data, '%lf')';
            
        case 'compresseddata';
            VolumeInfo.CompressedData=data;
        
        % byte order indicates is the data is encoded in little or big endian order
        case 'binarydatabyteordermsb'
            VolumeInfo.BinaryDataByteOrderMSB=data;
            
        % name of the file containing the raw binary data of the image 
        % This file must be in the same directory as the header.
        % 'LOCAL' if within same file as the header
        case 'elementdatafile'
            VolumeInfo.ElementDataFile=data;
            readelementdatafile=true;
        
        % primitive type used for pixels   
        case 'elementtype'
            VolumeInfo.ElementType=data;
            
        case 'headersize'
            val=sscanf(data, '%d')';
            if(val(1)>0), VolumeInfo.HeaderSize=val(1); end
            
        otherwise
            VolumeInfo.(type)=data;
    end
end

switch(lower(VolumeInfo.ElementType(5:end)))
    case 'char', VolumeInfo.BitDepth=8;
    case 'uchar', VolumeInfo.BitDepth=8;
    case 'short', VolumeInfo.BitDepth=16;
    case 'ushort', VolumeInfo.BitDepth=16;
    case 'int', VolumeInfo.BitDepth=32;
    case 'uint', VolumeInfo.BitDepth=32;
    case 'float', VolumeInfo.BitDepth=32;
    case 'double', VolumeInfo.BitDepth=64;
    otherwise, VolumeInfo.BitDepth=0;
end
if(~isfield(VolumeInfo,'HeaderSize'))
    VolumeInfo.HeaderSize=ftell(fid);
end

fclose(fid);

