% Algorithm 3 
% Rate-Linearization of Position and Orientation
%
% Created: Dec 1, 2000
% Modified: Mar 4, 2002
% Sangyoon Lee

clear all
flops(0)

% Loading data files
%slice = 4	% Image slice number
p = load('Kel_p9.dat');	% Position vectors of rods
v = load('Kel_v9.dat');	% Direction vectors of rods
%y = load('CRW_trs_y43_9.dat');	% Coordinates of fiducials on image plane
y = load('syn_y4_9.dat');	% Coordinates of fiducials on image plane
R0 = [0.9848 0.0000 -0.1736; 0.0000 1.0000 0.0000; 0.1736 0.0000 0.9848];
b0 = [19.3969 -20.0000 5.4202]';
N = size(p,2)	% # of rods


% Initializations
R = eye(3);	% Initial rotation matrix
%R = [-1 0 0; 0 0 1; 0 1 0];	% Initial rotation matrix
b = [5 5 5]'; % Initial translation vector
alpha = 1;	% Positive constant for (deldot) = -(alpha)*(del)
delt = 0.1;	% Time increment
limit = 100;	% # of iterations
iter = 1:1:limit';
error1 = zeros(limit, 1);	% To store error at each iteration
error2 = zeros(limit, 2);	% To store error at each iteration

% Calculation by iteration (velocity vector is calculated)
cnt = 1;
while cnt <= limit
   mat1 = zeros(3*N, 3*N);	% Main equation is 
   mat2 = zeros(3*N, 6);	% (mat1)*(mat2)*(l_vect) = (r_vect)
   l_vect = zeros(6,1); 	% 6*1 velocity vector
   r_vect = zeros(3*N, 1);
   for i = 1:N
      tmp1 = eye(3) - v(:,i) * v(:,i)';	% 3*3 block
      mat1((3*i-2):(3*i), (3*i-2):(3*i)) = tmp1;
      tmp2 = R * y(:,i);
      % Skew symmetric matrix corresponding to tmp2
      mat2((3*i-2):(3*i), 1:3) = [0 -tmp2(3) tmp2(2); ...
            tmp2(3) 0 -tmp2(1); -tmp2(2) tmp2(1) 0];
      mat2((3*i-2):(3*i), 4:6) = -eye(3);
      del = tmp1 * (p(:,i) - (tmp2 + b));
      r_vect((3*i-2):(3*i)) = -alpha * del;
   end
   mat3 = mat1 * mat2;
   l_vect = inv(mat3' * mat3) * (mat3' * r_vect);
   ang_vel = l_vect(1:3);	% Angular vecocity
   bdot = l_vect(4:6);
   
   % Error
   for i = 1:N
      tmp_e(:,i) = (eye(3) - v(:,i) * v(:,i)') * ...
         (p(:,i) - (R * y(:,i) + b));
      error1(cnt) = error1(cnt) + norm(tmp_e(:,i));
   end
   error1(cnt) = error1(cnt) / N;
   error2(cnt, :) = [sqrt(6 - 2 * trace(R0' * R)) norm(b0 - b)]; % NOTE
   
   % Update
   R = rot_up2_2(R, ang_vel);
   b = b + delt * bdot;
   cnt = cnt + 1;
end
flops_is = flops
norm_del = error1(limit)
dist_se3 = error2(limit, :)
R_b = [R b]




zzz



% Plot
figure(1)	% error vs # of iterations
plot(iter, error, 'k-')
xlabel('Number of iterations')
ylabel('Error')
%logerror = log(error);
%plot(iter, logerror, 'k-')
%plot(iter, logerror, 'k:')
%xlabel('Number of iterations')
%ylabel('log(Error)')
%tit = sprintf('Algorithm 2 for 6*1 approach, slice %d', slice);
%title(tit)
%legend('solid: synthetic data', 'dotted: real data')
grid on