function timestamp = igtlTimestampNow()
% igtlTimestampNow  Time elapsed since 00:00:00 January 1, 1970, UTC, in seconds
%
%   timestamp = igtlTimestampNow()
%
%  Example:
%
%   igtlConnection = igtlConnect('127.0.0.1',18944);
%   transform.name = 'NeedleToTracker';
%   transform.matrix = [ 1 0 0 10; 0 1 0 -5; 0 0 1 20; 0 0 0 1 ];
%   transform.timestamp = igtlTimestampNow();
%   igtlSendTransform(igtlConnection, transform);
%   igtlDisconnect(igtlConnection);
%

timestamp = java.lang.System.currentTimeMillis/1000;
