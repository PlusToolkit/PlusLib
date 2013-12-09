function transform = igtlReceiveTransform(igtlConnection)
% igtlReceiveTransform  Receives one transform from the OpenIGTLink server
%
%   transform = igtlReceiveTransform(igtlConnection)
%
%   igtlConnection: connection returned by igtlConnect(...)
%
%   transform: structure array containing the following fields
%
%     name: name of the transform (from the OpenIGTLink device name)
%
%     matrix: 4x4 homogeneous transformation matrix (empty, if failed to
%       read any transform, e.g., due to timeout while waiting for a
%       transform message)
%
%     timestamp: timestamp of the data acquisition, in seconds,
%       relative to 00:00:00 January 1, 1970, UTC.
%
%  To change the timeout value (maximum time to wait for a message), call
%  the setSoTimeout of the igtlConnection socket. For example, enter the
%  following for a 1sec timeout value:
%
%    igtlConnection.socket.setSoTimeout(1000);
%

    transform.name=[];
    transform.matrix=[];
    transform.timestamp=[];

    try

        % Read messages until a TRANSFORM message is available
        % (all other message types are ignored)
        while (true)
            msg=ReadOpenIGTLinkMessageHeader(igtlConnection);
            dataType=deblank(char(msg.dataTypeName));
            if isempty(dataType)
                % there is no message available, return to give chance for
                % processing            
                return
            end
            if strcmp(dataType,'TRANSFORM')
                % received a TRANSFORM message -> process it            
                break
            end
            % received a non-TRANSFORM message -> ignore it and read the next one
            IgnoreOpenIGTLinkMessageBody(igtlConnection, msg);
        end

        % Check if the body size is consistent with the number of bytes that we read from the stream
        % http://openigtlink.org/protocols/v2_transform.html
        if (msg.bodySize ~= 48)
            error('TRANSFORM message received with invalid body length')
            return
        end

        % Fill info from header
        transform.name=deblank(char(msg.deviceName));
        transform.timestamp=msg.timestamp;

        % Read matrix
        transform.matrix=diag([1 1 1 1]);    
        transform.matrix(1,1)=igtlConnection.dataInputStream.readFloat();  % R11
        transform.matrix(2,1)=igtlConnection.dataInputStream.readFloat();  % R21
        transform.matrix(3,1)=igtlConnection.dataInputStream.readFloat();  % R31
        transform.matrix(1,2)=igtlConnection.dataInputStream.readFloat();  % R12
        transform.matrix(2,2)=igtlConnection.dataInputStream.readFloat();  % R22
        transform.matrix(3,2)=igtlConnection.dataInputStream.readFloat();  % R32
        transform.matrix(1,3)=igtlConnection.dataInputStream.readFloat();  % R13
        transform.matrix(2,3)=igtlConnection.dataInputStream.readFloat();  % R23
        transform.matrix(3,3)=igtlConnection.dataInputStream.readFloat();  % R33
        transform.matrix(1,4)=igtlConnection.dataInputStream.readFloat();  % TX
        transform.matrix(2,4)=igtlConnection.dataInputStream.readFloat();  % TY
        transform.matrix(3,4)=igtlConnection.dataInputStream.readFloat();  % TZ
    catch ME
        % If there is an error (e.g., timeout due to no more transforms
        % available) then just make sure that an empty matrix is returned
        transform.matrix=[];
    end
 
end

%% OpenIGTLink socket communication helper functions

% Read OpenIGTLink message header
% http://openigtlink.org/protocols/v2_header.html    
function parsedMsg=ReadOpenIGTLinkMessageHeader(igtlConnection)
    parsedMsg.versionNumber=igtlConnection.dataInputStream.readUnsignedShort();   
    for b=1:12 parsedMsg.dataTypeName(b)=char(igtlConnection.dataInputStream.readUnsignedByte()); end;
    for b=1:20 parsedMsg.deviceName(b)=char(igtlConnection.dataInputStream.readUnsignedByte()); end;    
    [sec nanosec] = unpackTimestampUint64(socketReadUint64(igtlConnection));
    parsedMsg.timestamp=double(sec)+double(1e-9)*double(nanosec);
    parsedMsg.bodySize=socketReadUint64(igtlConnection);
    parsedMsg.bodyCrc=socketReadUint64(igtlConnection);
end

% Skip the message body so that the next message can be received 
function IgnoreOpenIGTLinkMessageBody(igtlConnection, msg)
    igtlConnection.dataInputStream.skipBytes(int32(msg.bodySize));
end

% Read a 64-bit unsigned int from the socket
% This helper function is needed because igtlConnection.dataInputStream.readUnsignedLong()
% does not return correct results for large numbers (because probably the long
% value is converted to double)
function result=socketReadUint64(igtlConnection)
    uint8Vector=zeros(8,1,'uint8');
    for b=8:-1:1 uint8Vector(b)=igtlConnection.dataInputStream.readUnsignedByte(); end;
    result = typecast(uint8Vector,'uint64');
end

% Read a 32-bit unsigned int from the socket
% This helper function is needed because igtlConnection.dataInputStream.readUnsignedLong()
% does can only read signed 32-bit int
function result=socketReadUnsignedInt32(igtlConnection)
    signedResult = igtlConnection.dataInputStream.readInt();
    if signedResult>=0
        result = signedResult;
    else
        result = bitcmp(-signedResult,32)+1;
    end
    result = signedResult;
end

% Retrieve timestamp information from the 64-bit OpenIGTLink timestamp
% Timestamps are represented as a 64-bit unsigned fixed-point number.
% http://wiki.na-mic.org/Wiki/index.php/OpenIGTLink/Timestamp
function [sec nanosec] = unpackTimestampUint64(timestamp)
    sec = uint32(bitshift(timestamp,-32)); % high 32 bits
    frac = bitand(timestamp, 4294967295); % low 32 bits
    nanosec = uint32(0);
    convBase = uint32(1e9);
    mask = bitshift(1,31);
    for b=0:31
        convBase = bitshift(convBase,-1);
        if bitand(frac,mask)
            nanosec = nanosec + convBase;
        end
        mask = bitshift(mask,-1);
    end    
end
