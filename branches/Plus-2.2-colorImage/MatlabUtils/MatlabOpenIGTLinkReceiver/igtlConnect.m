function igtlConnection = igtlConnect(hostname, port)
% igtlConnect  Connect to an OpenIGTLink server
%
%   igtlConnection = igtlConnect(hostname, port) 
%
%   hostname: name or IP address of the server host (default: 'localhost')
%   port: IP port number of the server (default: 18944)
%

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

try
    igtlConnection.socket = Socket(igtlConnection.host,igtlConnection.port);
    % in = BufferedReader(InputStreamReader(sock.getInputStream));
    % out = PrintWriter(sock.getOutputStream,true);
catch ME
    error('Failed to connect to OpenIGTLink server at %s:%i.\n\n%s', igtlConnection.host, igtlConnection.port, ME.message);
end

% Set socket data reception timeout to 1 sec by default
igtlConnection.socket.setSoTimeout(1000);

igtlConnection.inputStream = igtlConnection.socket.getInputStream;
igtlConnection.dataInputStream = DataInputStream(igtlConnection.inputStream);
igtlConnection.messageHeaderReceiveTimeoutSec=5;
igtlConnection.messageBodyReceiveTimeoutSec=50;
