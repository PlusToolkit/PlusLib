function [ts ind]=readmhatimestamp(fname)

% readmhatimestamp: Read (unfiltered) timestamps and frame indexes from a sequence metafile header
%
%   Description
%     This is a helper function for reading frame indexes and corresponding
%     timestamps from a sequence metafile (mha). This can be used for
%     investigating potential item timestamping issues.
%     The [ts, ind] plot is a straight line if the frame rate is constant.
%
%   Example
%     [ts ind]=readmhatimestamp('01_Heart_NDI_Certus_Buffer_01.mha'); 
%     plot(ts, ind, '.')
%

fid = fopen(fname);

ts=[];
ind=[];

while 1
    tline = fgetl(fid);
    if ~ischar(tline)
        break
    end
%    tline
    
    token=regexp(tline, 'Seq_Frame[0-9]*_FrameNumber = (.*)', 'tokens');
    if (length(token)>0)
        n=str2num(cell2mat(token{1}));
        ind=[ind; n];
    end

    token=regexp(tline, 'Seq_Frame[0-9]*_UnfilteredTimestamp = (.*)', 'tokens');
    if (length(token)>0)
        n=str2num(cell2mat(token{1}));
        ts=[ts; n];
    end

    if (regexp(tline, 'ElementDataFile')>0)
        break
    end
        
end

