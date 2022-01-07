function V = vxDevectorize42( M, k1, k2, k3 )
%vxDevectorize42 - convert a 2-D 'flat' matrix back to 4-D grid.
%
%   V = vxDevectorize42( M, k1, k2, k3 ) creates a 4-D matrix of values 
%   where each new 3-D 'cube' is a k1xk2xk3 submatrix, reconstructed
%   by the inverse process of vxVectorize42.
%
%   The function uses vxVectorizeMap42 internally to create the
%   corresponding translation matrix, based on the values of k1
%   (rows), k2 (columns) and k3 (depth) of the resulting 3-D matrix.
%
%   See also: vxVectorize42
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 08-May-2013

if (length(size(M))~=2)
    error('Error: vxDevectorize42: Input dimension must be 2.');
elseif (size(M,2)~=k1*k2*k3)
    error('Error: vxDevectorize42: Data and mapping dimensions must agree.');
else
    V=zeros(k1,k2,k3,size(M,1));           % pre-allocate 'deflattened' (reconstructed) matrix
    MI=vxVectorizeMap42(k1,k2,k3);         % create 'deflattening' translation matrix
    
    for k=1:size(M,1),
        for c=1:size(M,2),
            I=MI(c,:);
            V(I(1),I(2),I(3),k)=M(k,c);      % element-by-element copy into new matrix
        end;
    end;
end;
    
end
