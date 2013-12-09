igtlConnection = igtlConnect('127.0.0.1',18944);

startTime=-1;

for transformIndex=1:30
    disp('-------------------')
    transform = igtlReceiveTransform(igtlConnection);
    transform.name
    transform.matrix
    if startTime<0
        startTime=double(transform.timestamp)
    else
        double(transform.timestamp)-startTime
    end
    
end

igtlConnection = igtlDisconnect(igtlConnection);
