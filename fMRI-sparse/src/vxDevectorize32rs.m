function V = vxDevectorize32rs( M, k1, k2 )
%vxDevectorize32rs - convert a 2-D 'flat' matrix back to 3-D grid.
%
%   V = vxDevectorize32rs( M, k1, k2 ) creates a 3-D matrix of 
%   values where each new 2-D 'slice' is a k1xk2 submatrix, 
%   reconstructed by the inverse process of vxVectorize32rs.
%
%   This 'rs' version uses reshape() for the core inner row-wise 
%   transformation for maximum speed, i.e, does not use the
%   vxVectorizeMap32 internally like vxDevectorize32 does.
%
%   See also: vxVectorize32rs, vxVectorize42rs, vxDevectorize32
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 27-May-2013

if (length(size(M))~=2)
    error('Error: vxDevectorize32rs: Input dimension must be 2.');
elseif (size(M,2)~=k1*k2)
    error('Error: vxDevectorize32rs: Data and mapping dimensions must agree.');
else
    V=zeros(k1,k2,size(M,1));           % pre-allocate 'deflattened' (reconstructed) matrix
    for k=1:size(M,1),
        V(:,:,k)=reshape(M(k,:),k2,k1)';    % apply 'reshape' for each concatenated row
    end;
end;
    
end
