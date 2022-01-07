function M = vxVectorizeMap42( k1, k2, k3 )
%vxVectorizeMap42 - create a translation map between 4-D and 2-D maps.
%
%   M = vxVectorizeMap42( k1, k2, k3 ) creates a translation map between 
%   an arbitrary 4-D grid and a 2-D matrix as created by vxVectorize42.
%
%   The resulting map can be used to translate the column-wise positions
%   of the concatenated 1st/2nd/3rd dims into their original pair.
%
%   See also: vxDevectorize42, vxVectorizeMap32, vxVectorize42
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 08-May-2013

if ((k1<1)||(k2<1)||(k3<1))
    error('Error: vxVectorizeMap42: Invalid input dimensions.');
else
    M=zeros(k1*k2*k3,3);
    for s3=1:k3
        for s1=1:k1
            for s2=1:k2
                c=(s3-1)*k1*k2+(s1-1)*k2+s2;
                M(c,:)=[s1 s2 s3];
            end;
        end;
    end;
end;

end
