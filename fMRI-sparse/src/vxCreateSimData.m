function [ BS, BT ] = vxCreateSimData( flag_Sdep, flag_Tdep )
%vxCreateSimData - create standard fMRI-like test data series.
%
%   [ BS, BT ] = vxCreateSimData( flag_Sdep, flag_Tdep ) creates 
%   a sequence of 2-D matrices ('slices') based on specified
%   spatial and/or temporal dependencies.
%
%   The 'slices' are images of two white blocks (squares) in a
%   black background. If spatial dependency is selected (flag_Sdep=1)
%   then these two blocks have one overlapping corner. Each block is
%   'active' (on) or "inactive' (off) according to two time sequences
%   that are created internally. If temporal dependency is selected
%   (flag_Tdep=1) then the two blocks appear at the same time at 
%   roughly 3/10 of each cycle, for a total of 10 repeats. Due to 
%   internal specifics for block sizes and centers, the size of the 
%   'slice' is fixed to 200x200 and timeseries to 100 frames.
%
%   BS and BT are the 'slices' sequence and the two time sequences,
%   respectively, that are created according to specified dependencies.
%
%   See also: vxCreateRealData, vxCreateComplexData
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 28-May-2013

fprintf('creating box patterns and activation series:\n');

%... create basic patterns and series ...
fprintf('\tgenerating box patterns and activation series... ');

k1=200;  k2=200;  k3=200;
S1=zeros(k1,1);  S1(20:70,1)=1;         % basic 1-D template for upper-left box
S2=zeros(k2,1);  S2(130:180,1)=1;       % basic 1-D template for lower-right box
S3=zeros(k3,1);  S3(60:140,1)=1;        % basic 1-D template for middle box

bm1=[ 45  45 (45-1)*200+45 ];           % center of box 1 (upper-left) and #column in 'flat'
bm2=[155 155 (155-1)*200+155 ];         % center of box 2 (lower-right) and #column in 'flat'
bm3=[100 100 (100-1)*200+100 ];         % center of box 3 (middle) and #column in 'flat'

P1=vxBoxPatternCreate(S1,S1');          % create 2-D patterns from 1-D templates
P2=vxBoxPatternCreate(S2,S2');
P3=vxBoxPatternCreate(S3,S3');

%... create pattern series with/without spatial/temporal dependencies ...
if (flag_Tdep)
    strTdep='WITH';
    T1=repmat([1 1 1 1 1 1 0 0 0 0],1,10)';         % showing box-1 (initial/default)
    T2=repmat([0 0 0 1 1 1 1 1 1 1],1,10)';         % showing box-2 (with time-overlapping)
else
    strTdep='NO';
    T1=repmat([1 1 1 1 1 0 0 0 0 0],1,10)';         % showing box-1 (initial/default)
    T2=repmat([0 0 0 0 0 1 1 1 1 1],1,10)';         % showing box-2 (no time-overlapping)
end;

BP=zeros(size(P1,1),size(P1,2),2);     % combine two box patterns (2-D)
BP(:,:,1)=P1;                          % slice: upper-left box pattern (initial/default)
if (flag_Sdep)
    strSdep='WITH';
    BP(:,:,2)=P3;                      % slice: middle box pattern (overlapping)
else
    strSdep='NO';
    BP(:,:,2)=P2;                      % slice: lower-right box pattern (no overlapping)
end;

BT=zeros(size(T1,1),2);                % create activation series (1-D)
BT(:,1)=T1;
BT(:,2)=T2;

fprintf('done\n\tgenerating full series: %s spatial / %s temporal dependencies... ',strSdep,strTdep);

BS=vxBoxSeriesCreate(BP,BT);                 % generate pattern sequence

fprintf('done\n');

end

