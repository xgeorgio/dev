function M = vxVectorizeMap32( k1, k2 )
%vxVectorizeMap32 - create a translation map between 3-D and 2-D maps.
%
%   M = vxVectorizeMap32( k1, k2 ) creates a translation map between 
%   an arbitrary 3-D grid and a 2-D matrix as created by vxVectorize32.
%
%   The resulting map can be used to translate the column-wise positions
%   of the concatenated 1st/2nd dims into their original pair.
%
%   See also: vxDevectorize32, vxVectorizeMap42, vxVectorize32
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 08-May-2013

if ((k1<1)||(k2<1))
    error('Error: vxVectorizeMap32: Invalid input dimensions.');
else
    M=zeros(k1*k2,2);
    for s1=1:k1
        for s2=1:k2
            c=(s1-1)*k2+s2;
            M(c,:)=[s1 s2];
        end;
    end;
end;

end
