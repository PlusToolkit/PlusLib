function igtlDisconnect(igtlConnection)
% igtlDisconnect  Disconnect from the OpenIGTLink server
%
%   igtlDisconnect(igtlConnection)
%
%   igtlConnection: connection returned by igtlConnect(...)
%

if ~isempty(igtlConnection.socket)
    igtlConnection.socket.close();
    igtlConnection.socket=[];
end
