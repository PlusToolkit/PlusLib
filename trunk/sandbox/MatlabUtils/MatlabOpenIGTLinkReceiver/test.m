igtlConnection = igtlConnect('127.0.0.1',18944);

for transformIndex=1:30
    disp('-------------------')
    transform = igtlReceiveTransform(igtlConnection);
    transform.name
    transform.matrix
    transform.timestamp
end

igtlConnection = igtlDisconnect(igtlConnection);
