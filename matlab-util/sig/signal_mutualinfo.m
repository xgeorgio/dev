function val = signal_mutualinfo( X, Y, Nbins )
%signal_mutualinfo - Calculate the mutual information of a pair of data series.
%
%   y = signal_mutualinfo( X, Y ) returns the mutual information between the
%   pair of data series X and Y, using the default number of histogram 
%   bins for the pdf estimation.
%
%   y = signal_mutualinfo( X, Y, Nbins ) supplies the number of bins to use.
%
%   The default Nbins is 10 or the (larger) number that gives roughly
%   100 samples per bin.
%
%   Example:
%       y = signal_mutualinfo( 0.2*randn(100,1), ones(100,1) );
%
%   See also: signal_mutualinfo2
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
[Hxy,Bxy]=hist3([X Y],[N N]);       % 2-D mutual pdf between the data X,Y series
neflag=0;
for k=1:N
    neflag = neflag + ~isempty(find(Hxy(k,:)==0));
end
if (neflag)
    Hxy=Hxy+1;
end
Hxy=Hxy/sum(sum(Hxy));

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

% calculate the mutual information
val=0;
for x=1:N
    for y=1:N
        val = val + Hxy(x,y).*log( Hxy(x,y)/(Hx(x)*Hy(y)) );
    end
end

end

