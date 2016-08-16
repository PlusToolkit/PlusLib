igtlConnection = igtlConnect('127.0.0.1',18944);
transform.name = 'NeedleToTracker';

startTime = igtlTimestampNow();

for t=1:1000
  t=igtlTimestampNow()-startTime;
  transform.matrix = [ 1 0 0 12+30*sin(t*0.5); 0 1 0 -5; 0 0 1 20; 0 0 0 1 ];
  transform.timestamp = igtlTimestampNow();
  transform
  igtlSendTransform(igtlConnection, transform);
  pause(0.1)
end

igtlDisconnect(igtlConnection);
