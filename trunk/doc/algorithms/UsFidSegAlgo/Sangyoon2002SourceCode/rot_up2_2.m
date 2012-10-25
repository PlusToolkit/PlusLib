% Updating a rotation matrix
% Input: current rotation matrix, angular velocity vector
% Output: updated rotation matrix
%
% created: Dec 1, 2000
% modified: Dec 11, 2000
% Sangyoon Lee


function rot_new = rot_up2_2(rot_old, ang_vel)

delt = 0.1;	% Artificial time increment
% Skew symmetric matrix corresponding to angular velocity
skew = [0 -ang_vel(3) ang_vel(2); ang_vel(3) 0 -ang_vel(1);
   -ang_vel(2) ang_vel(1) 0];
rot_new = (eye(3) + delt * skew) * rot_old;	% Update
rot_new = rot_new * (inv(sqrtm(rot_new' * rot_new)));	% Renormalization
