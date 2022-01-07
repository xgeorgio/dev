function V = vxVectorize32( M )
%vxVectorize32 - convert a 3-D grid into a 2-D 'flat' matrix.
%
%   V = vxVectorize32( M ) creates a 2-D matrix of values where each
%   new row is a concatenation of all dimensions except the last
%   which is retainned as the row-wise dimension.
%
%   The new row-wise dimension is the last (3rd) original dimension
%   and the contents are concatenated column-wise, then row-wise.
%
%   See also: vxDevectorize32, vxVectorizeMap32, vxVectorize42
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 08-May-2013

if (length(size(M))~=3)
    error('Error: vxVectorize32: Input dimension must be 3.');
else
    [sz1,sz2,sz3]=size(M);
    
    V=zeros(sz3,sz1*sz2);               % new rows are original 3rd dim
    for k=1:sz3
        for r=1:sz1
            p0=(r-1)*sz2;               % each 2-D matrix concatenated
            V(k,p0+1:p0+sz2)=M(r,:,k);  % into a 1-D vector            
        end;
    end;
end;
    
end
