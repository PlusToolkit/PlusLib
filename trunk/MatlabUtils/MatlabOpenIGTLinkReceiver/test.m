%% Simple test script for receiving transforms from an OpenIGTLink server

% Connect to the OpenIGTLink server
igtlConnection = igtlConnect('127.0.0.1',18944);

% Test options
numberOfTransformsToReceive=100;
displayReceivedTransforms=1;

% Initialize variables for statistics computation and data display
startTime=-1;
receiveStartTime=tic();

for transformIndex=1:numberOfTransformsToReceive    

    % Get a transform
    transform = igtlReceiveTransform(igtlConnection);
    
    % Display the received transform
    if ~isempty(transform.matrix)
        % Compute relative timestamp (from the start of the script)
        if startTime<0
            startTime=transform.timestamp;
            relativeTime=0;
        else
            relativeTime=transform.timestamp-startTime;
        end        
        % Print name, transform, matrix
        if displayReceivedTransforms
          disp(['------------ ',transform.name,' (',num2str(relativeTime),') ------------']);
          disp(transform.matrix);
        end
    else
        % No transforms are received, probably timeout
        disp('No transforms are available');
    end
    
end

% Display some statistics
elapsedTime=toc(receiveStartTime);
disp(['Received ',num2str(numberOfTransformsToReceive),' transforms in ',num2str(elapsedTime),' sec: ',num2str(numberOfTransformsToReceive/elapsedTime),' transforms/sec']);
disp(['Timestamp span: ',num2str(relativeTime)]);

% Disconnect from the OpenIGTLink server
igtlDisconnect(igtlConnection);
