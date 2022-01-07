function V = vxVectorize32rs( M )
%vxVectorize32rs - convert a 3-D grid into a 2-D 'flat' matrix.
%
%   V = vxVectorize32rs( M ) creates a 2-D matrix of values where 
%   each new row is a concatenation of all dimensions except the 
%   last which is retainned as the row-wise dimension.
%
%   The new row-wise dimension is the last (3rd) original dimension
%   and the contents are concatenated column-wise, then row-wise.
%
%   This 'rs' version uses reshape() for the core inner row-wise 
%   transformation for maximum speed.
%
%   See also: vxDevectorize32rs, vxVectorize42rs, vxVectorize32
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 27-May-2013

if (length(size(M))~=3)
    error('Error: vxVectorize32rs: Input dimension must be 3.');
else
    [sz1,sz2,sz3]=size(M);    
    V=zeros(sz3,sz1*sz2);               % new rows are original 3rd dim
    for k=1:sz3
        V(k,:)=reshape(M(:,:,k)',1,sz1*sz2);    % use 'reshape' for each row
    end;
end;
    
end
