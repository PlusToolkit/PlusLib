function transform = igtlReceiveTransform(igtlConnection)

    transform={};
    msg=ReadOpenIGTLinkTransformMessage(igtlConnection);
    if(~isempty(msg) && ~isempty(msg.transformMatrix))
        transform.name=msg.transformName;
        transform.matrix=msg.transformMatrix;
        transform.timestamp=msg.timestamp;
    end

end

%% OpenIGTLink helper functions

function msg=ReadOpenIGTLinkTransformMessage(clientSocket)
    msg=ReadOpenIGTLinkMessage(clientSocket);
    if (length(msg.body)<5)
        disp('Error: TRANSFORM message received with incomplete contents')
        msg.transformName=[];
        msg.transformMatrix=[];
        return
    end
    msg.transformName=msg.deviceName;
    msg.transformMatrix=diag([1 1 1 1]);
    % http://openigtlink.org/protocols/v2_transform.html
    msg.transformMatrix(1,1)=convertFromUint8VectorToFloat32(msg.body(1:4));    % R11
    msg.transformMatrix(2,1)=convertFromUint8VectorToFloat32(msg.body(5:8));    % R21
    msg.transformMatrix(3,1)=convertFromUint8VectorToFloat32(msg.body(9:12));   % R31
    msg.transformMatrix(1,2)=convertFromUint8VectorToFloat32(msg.body(13:16));  % R12
    msg.transformMatrix(2,2)=convertFromUint8VectorToFloat32(msg.body(17:20));  % R22
    msg.transformMatrix(3,2)=convertFromUint8VectorToFloat32(msg.body(21:24));  % R32
    msg.transformMatrix(1,3)=convertFromUint8VectorToFloat32(msg.body(25:28));  % R13
    msg.transformMatrix(2,3)=convertFromUint8VectorToFloat32(msg.body(29:32));  % R23
    msg.transformMatrix(3,3)=convertFromUint8VectorToFloat32(msg.body(33:36));  % R33
    msg.transformMatrix(1,4)=convertFromUint8VectorToFloat32(msg.body(37:40));  % TX
    msg.transformMatrix(2,4)=convertFromUint8VectorToFloat32(msg.body(41:44));  % TY
    msg.transformMatrix(3,4)=convertFromUint8VectorToFloat32(msg.body(45:48));  % TZ
end

function msg=ReadOpenIGTLinkStringMessage(clientSocket)
    msg=ReadOpenIGTLinkMessage(clientSocket);
    if (length(msg.body)<5)
        disp('Error: STRING message received with incomplete contents')
        msg.string='';
        return
    end        
    strMsgEncoding=convertFromUint8VectorToUint16(msg.body(1:2));
    if (strMsgEncoding~=3)
        disp(['Warning: STRING message received with unknown encoding ',num2str(strMsgEncoding)])
    end
    strMsgLength=convertFromUint8VectorToUint16(msg.body(3:4));
    msg.string=char(msg.body(5:4+strMsgLength));
end

function msg=ReadOpenIGTLinkMessage(clientSocket)
    openIGTLinkHeaderLength=58;
    headerData=ReadWithTimeout(clientSocket, openIGTLinkHeaderLength, clientSocket.messageHeaderReceiveTimeoutSec);
    if (length(headerData)==openIGTLinkHeaderLength)
        msg=ParseOpenIGTLinkMessageHeader(headerData);
        msg.body=ReadWithTimeout(clientSocket, msg.bodySize, clientSocket.messageBodyReceiveTimeoutSec);            
    else
        error('ERROR: Timeout while waiting receiving OpenIGTLink message header')
    end
end    
        
function result=WriteOpenIGTLinkStringMessage(clientSocket, msgString, deviceName)
    msg.dataTypeName='STRING';
    msg.deviceName=deviceName;
    msg.timestamp=0;
    msgString=[uint8(msgString) uint8(0)]; % Convert string to uint8 vector and add terminator character
    msg.body=[convertFromUint16ToUint8Vector(3),convertFromUint16ToUint8Vector(length(msgString)),msgString];
    result=WriteOpenIGTLinkMessage(clientSocket, msg);
end

% Returns 1 if successful, 0 if failed
function result=WriteOpenIGTLinkMessage(clientSocket, msg)
    import java.net.Socket
    import java.io.*
    import java.net.ServerSocket
    % Add constant fields values
    msg.versionNumber=1;
    msg.bodySize=length(msg.body);
    msg.bodyCrc=0; % TODO: compute this
    % Pack message
    data=[];
    data=[data, convertFromUint16ToUint8Vector(msg.versionNumber)];
    data=[data, padString(msg.dataTypeName,12)];
    data=[data, padString(msg.deviceName,20)];
    data=[data, convertFromInt64ToUint8Vector(msg.timestamp)];
    data=[data, convertFromInt64ToUint8Vector(msg.bodySize)];
    data=[data, convertFromInt64ToUint8Vector(msg.bodyCrc)];
    data=[data, uint8(msg.body)];    
    result=1;
    try
        DataOutputStream(clientSocket.outputStream).write(uint8(data),0,length(data));
    catch ME
        disp(ME.message)
        result=0;
    end
    try
        DataOutputStream(clientSocket.outputStream).flush;
    catch ME
        disp(ME.message)
        result=0;
    end
    if (result==0)
      disp('Sending OpenIGTLink message failed');
    end
