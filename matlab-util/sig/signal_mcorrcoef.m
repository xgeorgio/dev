function R = signal_mcorrcoef( X, Y )
%signal_mcorrcoef - calculate multiple correlation coefficient(s).
%
%   R = signal_mcorrcoeff( X, Y ) returns the multiple correlation
%   coefficient (MCC) between NxL matrix X and column matrix Y 
%   Multiple Correlation Coefficient (MCC):  R = sqrt( c'*(r^-1)*c )
%   where c=corr(X,Y) and r=corr(X,X). If Y contains L>1 columns, then 
%   R is a 1xL row vector with MCC values, one for each column in Y.
%
%   Typically, Y is the 'template' against which X is tested in terms
%   of MCC value, i.e., a cummulative statistic for the entire X matrix,
%   instead of simple pairwise correlations between columns of X and Y.
%
%   See also: corr, corrcoeff
%

% check arguments, apply default actions
if (size(Y,1)~=size(X,1)),
    error('Error: signal_mcorrcoeff: incompatible matrix dimensions');
else
    R=zeros(1,size(Y,2));       % row vector of MCC values for every column in Y
    r=corr(X);                  % calculate correlation matrix for columns in X
    r=r^-1;                     % calculate inverse of correlations of X
    c=corr(X,Y);                % calculate cross-correlations with Y columns
    for k=1:length(R),
        R(k)=sqrt(c(:,k)'*r*c(:,k));      % calculate final MCC value for Y column k 
    end;
end;

end

