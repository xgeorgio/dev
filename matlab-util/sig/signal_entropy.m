%---------------------------------------------------------------------------
%
%    PROJECT:        (General Purpose Routines)
%    PACKAGE:        General Signal Processing
%    FILE:           'signal_entropy.m'
%
%    PURPOSE:        Calculate the standard entropy value of a 1D signal
%    VERSION:        1.0
%
%    UPDATED:        16-Jul-2007/09:50
%
%    HISTORY:        version 1.0: implemented core functionality (16-Jul-2007/09:50)
%
%    DESCRIPTION:    Employ the standard histogram-based calculation of the entropy value,
%                    using adjustable number of bins and log10.
%
%    INPUT:          X = [1xN] base signal values (data series)
%                    nbins = number of histogram bins to be used in the calculation
%
%    OUTPUT:         eval = entropy value of the input signal
%
%    COPYRIGHT:      Harris Georgiou (c) 2007
%
%---------------------------------------------------------------------------


function eval = signal_entropy( X, nbins )

logx_limit = 1e-6;                 % minimum input (hist.value) for log to avoid 'NaN' results

H=hist(X,nbins);
idx=find(H<=logx_limit);
H(idx)=logx_limit;
H=H/sum(H);

eval=0;
for k=1:length(H),
    eval = eval + (-H(k))*log10(H(k));
end;