end

function data=ReadWithTimeout(clientSocket, requestedDataLength, timeoutSec)
    import java.net.Socket
    import java.io.*
    import java.net.ServerSocket

    % preallocate to improve performance
    data=zeros(1,requestedDataLength,'uint8');
    signedDataByte=int8(0);
    bytesRead=0;
    while(bytesRead<requestedDataLength)    
        % Computing (requestedDataLength-bytesRead) is an int64 operation, which may not be available on Matlab R2009 and before
        int64arithmeticsSupported=~isempty(find(strcmp(methods('int64'),'minus')));
        if int64arithmeticsSupported
            % Full 64-bit arithmetics
            bytesToRead=min(clientSocket.inputStream.available, requestedDataLength-bytesRead);
        else
            % Fall back to floating point arithmetics
            bytesToRead=min(clientSocket.inputStream.available, double(requestedDataLength)-double(bytesRead));
        end  
        if (bytesRead==0 && bytesToRead>0)
            % starting to read message header
            tstart=tic;
        end
        for i = bytesRead+1:bytesRead+bytesToRead
            signedDataByte = DataInputStream(clientSocket.inputStream).readByte;
            if signedDataByte>=0
                data(i) = signedDataByte;
            else
                data(i) = bitcmp(-signedDataByte,8)+1;
            end
        end            
        bytesRead=bytesRead+bytesToRead;
        if (bytesRead>0 && bytesRead<requestedDataLength)
            % check if the reading of the header has timed out yet
            timeElapsedSec=toc(tstart);
            if(timeElapsedSec>timeoutSec)
                % timeout, it should not happen
                % remove the unnecessary preallocated elements
                data=data(1:bytesRead);
                break
            end
        end
    end
end

%%  Parse OpenIGTLink message header
% http://openigtlink.org/protocols/v2_header.html    
function parsedMsg=ParseOpenIGTLinkMessageHeader(rawMsg)
    parsedMsg.versionNumber=convertFromUint8VectorToUint16(rawMsg(1:2));
    parsedMsg.dataTypeName=char(rawMsg(3:14));
    parsedMsg.deviceName=char(rawMsg(15:34));
    parsedMsg.timestamp=convertFromUint8VectorToInt64(rawMsg(35:42));
    parsedMsg.bodySize=convertFromUint8VectorToInt64(rawMsg(43:50));
    parsedMsg.bodyCrc=convertFromUint8VectorToInt64(rawMsg(51:58));
end

function result=convertFromUint8VectorToUint16(uint8Vector)
  result=int32(uint8Vector(1))*256+int32(uint8Vector(2));
end 

function result=convertFromUint8VectorToInt64(uint8Vector)
  multipliers = [256^7 256^6 256^5 256^4 256^3 256^2 256^1 1];
  % Matlab R2009 and earlier versions don't support int64 arithmetics.
  int64arithmeticsSupported=~isempty(find(strcmp(methods('int64'),'mtimes')));
  if int64arithmeticsSupported
    % Full 64-bit arithmetics
    result = sum(int64(uint8Vector).*int64(multipliers));
  else
    % Fall back to floating point arithmetics: compute result with floating
    % point type and convert the end result to int64
    % (it should be precise enough for realistic file sizes)
    result = int64(sum(double(uint8Vector).*multipliers));
  end  
end 

function result=convertFromUint8VectorToFloat32(uint8Vector)
  uintResult = uint32(uint8Vector(4)) + uint32(uint8Vector(3))*256 + uint32(uint8Vector(2))*256^2 + uint32(uint8Vector(1))*256^3;
  result = typecast(uintResult, 'single');
end

function selectedByte=getNthByte(multibyte, n)
  selectedByte=uint8(mod(floor(multibyte/256^n),256));
end

function result=convertFromUint16ToUint8Vector(uint16Value)
  result=[getNthByte(uint16Value,1) getNthByte(uint16Value,0)];
end 

function result=convertFromInt64ToUint8Vector(int64Value)
  result=zeros(1,8,'uint8');
  result(1)=getNthByte(int64Value,7);
  result(2)=getNthByte(int64Value,6);
  result(3)=getNthByte(int64Value,5);
  result(4)=getNthByte(int64Value,4);
  result(5)=getNthByte(int64Value,3);
  result(6)=getNthByte(int64Value,2);
  result(7)=getNthByte(int64Value,1);
  result(8)=getNthByte(int64Value,0);
end 

function paddedStr=padString(str,strLen)
  paddedStr=str(1:min(length(str),strLen));
  paddingLength=strLen-length(paddedStr);
  if (paddingLength>0)
      paddedStr=[paddedStr,zeros(1,paddingLength,'uint8')];
  end
end
