function V = signal_crossentropy2( M, Nbins )
%signal_crossentropy2 - Calculate the cross-entropy between data series.
%
%   V = signal_crossentropy2( M ) returns the cross-entropy between every
%   pair of data series ordered in columns, using the default number of 
%   histogram bins for the pdf estimation.
%
%   M is a (NxC) matrix where C are the data series of size N each and 
%   the resulting matrix V is symmetric of size (CxC).
%   
%   V = signal_crossentropy2( M, Nbins ) supplies the number of bins to use.
%
%   The default Nbins is 10 or the (larger) number that gives roughly
%   100 samples per bin.
%
%   Example:
%       V = signal_crossentropy2( 0.2*randn(100,4)+ones(100,4) );
%
%   See also: signal_crossentropy
%

% check arguments, apply default actions
if (nargin==1)
    Nbins=0;
end;

% calculate the cross-entropy for every pair of data series
cN=size(M,2);
V=zeros(cN,cN);
for u=1:cN
    for k=u:cN
        V(u,k)=signal_crossentropy(M(:,u),M(:,k),Nbins);
        V(k,u)=V(u,k);
    end
end
    
end

