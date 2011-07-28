function [fts uts ind]=readmhatimestamp(fname)

% readmhatimestamp: Read filtered and unfiltered timestamps and frame indexes from a sequence metafile header
%
%   Description
%     This is a helper function for reading frame indexes and corresponding
%     timestamps from a sequence metafile (mha). This can be used for
%     investigating potential item timestamping issues.
%     The [uts, ind] plot is a straight line if the frame rate is constant.
%
%   Example
%     [fts uts ind]=readmhatimestamp('01_Heart_NDI_Certus_Buffer_01.mha'); 
%     plot(uts, ind, '.')
%

fid = fopen(fname);

fts=[];
uts=[];
ind=[];

while 1
    tline = fgetl(fid);
    if ~ischar(tline)
        break
    end
    
    token=regexp(tline, 'Seq_Frame[0-9]*_FrameNumber = (.*)', 'tokens');
    if (length(token)>0)
        n=str2num(cell2mat(token{1}));
        ind=[ind; n];
    end

    token=regexp(tline, 'Seq_Frame[0-9]*_UnfilteredTimestamp = (.*)', 'tokens');
    if (length(token)>0)
        n=str2num(cell2mat(token{1}));
        uts=[uts; n];
    end

    token=regexp(tline, 'Seq_Frame[0-9]*_Timestamp = (.*)', 'tokens');
    if (length(token)>0)
        n=str2num(cell2mat(token{1}));
        fts=[fts; n];
    end
   
    if (regexp(tline, 'ElementDataFile')>0)
        break
    end
        
end

