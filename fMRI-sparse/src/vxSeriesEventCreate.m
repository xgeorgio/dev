function D = vxSeriesEventCreate( interv, ntimes, slack, vmin, vmax )
%vxSeriesEventCreate - Create a event-series vector of a specified pattern.
%
%   V = vxSeriesEventCreate( interv, ntimes ) creates a 1-D matrix of values
%   using default values for slack, vmin and vmax values.
%       interv  : length (points) of interval between peaks
%       ntimes  : how many interv/peak copies to create
%
%   V = vxSeriesEventCreate( interv, ntimes, slack ) creates a 1-D matrix with
%   the specified slack (offsets) at the start and at the end of the series.
%
%   V = vxSeriesEventCreate( interv, ntimes, slack, vmin, vmax ) creates a 1-D matrix 
%   with the specified slack, minimum and maximum values.
%
%   Example:
%       V = vxSeriesEventCreate(3,5,1,-1,1)
%
%   See also: vxSeriesBlockCreate
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 19-Apr-2013

% check arguments, apply defaults for missing
if ((nargin<2)||(interv<1)||(ntimes<1))
    error('Error: vxSeriesEventCreate: Invalid input arguments.');
elseif (nargin==2)
    slack=0;
    vmin=0;
    vmax=1;
elseif (nargin==3)
    if (slack<0)
        warning('vxSeriesEventCreate: slack argument reset to default');
        slack=0;
    end;
    vmin=0;
    vmax=1;
elseif (nargin==4)
    vmax=1;
end;

% reset low/high output limits if necessary 
if (vmin>vmax)
    warning('vxSeriesEventCreate: vmin and vmax swapped');
    t=vmin;  vmin=vmax;  vmax=t;
elseif (vmin==vmax)
    warning('vxSeriesEventCreate: vmax reset to vmin+1');
    vmax=vmin+1;
end;

% create as special block-type series
D=vxSeriesBlockCreate(interv,1,ntimes,slack,vmin,vmax);

end
