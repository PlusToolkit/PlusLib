function transducerToProbe = getTransducerToProbeTransform(transducerToProbeUltrasonix)
% Get TransducerToProbeTransform matrix from Ultrasonix probe calibration matrix

% Ultrasonix uses [1yx] coordinates (first column is origin, second column is Y axis, third column is X axis)

% Transducer's origin in the Probe sensor coordinate system
transducerOrigin_Probe = [transducerToProbeUltrasonix(1,1); transducerToProbeUltrasonix(2,1); transducerToProbeUltrasonix(3,1)];

% Transducer's X, Y, Z axes in the Probe sensor coordinate system:
% (Plus's transducer X coordinate system axis points to M direction, while
% in Ultrasonix it is the opposite, U direction, therefore the minus sign
% at the transducerX_Probe computation)
transducerX_Probe = -[transducerToProbeUltrasonix(1,3); transducerToProbeUltrasonix(2,3); transducerToProbeUltrasonix(3,3)];
transducerY_Probe = [transducerToProbeUltrasonix(1,2); transducerToProbeUltrasonix(2,2); transducerToProbeUltrasonix(3,2)];

% We do not want the TransducerToProbe matrix to be a projection matrix
% because that cannot be inverted and cannot be used for rendering 3D
% objects (a projection matrix would flatten everything to the image plane).
% Therefore, we compute the Z axis as the cross product of the X and Y vector.
transducerZ_Probe = cross(transducerX_Probe, transducerY_Probe);
transducerZ_Probe = transducerZ_Probe/norm(transducerZ_Probe);

transducerToProbe = [[transducerX_Probe, transducerY_Probe, transducerZ_Probe, transducerOrigin_Probe]; 0 0 0 1];
