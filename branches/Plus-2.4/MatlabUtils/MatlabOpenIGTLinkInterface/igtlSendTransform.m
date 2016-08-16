function igtlSendTransform(igtlConnection, transform)
% igtlSendTransform  Sends one transform to the OpenIGTLink server
%
%   igtlReceiveTransform(igtlConnection, transform)
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
%       relative to 00:00:00 January 1, 1970, UTC. If not defined then
%       the message is sent with the timestamp of the current time.
%
%  To change the timeout value (maximum time to wait for a message), call
%  the setSoTimeout of the igtlConnection socket. For example, enter the
%  following for a 1sec timeout value:
%
%    igtlConnection.socket.setSoTimeout(1000);
%
%  Example:
%
%   igtlConnection = igtlConnect('127.0.0.1',18944);
%   transform.name = 'NeedleToTracker';
%   transform.matrix = [ 1 0 0 10; 0 1 0 -5; 0 0 1 20; 0 0 0 1 ];
%   igtlSendTransform(igtlConnection, transform);
%   igtlDisconnect(igtlConnection);
%

    if ~isfield(transform, 'timestamp')
        transform.timestamp = igtlTimestampNow();
    end
    
    % version number
    % note that it is an unsigned short value, but small positive signed and unsigned numbers are represented the same way, so we can use writeShort
    igtlConnection.dataOutputStream.writeShort(1);
    writePaddedString(igtlConnection,'TRANSFORM',12); % message type
    writePaddedString(igtlConnection,transform.name,20); % device name
    socketWriteUint64(igtlConnection,packTimestampUint64(transform.timestamp)); % timestamp
    socketWriteUint64(igtlConnection,48); % body size    
    
    msgBody = single([
        transform.matrix(1,1)  % R11
        transform.matrix(2,1)  % R21
        transform.matrix(3,1)  % R31
        transform.matrix(1,2)  % R12
        transform.matrix(2,2)  % R22
        transform.matrix(3,2)  % R32
        transform.matrix(1,3)  % R13
        transform.matrix(2,3)  % R23
        transform.matrix(3,3)  % R33
        transform.matrix(1,4)  % TX
        transform.matrix(2,4)  % TY
        transform.matrix(3,4) ]); % TZ
    
    % Compute and write body CRC
    crc=0;
    for k=1:length(msgBody)
        crc=igtlComputeCrc(msgBody(k),crc);
    end
    socketWriteUint64(igtlConnection, crc);
    
    % Write body
    for k=1:length(msgBody)
        igtlConnection.dataOutputStream.writeFloat(msgBody(k));
    end
       
    igtlConnection.dataOutputStream.flush()

end

%% OpenIGTLink socket communication helper functions

% Write a string padded with zeros
function writePaddedString(igtlConnection,str,lengthWithPadding)
    numberOfStringChars = min(length(str), lengthWithPadding);
    for b=1:numberOfStringChars igtlConnection.dataOutputStream.writeByte(int8(str(b))); end;
    for b=1:(lengthWithPadding-numberOfStringChars) igtlConnection.dataOutputStream.writeByte(0); end;
end

% Write a 64-bit unsigned int to the socket
% This helper function is needed because igtlConnection.dataOutputStream.writeUnsignedLong()
% does not work correctly for large numbers (because probably the long value is converted to double)
function socketWriteUint64(igtlConnection, uint64value)
    int8Vector = typecast(uint64(uint64value),'int8');
    for b=8:-1:1 igtlConnection.dataOutputStream.writeByte(int8Vector(b)); end;
end

% Retrieve timestamp information from the 64-bit OpenIGTLink timestamp
% Timestamps are represented as a 64-bit unsigned fixed-point number.
% http://wiki.na-mic.org/Wiki/index.php/OpenIGTLink/Timestamp
function timestampUint64 = packTimestampUint64(timestampFloat)
    % TODO: compute actual timestamp
    timestampUint64 = uint64(0);
end
