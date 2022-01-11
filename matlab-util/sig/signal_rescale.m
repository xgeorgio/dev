function Y = signal_rescale( X, vmin, vmax )
%signal_rescale - Perform simple rescaling (windowing) to a data series.
%
%   Y = signal_rescale( X ) returns a rescaled version of X with values
%   within the default range 0...1 and same dimensions, i.e either as
%   row- or column-vector (depending on input X).
%
%   Y = signal_rescale( X, vmin, vmax ) applies the rescaling within the
%   specified range [vmin...vmax], provided that vmin<vmax.
%
%   Input X can be a vector (Nx1) or (1xN) or a full matrix (NxC).
%
%   Example:
%       V = signal_rescale( randn(100) );
%
%   See also: signal_white
%

% check arguments, apply default actions
if (nargin==1)
    vmin=0;  vmax=1;
elseif (nargin==2)
    vmax=1;
end;

if (vmin>vmax)
    warning('signal_rescale: swapping vmin and vmax values');
    t=vmin;  vmin=vmax;  vmax=t;
elseif (vmin==vmax)
    warning('signal_rescale: setting vmax to vmin+1');
    vmax=vmin+1;
end;
    
% normalize/window function
mn=min(X(:));
mx=max(X(:));

if (mx==mn)
    warning('signal_rescale: zero scaling divisor, setting to 1');
    mx=mn+1;
end;

Y=(X-mn)*(vmax-vmin)/(mx-mn)+vmin;

end
