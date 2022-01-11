function [ dd, hh, mm, ss ] = time2eta( secs )
%time2eta - Convert time in seconds to full time format
%
%   [ dd, hh, mm, ss ] = time2eta( secs ) takes as input the a value in secs
%   and provides the corresponding dd/hh/mm/ss set of values.
%
%   Example:
%       time2eta( 3*24*60+45 );
%
%   See also: strETA
%

dd = floor( secs/3600/24 );
secs = secs - dd*3600*24;

hh = floor( secs/3600 );
secs = secs - hh*3600;

mm = floor( secs/60 );
secs = secs - mm*60;

ss = secs;

end

