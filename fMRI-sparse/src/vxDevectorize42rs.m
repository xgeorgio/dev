function V = vxDevectorize42rs( M, k1, k2, k3 )
%vxDevectorize42rs - convert a 2-D 'flat' matrix back to 4-D grid.
%
%   V = vxDevectorize42rs( M, k1, k2, k3 ) creates a 4-D matrix  
%   of values where each new 3-D 'cube' is a k1xk2xk3 submatrix, 
%   reconstructed by the inverse process of vxVectorize42rs.
%
%   This 'rs' version uses reshape() for the core inner row-wise 
%   transformation for maximum speed, i.e, does not use the
%   vxVectorizeMap32 internally like vxDevectorize32 does.
%
%   See also: vxVectorize42rs, vxVectorize32rs, vxDevectorize42
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 27-May-2013

if (length(size(M))~=2)
    error('Error: vxDevectorize42rs: Input dimension must be 2.');
elseif (size(M,2)~=k1*k2*k3)
    error('Error: vxDevectorize42rs: Data and mapping dimensions must agree.');
else
    V=zeros(k1,k2,k3,size(M,1));           % pre-allocate 'deflattened' (reconstructed) matrix    
    for k=1:size(M,1),
        V(:,:,:,k)=vxDevectorize32rs( reshape(M(k,:),k1*k2,k3)', k1, k2 );
    end;
end;
    
end
