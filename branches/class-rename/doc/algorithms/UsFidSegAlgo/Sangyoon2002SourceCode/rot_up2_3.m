% Updating a rotation matrix
% Input: current rotation matrix, angular velocity vector
% Output: updated rotation matrix
%
% created: Dec 1, 2000
% modified: Dec 11, 2000
% Sangyoon Lee

function rot_new = rot_up2_3(rot_old, ang_vel)

% Skew symmetric matrix corresponding to angular velocity
skew = [0 -ang_vel(3) ang_vel(2); ang_vel(3) 0 -ang_vel(1);
      -ang_vel(2) ang_vel(1) 0];
rot_new = rot_old * (eye(3) + skew);	% Update
rot_new = rot_new * (inv(sqrtm(rot_new' * rot_new)));	% Renormalization
