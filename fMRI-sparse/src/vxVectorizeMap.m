function M = vxVectorizeMap( k1, k2, k3 )
%vxVectorizeMap - create a translation map between 3-D/4-D and 2-D maps.
%
%   M = vxVectorizeMap( k1, k2 ) creates a translation map between 
%   an arbitrary 3-D grid and a 2-D matrix as created by vxVectorize32.
%
%   M = vxVectorizeMap( k1, k2, k3 ) creates a translation map between 
%   an arbitrary 4-D grid and a 2-D matrix as created by vxVectorize42.
%
%   The resulting map can be used to translate the column-wise positions
%   of the concatenated 1st/2nd or 1st/2nd/3rd dims into their original 
%   pair, compatible with vxVectorize. 
%
%   See also: vxVectorize, vxDevectorize
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 08-May-2013

switch (nargin)
    case 2
        M=vxVectorizeMap32(k1,k2);
    case 3
        M=vxVectorizeMap42(k1,k2,k3);
    otherwise
        error('Error: vxVectorizeMap: Invalid input dimensions.');
end;
    
end
