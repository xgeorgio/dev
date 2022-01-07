function Y = signal_white( X )
%signal_white - Perform simple mean/std whitening to a data series.
%
%   Y = signal_white( X ) returns a 'whitened' version of X with values
%   of new zero-valued mean and rescaled (divided) by standard deviation.
%
%   The transformation is performed column-wise and new data series has
%   mean=0 and std=1 (per-column).
%
%   See also: signal_rescale
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 25-Sept-2013

% check arguments, apply default actions
if (length(size(X))>2)
    error('signal_white: input dimension must be MxN');
else
    % normalize/window function
    Y=zeros(size(X));
    m=mean(X);  s=std(X);  
    for k=1:size(X,2)
        if (s==0)
            warning('signal_white: std is 0, adjusted to 1');
            s=1;
        end;
        
        Y(:,k)=X(:,k)-m(k);
        Y(:,k)=X(:,k)/s(k);
    end;
end;

end
