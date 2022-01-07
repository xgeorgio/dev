function S = calcSparsity( X )
%calcSparsity - calculate per-column sparsity of a matrix.
%
%   S = calcSparsity( X ) returns a row vector with the sums of
%   non-zero elements per column in X. Then average sparsity can
%   be calculated as: L=sum(S)/length(S).
%
%   If X is a coefficient matrix that is combined with a matrix D
%   of K components, e.g. GLM regressors or dictionary atoms as D*X, 
%   then the sparsity ratio is: L/K
%
%   See also: -
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 22-Sept-2013

S=zeros(1,size(X,2));
for k=1:length(S)
    S(k)=length(find(X(:,k)));
end;

end
