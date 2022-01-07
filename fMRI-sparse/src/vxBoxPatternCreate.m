function D = vxBoxPatternCreate( SI, SJ, slack, vmin, vmax )
%vxBoxPatternCreate - Create a 2-D grid of a specified pattern.
%
%   V = vxBoxPatternCreate( SI, SJ ) creates a 2-D matrix of values
%   using default values for slack, vmin and vmax values.
%       SI      : Nx1 column vector of first pattern (1-D)
%       SJ      : 1xM row vector of second pattern (1-D)
%
%   The result D is the dot product of SI*SJ, i.e. a NxM matrix.
%
%   V = vxBoxPatternCreate( SI, SJ, slack ) creates a 2-D matrix with
%   the specified slack (offsets) at the start and at the end of the series.
%
%   V = vxBoxPatternCreate( SI, SJ, slack, vmin, vmax ) creates a 2-D matrix 
%   with the specified slack, minimum and maximum values.
%
%   Example:
%       V = vxBoxPatternCreate([0 1]',[1 2 1],1,-1,1)
%
%   See also: vxSeriesBlockCreate
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 19-Apr-2013

% check arguments, apply defaults for missing
if (nargin<2)
    error('Error: vxBoxPatternCreate: Invalid input arguments.');
elseif (size(SI,2)>1)
    error('Error: vxBoxPatternCreate: first argument must be column vector.');
elseif (size(SJ,1)>1)
    error('Error: vxBoxPatternCreate: second argument must be row vector.');
elseif (nargin==2)
    slack=0;
    vmin=0;
    vmax=1;
elseif (nargin==3)
    if (slack<0)
        warning('vxBoxPatternCreate: slack argument reset to default');
        slack=0;
    end;
    vmin=0;
    vmax=1;
elseif (nargin==4)
    vmax=1;
end;

% reset low/high output limits if necessary 
if (vmin>vmax)
    warning('vxBoxPatternCreate: vmin and vmax swapped');
    t=vmin;  vmin=vmax;  vmax=t;
elseif (vmin==vmax)
    warning('vxBoxPatternCreate: vmax reset to vmin+1');
    vmax=vmin+1;
end;

% create full (2-D) pattern from two vectors (1-D)
D=SI*SJ; 

% normalize/window function
mn=min(D(:));
mx=max(D(:));
dv=mx-mn;
D=(D-mn)*(vmax-vmin)/(mx-mn)+vmin;

% add slack if requested
if (slack>0)
    DD=ones( size(D,1)+2*slack, size(D,2)+2*slack )*vmin;
    DD(slack+1:slack+size(D,1),slack+1:slack+size(D,2))=D(:,:);
    D=DD;
end;

end
