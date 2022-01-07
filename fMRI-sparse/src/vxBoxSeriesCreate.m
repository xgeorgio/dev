function Q = vxBoxSeriesCreate( BS, BA, slack, vmin, vmax )
%vxBoxSeriesCreate - Create a sequence of 2-D specified patterns.
%
%   V = vxBoxSeriesCreate( BS, BA ) creates a sequence of 2-D matrices
%   using default values for slack, vmin and vmax values.
%
%   BS is a set of 2-D matrices (NxMxK) with different patterns. BA is a
%   set of columns (TxK) defining the activation series and weights of
%   each of the defined pattern. Based on these definitions, a sequence
%   of T frames is constructed as the weighted sum of every pattern.
%   When the activation patterns are only 0 or 1, the final sequence is
%   an alternation between the defined input patterns (2-D).
%
%   V = vxBoxSeriesCreate( BS, BA, slack ) creates a sequence of 2-D 
%   matrices the specified slack (offsets) at the start and at the end.
%
%   V = vxBoxPatternCreate( SI, SJ, slack, vmin, vmax ) creates a sequence 
%   of 2-D matrix with the specified slack, minimum and maximum values.
%
%   Example:
%       V = vxBoxSeriesCreate([0 1; 0 1],[1 2; 1 3],[1 0; 1 1])
%
%   See also: vxBoxSeriesCreate
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 19-Apr-2013

% check arguments, apply defaults for missing
if (nargin<2)
    error('Error: vxBoxSeriesCreate: Invalid input arguments.');
elseif (size(BS,3)~=size(BA,2))
    error('Error: vxBoxSeriesCreate: Number of patterns and activations do not match.');
elseif (nargin==2)
    slack=0;
    vmin=0;
    vmax=1;
elseif (nargin==3)
    if (slack<0)
        warning('vxBoxSeriesCreate: slack argument reset to default');
        slack=0;
    end;
    vmin=0;
    vmax=1;
elseif (nargin==4)
    vmax=1;
end;

% reset low/high output limits if necessary 
if (vmin>vmax)
    warning('vxBoxSeriesCreate: vmin and vmax swapped');
    t=vmin;  vmin=vmax;  vmax=t;
elseif (vmin==vmax)
    warning('vxBoxSeriesCreate: vmax reset to vmin+1');
    vmax=vmin+1;
end;

% prepare series and create as weighted sum of all given patterns (2-D)
sz1=size(BS,1);         % num of rows in pattern (2-D)
sz2=size(BS,2);         % num of columns in pattern (2-D)
npatt=size(BA,2);       % num of patterns (2-D)
nframes=size(BA,1);     % length of sequence (with no slack)

Q=zeros(sz1,sz2,nframes);

for k=1:nframes
    Q(:,:,k)=BS(:,:,1)*BA(k,1);
    for c=2:npatt
        Q(:,:,k)=Q(:,:,k)+BS(:,:,c)*BA(k,c);    % weighted sum of all patterns (2-D)
    end;
end;

% normalize/window function
mn=min(min(Q(:)));
mx=max(max(Q(:)));
dv=mx-mn;
Q=(Q-mn)*(vmax-vmin)/(mx-mn)+vmin;

% add slack if requested
if (slack>0)
    SL=ones(sz1,sz2,nframes+2*slack)*vmin;
    SL(:,:,slack+1:slack+nframes)=Q;
    Q=SL;
end;

