% OpenIGTLink server that executes the received string commands
function cli_commandserver(port)

    global OPENIGTLINK_SERVER_SOCKET
    
    import java.net.Socket
    import java.io.*
    import java.net.ServerSocket
    
    % Add current directory to the path so that all cli_* functions will be available even when the current working directory is changed
    addpath(pwd);
    
    serverSocketInfo.port=4100;
    serverSocketInfo.timeout=1000;

    if (nargin>0)
        serverSocketInfo.port=port;
    end

    disp(['Starting OpenIGTLink command server at port ' num2str(serverSocketInfo.port)]);    

    % Open a TCP Server Port
    if (exist('OPENIGTLINK_SERVER_SOCKET','var'))
        if (not(isempty(OPENIGTLINK_SERVER_SOCKET)))
          % Socket has not been closed last time
          disp('Socket has not been closed properly last time. Closing it now.');
          OPENIGTLINK_SERVER_SOCKET.close;
          OPENIGTLINK_SERVER_SOCKET=[];
        end
    end

    try
        serverSocketInfo.socket = ServerSocket(serverSocketInfo.port);        
        OPENIGTLINK_SERVER_SOCKET=serverSocketInfo.socket;
    catch 
        error('Failed to open server port. Make sure the port is not open already or blocked by firewall.');
    end        
    serverSocketInfo.socket.setSoTimeout(serverSocketInfo.timeout);

    disp('Waiting for client connections...');
    
    % Handle client connections
    while(true)

        % Wait for client connection
        drawNowCounter=0;
        while(true),            
            try 
              clientSocketInfo.socket = serverSocketInfo.socket.accept;  
              break; 
            catch
            end
            if (drawNowCounter>10)
              drawnow
              drawNowCounter=0;
            end;
            drawNowCounter=drawNowCounter+1;
            pause(0.5);
        end

        % Client connected
        disp('Client connected')
        % Rehash forces re-reading of all Matlab functions from files
        rehash        
        
        clientSocketInfo.remoteHost = char(clientSocketInfo.socket.getInetAddress);
        clientSocketInfo.outputStream = clientSocketInfo.socket.getOutputStream;
        clientSocketInfo.inputStream = clientSocketInfo.socket.getInputStream;       
        clientSocketInfo.messageHeaderReceiveTimeoutSec=5;
        clientSocketInfo.messageBodyReceiveTimeoutSec=25;

        % Read command
        receivedMsg=ReadOpenIGTLinkStringMessage(clientSocketInfo);
        if(~isempty(receivedMsg) && ~isempty(receivedMsg.string))
            dataType=deblank(char(receivedMsg.dataTypeName));
            deviceName=deblank(char(receivedMsg.deviceName));
            cmd=deblank(char(receivedMsg.string));
            replyDeviceName='ACK';
            if (~strcmp(dataType,'STRING'))
              response=['ERROR: Expected STRING data type, received data type: [',dataType,']'];
            elseif (length(deviceName)<3 || ~strcmp(deviceName(1:3),'CMD'))
              response=['ERROR: Expected device name starting with CMD. Received device name: [',deviceName,']'];
            elseif (isempty(cmd))
              response='ERROR: Received empty command string';
            else
              % Reply device name for CMD is ACQ, for CMD_someuid is ACK_someuid
              replyDeviceName=deviceName;
              replyDeviceName(1:3)='ACK';
              try
                disp([' Execute command: ',cmd]);
                response=evalc(cmd);
                if (isempty(response))
                  % Replace empty response by OK to indicate success
                  response='OK';
                end
                disp(' Command execution completed successfully');
              catch ME
                response=['ERROR: Command execution failed. ',ME.getReport('extended','hyperlinks','off')];
              end
            end
        else
            response='ERROR: Error while receiving the command';            
        end        
        
        % Send reply
        responseStr=num2str(response);
        disp([' Response (sent to device ',replyDeviceName,'): ', responseStr]);
        WriteOpenIGTLinkStringMessage(clientSocketInfo, responseStr, replyDeviceName);

        % Close connection
        clientSocketInfo.socket.close;
        clientSocketInfo.socket=[];
        disp('Client connection closed');

    end

    % Close server socket
    serverSocketInfo.socket.close;
    serverSocketInfo.socket=[];

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

%%  Parse OpenIGTLink messag header
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
