function igtlConnection = igtlDisconnect(igtlConnection)

if ~isempty(igtlConnection.socket)
    igtlConnection.socket.close();
    igtlConnection.socket=[];
end
