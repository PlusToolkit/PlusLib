% Algorithm 4
% Minimization over Position, Orientation, and Arc lengths 
%
% created: Nov 30, 2000
% modified: Mar 4, 2002
% Sangyoon Lee

clear all
flops(0)


% Loading data files
%slice = 4;	% Image slice number
p = load('Kel_p9.dat');	% Position vectors of rods
v = load('Kel_v9.dat');	% Direction vectors of rods
%y = load('CRW_y2_9.dat');	% Coordinates of fiducials in image plane
y = load('Syn_y4_9.dat');	% Coordinates of fiducials in image plane
R0 = [0.9848 0.0000 -0.1736; 0.0000 1.0000 0.0000; 0.1736 0.0000 0.9848];
b0 = [19.3969 -20.0000 5.4202]';
N = size(p,2)	% # of rods


% Initializations
R = eye(3);	% Initial rotation matrix
limit = 100;	% # of iterations
iter = 1:1:limit';
error1 = zeros(limit, 1);	% To store error at each iteration
error2 = zeros(limit, 2);	% To store error at each iteration

% Calculation by iteration
cnt = 1;
while cnt <= limit
   lv = zeros(3*N, 1);
   mat = zeros(3*N, N+6);
   for i = 1:N
      lv(3*i-2) = p(1,i) - dot(R(1,:), y(:,i));
      lv(3*i-1) = p(2,i) - dot(R(2,:), y(:,i));
      lv(3*i) = p(3,i) - dot(R(3,:), y(:,i));
      
      % Skew symmetric matrix corresponding to vector y
      skew_Y = [0 -y(3,i) y(2,i); y(3,i) 0 -y(1,i); -y(2,i) y(1,i) 0];
      mat(3*i-2, i) = -v(1,i);
      mat(3*i-1, i) = -v(2,i);
      mat(3*i, i) = -v(3,i);
      mat((3*i-2):(3*i), (N+1):(N+3)) = -R * skew_Y;
      mat((3*i-2):(3*i), (N+4):(N+6)) = eye(3);
   end
   q = (inv(mat' * mat)) * (mat' * lv);
   s = q(1:N);	% Arc lengths
   av = q((N+1):(N+3));	% Angular velocity
   b = q((N+4):(N+6));	% Translation vector
   
   % Error
   for i = 1:N
      tmp_e(:,i) = (p(:,i) + s(i) * v(:,i)) - (R * y(:,i) + b);
      error1(cnt) = error1(cnt) + norm(tmp_e(:,i));
   end
   error1(cnt) = error1(cnt) / N;
   error2(cnt, :) = [sqrt(6 - 2 * trace(R0' * R)) norm(b0 - b)]; % NOTE
   
   % Update
   R = rot_up2_3(R, av);
   cnt = cnt + 1;
end
flops_is = flops
psv_Ryb = error1(limit)
dist_se3 = error2(limit, :)
R
q_is = q'






% Plots
figure(1)	% Error vs # of iterations
plot(iter, error1, 'k-')
xlabel('Number of iterations')
ylabel('Error (cm)')
%logerror = log(error);
%plot(iter, logerror, 'k:')
%plot(iter, logerror, 'k-')
%xlabel('Number of iterations')
%ylabel('log(Error)')
%tit = sprintf('Algorithm 3 for 6*1 approach, slice %d', slice);
%title(tit)
%legend('solid: synthetic data', 'dotted: real data')
grid on
