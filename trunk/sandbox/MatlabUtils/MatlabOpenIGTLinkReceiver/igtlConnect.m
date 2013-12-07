function igtlConnection = igtlConnect(hostname, port)
% igtlConnect  Connect to an OpenIGTLink server
%
%   igtlConnection = igtlConnect(hostname, port) 
%
%   hostname: name or IP address of the server host (default: 'localhost')
%   port: IP port number of the server (default: 18944)

import java.net.Socket
import java.io.*
%import java.net.ClientSocket

igtlConnection={};
igtlConnection.host='localhost';
if (nargin>1)
    igtlConnection.host=hostname;
end    
igtlConnection.port=18944;
if (nargin>1)
    igtlConnection.port=port;
end
igtlConnection.timeout=1000;

try
    igtlConnection.socket = Socket(igtlConnection.host,igtlConnection.port);
    % in = BufferedReader(InputStreamReader(sock.getInputStream));
    % out = PrintWriter(sock.getOutputStream,true);
catch ME
    error('Failed to connect to server: %s', ME.message);
end

igtlConnection.socket.setSoTimeout(igtlConnection.timeout);

igtlConnection.outputStream = igtlConnection.socket.getOutputStream;
igtlConnection.inputStream = igtlConnection.socket.getInputStream;
igtlConnection.messageHeaderReceiveTimeoutSec=5;
igtlConnection.messageBodyReceiveTimeoutSec=25;
