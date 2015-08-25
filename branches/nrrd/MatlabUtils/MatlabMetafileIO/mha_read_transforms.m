% mha_read_transforms: Read transformation matrices, their status, and timestamps from a sequence metafile header (mha or mhd)
%
%   Description
%     This is a helper function for reading frame indexes and corresponding
%     timestamps from a sequence metafile (mha). This can be used for
%     investigating potential item timestamping issues.
%     The [uts, ind] plot is a straight line if the frame rate is constant.
%
%   Outputs:
%     transforms: struct containing a vector of 4x4 transforms and status for each frame for each tool, e.g., transforms.ToolToTrackerTransformMatrix(m,n,frameIndex+1)
%     timestamps: vector of timestamps
%     unfilteredTimestamps: vector of raw (not jitter-filtered) timestamps
%
%   Example: Plot the y translation component of the ToolToTracker transform in time
%     [transforms timestamps unfilteredTimestamps] = mha_read_transforms('TrackedImageSequence_20130326_174150-long-raolao.mha');
%     plot(timestamps, squeeze( transforms.ToolToTrackerTransformMatrix(3,4,:) ), '.')
%

function [transforms timestamps unfilteredTimestamps] = mha_read_transforms(filename)

fid = fopen(filename);

verbose=0;

timestamps=[];
unfilteredTimestamps=[];
transforms=struct;

while 1

  tline = fgetl(fid);

  if ~ischar(tline)
    break
  end

  % Detect ...Transform and ...TransformStatus fields with one regexp to
  % reduce parsing time
  token=regexp(tline, 'Seq_Frame([0-9]*)_(.*)Transform((|Status)) = (.*)', 'tokens');
  if (length(token)>0)
      frameIndex=sscanf(token{1}{1},'%d');
      transformName=token{1}{2};
      if (isempty(token{1}{3}))
        % Transform      
          transformFieldName=[transformName 'TransformMatrix'];
          if isfield(transforms,transformFieldName)==0
            % Initialize the array to its final size, because concatenation for each frame would take a very long time
            transforms.(transformFieldName)=zeros(4,4,numberOfFrames);
          end
          transforms.(transformFieldName)(:,:,frameIndex+1)=reshape(sscanf(token{1}{4},'%f'),4,4)';
      else
        % TransformStatus
        transformStatusFieldName=[transformName 'TransformStatus'];
        transformStatus=strtrim(token{1}{4});
        if (strcmp(transformStatus,'OK'))
          statusCode=1;
        else
          statusCode=0;
        end
        if isfield(transforms,transformStatusFieldName)==0
            % Initialize the array to its final size, because concatenation for each frame would take a very long time
            transforms.(transformStatusFieldName)=zeros(numberOfFrames,1);
        end
        transforms.(transformStatusFieldName)(frameIndex+1)=statusCode;          
      end
      continue;      
  end

  token=regexp(tline, 'Seq_Frame([0-9]*)_UnfilteredTimestamp = (.*)', 'tokens');
  if (length(token)>0)
    frameIndex=sscanf(token{1}{1},'%d');
    n=sscanf(token{1}{2},'%f');
    unfilteredTimestamps(frameIndex+1)=n;
    continue;
  end

  token=regexp(tline, 'Seq_Frame([0-9]*)_Timestamp = (.*)', 'tokens');
  if (length(token)>0)
    frameIndex=sscanf(token{1}{1},'%d');
    n=sscanf(token{1}{2},'%f');
    timestamps(frameIndex+1)=n;
    if verbose && (mod(frameIndex,100)==0 || frameIndex+1==numberOfFrames)
      disp(['Reading... (' num2str(round(frameIndex/numberOfFrames*100)) '%)']);
    end
    continue;
  end
  
  token=regexp(tline, 'DimSize = (.*)', 'tokens');
  if (length(token)>0)
    dimSize=sscanf(token{1}{1},'%d');
    numberOfFrames=dimSize(3);
    % Initialize arrays to their final sizes, because concatenation for each frame would take a very long time
    timestamps=zeros(numberOfFrames,1);
    unfilteredTimestamps=zeros(numberOfFrames,1);
    continue;
  end

  if (regexp(tline, 'ElementDataFile')>0)
    break
  end

end

fclose(fid);
