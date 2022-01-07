function H = vxBOLDtwogammaHRF( len, Tpar, reptime, hasOvershoot )
%vxBOLDtwogammaHRF - Create a sequence of Gamma-based HRF for BOLD (fMRI).
%
%   H = vxBOLDtwogammaHRF( len ) creates standard HRF response pattern
%   using default values for Tpar, resptime and hasOvershoot values.
%
%   H = vxBOLDtwogammaHRF( len, Tpar ) creates customized HRF response 
%   pattern using default values for restime and hasOvershoot values 
%   and the specified Gamma parameters in Tpar:
%       Tpar(1) = delay of response
%       Tpar(2) = delay of undershoot
%       Tpar(3) = dispersion of response
%       Tpar(4) = dispersion of undershoot
%       Tpar(5) = ratio of response to undershoot
%       Tpar(6) = length of kernel (in secs)
%
%   H = vxBOLDtwogammaHRF( len, Tpar, restime ) creates customized HRF 
%   response pattern using the given set of Tpar, default value for 
%   hasOvershoot andthe specified restime, i.e., repetition time (in secs).
%
%   H = vxBOLDtwogammaHRF( len, Tpar, restime, hasOvershoot ) creates a
%   customized HRF response pattern using all the specified parameters and
%   overshoot-enabled interpolation in the final step.
%
%   Example:
%       V = vxBOLDtwogammaHRF(100,[6 16 1 1 6 32],0.1,1)
%
%   See also: vxSeriesBlockCreate
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 23-Apr-2013

% check arguments, apply defaults for missing
if (nargin<2),  Tpar=[6 16 1 1 6 32];  end;     % defaults for Gamma params (sec)
if (nargin<3),  reptime=0.1;  end;              % defaults for Gamma params (sec)
if (nargin<4),  hasOvershoot=0;  end;           % defaults for interpolation mode

if (nargin<1)
    error('Error: vxBOLDtwogammaHRF: Invalid HRF length requested.');
elseif (len<0)
    error('Error: vxBOLDtwogammaHRF: Requested length must be positive.');
elseif ((length(Tpar)~=6)||(min(Tpar)<=0))
    error('Error: vxBOLDtwogammaHRF: Size of Tpar must be 6, all positive.');
elseif (reptime<=0)
    error('Error: vxBOLDtwogammaHRF: reptime parameter must be positive.');
elseif ((hasOvershoot~=0)&&(hasOvershoot~=1))
    % strict checking, can be removed
    warning('vxBOLDtwogammaHRF: hasOvershoot neither 0 or 1 (boolean).');
end;

% calculate base variables and series
delta=reptime/16;

w=zeros(1,Tpar(6)+1);       % w(t) is the base series for Gamma input
for k=1:Tpar(6)
    w(k+1)=delta/k;
end;

if (len<length(w))          % check requested length
    len=length(w);
    warning('vxBOLDtwogammaHRF: Output length readjusted to fit HRF specs.');
end;

% calculate core HRF series from specified params
Y=zeros(1,length(w));
for k=1:length(w)
    % for HRF model details and default parameter values see: 
    % Jagath C. Rajapakse, Juan Zhou: 'Learning effective brain 
    % connectivity with dynamic Bayesian networks', 
    % NeuroImage 37 (2007) 749-760.
    
    g1 = 1 / gamma(Tpar(1)/Tpar(3)) * ((delta/Tpar(3))^(Tpar(1)/Tpar(3))) * (w(k)^(Tpar(1)/Tpar(3)-1)) * exp(-delta/Tpar(3)*w(k));
    g2= 1 / (Tpar(5)*gamma(Tpar(2)/Tpar(4))) * ((delta/Tpar(4))^(Tpar(2)/Tpar(4))) * (w(k)^(Tpar(2)/Tpar(4)-1))*exp(-delta/Tpar(4)*w(k));
    Y(k)=g1-g2;     % final value is the difference of the two parametric gamma distributions
end;

% interpolate if necessary to get the requested output length
if (len>length(w))
    if (hasOvershoot)
        % use piecewise cubic (spline) for simulated overshoot
        H=interp1(1:length(Y),Y,linspace(1,length(Y),len),'spline');
    else
        % use cubic polynomial interpolation (no overshoot)
        H=interp1(1:length(Y),Y,linspace(1,length(Y),len),'cubic');
    end;
else
    H=Y;    % copy resulting HRF as-is (minimum resolution)
end;

end