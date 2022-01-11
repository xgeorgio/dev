function val = signal_crossentropy( X, Y, Nbins )
%signal_crossentropy - Calculate the cross-entropy of a pair of data series.
%
%   y = signal_crossentropy( X, Y ) returns the cross-entropy between the
%   pair of data series X and Y, using the default number of histogram 
%   bins for the pdf estimation.
%
%   y = signal_crossentropy( X, Y, Nbins ) supplies the number of bins to use.
%
%   The default Nbins is 10 or the (larger) number that gives roughly
%   100 samples per bin.
%
%   Example:
%       y = signal_crossentropy( 0.2*randn(100,1), ones(100,1) );
%
%   See also: signal_crossentropy2
%

% check arguments, apply default actions
if (nargin==2)
    Nbins=0;
end;

if (Nbins<=0)
    N=round(max([10 min([length(X)/100 length(Y)/100])]));
else
    N=round(Nbins);
end

% create empirical pdfs, check for zeros
[Hx,Bx]=hist(X,N);
if (~isempty(find(Hx==0)))
    Hx=Hx+1;
end
Hx=Hx/sum(Hx);

[Hy,By]=hist(Y,N);
if (~isempty(find(Hy==0)))
    Hy=Hy+1;
end
Hy=Hy/sum(Hy);

% calculate the cross-entropy
val = -sum( Hx.*log(Hy) + (1-Hx).*log(1-Hy) );

end

