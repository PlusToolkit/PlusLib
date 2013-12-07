igtlConnection = igtlConnect('127.0.0.1',18944);

transform = igtlReceiveTransform(igtlConnection);
transform.name
transform.matrix
transform.timestamp

igtlConnection = igtlDisconnect(igtlConnection);
