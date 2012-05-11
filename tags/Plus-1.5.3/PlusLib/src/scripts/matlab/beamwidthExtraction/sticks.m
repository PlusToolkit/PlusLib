% http://www.mathworks.de/matlabcentral/fileexchange/14862-sticks-filter
% Version from 03 May 2007

function s = sticks(n,k)
%STICKS  Sticks Filter Generation
%   STICKS(N,K) returns a set of sticks filters
%   N is the stick length
%   K is the sticks' thickness
%   Exemple : sticks(5,1) provide 8 5x5 matrix with stick thickness equal
%   to one

% 04/07 T. Mervin

% check that args are odd positive numbers and that 
% the sticks' thickness does not exceed filter size
if (mod(n,2) == 0) || (mod(k,2) == 0) || (k > n)
    disp('Wrong arguments. Expecting odd positive numbers, k < n')
else
    % N x N x 2N-2 matrix for the 2*N-2 sticks
    s = zeros(n,n,2*n-2); 
    
    t = pi/(2*n-2); % angular gap pi/(2n-2)
    s0 = 1/n;       
    m = round(n/2); 
    l = (k-1)/2;    

    % particular cases :  tan(pi/2) and tan(0)
    s(m-l:m+l,:,1) = s0; % horizontal, tan(0)
    s(:,m-l:m+l,n) = s0; % vertical, tan(pi/2)

    for j = 2 : (n-1)/2 + 1,
        for i = 1 : n,
            a = round(tan((j-1)*t)*(i-m) + m); % ]0,pi/4]
            b = round(-tan((j-1)*t)*(i-m) + m); % symmetry
            c = round(cot(((n-1)/2+j-1)*t)*(i-m) + m); % beyond pi/4
            d = round(-cot(((n-1)/2+j-1)*t)*(i-m) + m); % symmetry
            
            A1 = a-l;
            A2 = a+l;
            B1 = b-l;
            B2 = b+l;
            
            C1 = c-l;
            C2 = c+l;
            D1 = d-l;
            D2 = d+l;
            
            if A1 <= 0     % we check that the determined value 
                A1 = 1;    % don't exceed the matrix size
            end            % we truncate the values if it is so
            if A2 > n
                A2 = n;
            end
            if B1 <= 0
                B1 = 1;
            end
            if B2 > n
                B2 = n;
            end
            if C1 <= 0
                C1 = 1;
            end
            if C2 > n 
                C2 = n;
            end
            if D1 <= 0
                D1 = 1;
            end
            if D2 > n
                D2 = n;
            end
            
            s(A1:A2,i,j) = s0;
            s(B1:B2,i,2*n-j) = s0;

            s(i,C1:C2,(n-1)/2+j) = s0;
            s(i,D1:D2,2*n-((n-1)/2+j)) = s0;
        end
    end
end