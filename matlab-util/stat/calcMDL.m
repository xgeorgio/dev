function mdl = calcMDL( Y, D, X, Ksp )
%calcMDL - calculate per-column sparsity of a matrix.
%
%   mdl = calcMDL( Y, D, X, Ksp ) returns the estimated value for
%   Minimum Description Length (MDL):  MDL=L(y|n0)+L(n0)
%
%   Y is the source TxN data matrix, D is the TxM dictionary, X is 
%   the corresponding MxN coefficients matrix and Ksp is the defined
%   sparsity level (number of atoms used from D). If Ksp<=0 then it 
%   is calculated as the mean sparsity level from X (i.e. column-wise)
%   or limited to max (number of D columns) if set to larger values.
%
%   The estimation of MDL is generic and can be applied to any Y=D*X
%   decomposition framework (not limited to dictionary learning), since
%   the goodness-of-fit in L(y|n0) is calculated directly from the 
%   variances (column-wise) between Y and D*X.
%
%   See also: calcSparsity
%

% check arguments, apply default actions
if ((size(Y,1)~=size(D,1))||(size(Y,2)~=size(X,2))||(size(D,2)~=size(X,1))),
    error('Error: calcMDL: incompatible matrix dimensions');
else
    if (Ksp<=0),
        Ksp=mean(calcSparsity(X));    % calculate Ksp (as mean) from coefficients matrix
    elseif (Ksp>size(D,2)),
        Ksp=size(D,2);                % set to max (i.e. #Dcols)
    end;
    % else use Ksp as given, 1<=Ksp<=#Dcols

    S=sum((Y-D*X).^2)/size(Y,1);            % calculate per-voxel variances
    Ly=size(Y,1)/2*sum( log2(2*pi*S) );     % = L(y|n0)
    L0=3/2*Ksp*size(Y,2)*log2(size(D,2));   % = L(n0)
    mdl=Ly+L0;                              % MDL=L(y|n0)+L(n0) 
end;

end
