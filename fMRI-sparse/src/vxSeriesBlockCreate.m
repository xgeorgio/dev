function D = vxSeriesBlockCreate( len0, len1, ntimes, slack, vmin, vmax )
%vxSeriesBlockCreate - Create a block-series vector of a specified pattern.
%
%   V = vxSeriesBlockCreate( len0, len1, ntimes ) creates a 1-D matrix of values
%   using default values for slack, vmin and vmax values:
%       len0    : length (points) of low-value section
%       len1    : length (points) of high-value section
%       ntimes  : how many len0/len1 copies to create
%
%   V = vxSeriesBlockCreate( len0, len1, ntimes, slack ) creates a 1-D matrix with
%   the specified slack (offsets) at the start and at the end of the series.
%
%   V = vxSeriesBlockCreate( len0, len1, ntimes, slack, vmin, vmax ) creates a 1-D matrix 
%   with the specified slack, minimum and maximum values.
%
%   Example:
%       V = vxSeriesBlockCreate(2,3,5,1,-1,1)
%
%   See also: vxSeriesEventCreate
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 19-Apr-2013

% check arguments, apply defaults for missing
if ((nargin<3)||(len1<1)||(len0<1)||(ntimes<1))
    error('Error: vxSeriesBlockCreate: Invalid input arguments.');
elseif (nargin==3)
    slack=0;
    vmin=0;
    vmax=1;
elseif (nargin==4)
    if (slack<0)
        warning('vxSeriesBlockCreate: slack argument reset to default');
        slack=0;
    end;
    vmin=0;
    vmax=1;
elseif (nargin==5)
    vmax=1;
end;

% reset low/high output limits if necessary 
if (vmin>vmax)
    warning('vxSeriesBlockCreate: vmin and vmax swapped');
    t=vmin;  vmin=vmax;  vmax=t;
elseif (vmin==vmax)
    warning('vxSeriesBlockCreate: vmax reset to vmin+1');
    vmax=vmin+1;
end;

% create block pair, slack, copy to final
P(1:len0)=vmin;
P(len0+1:len0+len1)=vmax;
D0(1:slack)=vmin;
D=[];
for k=1:ntimes, D=[D P]; end;
D=[D0 D D0];

end
