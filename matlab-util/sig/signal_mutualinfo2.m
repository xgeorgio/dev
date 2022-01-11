function V = signal_mutualinfo2( M, Nbins )
%signal_mutualinfo2 - Calculate the mutual information between data series.
%
%   V = signal_mutualinfo2( M ) returns the mutual information between every
%   pair of data series ordered in columns, using the default number of 
%   histogram bins for the pdf estimation.
%
%   M is a (NxC) matrix where C are the data series of size N each and 
%   the resulting matrix V is symmetric of size (CxC).
%   
%   V = signal_mutualinfo2( M, Nbins ) supplies the number of bins to use.
%
%   The default Nbins is 10 or the (larger) number that gives roughly
%   100 samples per bin.
%
%   Example:
%       V = signal_mutualinfo2( 0.2*randn(100,4)+ones(100,4) );
%
%   See also: signal_mutualinfo2
%

% check arguments, apply default actions
if (nargin==1)
    Nbins=0;
end;

% calculate the mutual information for every pair of data series
cN=size(M,2);
V=zeros(cN,cN);
for u=1:cN
    for k=u:cN
        V(u,k)=signal_mutualinfo(M(:,u),M(:,k),Nbins);
        V(k,u)=V(u,k);
    end
end
    
end

