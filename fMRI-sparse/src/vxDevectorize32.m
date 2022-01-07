function V = vxDevectorize32( M, k1, k2 )
%vxDevectorize32 - convert a 2-D 'flat' matrix back to 3-D grid.
%
%   V = vxDevectorize32( M, k1, k2 ) creates a 3-D matrix of values 
%   where each new 2-D 'slice' is a k1xk2 submatrix, reconstructed
%   by the inverse process of vxVectorize32.
%
%   The function uses vxVectorizeMap32 internally to create the
%   corresponding translation matrix, based on the values of k1
%   (rows) and k2 (columns) of the resulting 3-D matrix.
%
%   See also: vxVectorize32
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 08-May-2013

if (length(size(M))~=2)
    error('Error: vxDevectorize32: Input dimension must be 2.');
elseif (size(M,2)~=k1*k2)
    error('Error: vxDevectorize32: Data and mapping dimensions must agree.');
else
    V=zeros(k1,k2,size(M,1));           % pre-allocate 'deflattened' (reconstructed) matrix
    MI=vxVectorizeMap32(k1,k2);         % create 'deflattening' translation matrix
    
    for k=1:size(M,1),
        for c=1:size(M,2),
            I=MI(c,:);
            V(I(1),I(2),k)=M(k,c);      % element-by-element copy into new matrix
        end;
    end;
end;
    
end
