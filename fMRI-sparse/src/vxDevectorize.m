function V = vxDevectorize( M, k1, k2, k3 )
%vxDevectorize - convert a 2-D matrix back to 3-D/4-D grid
%
%   V = vxDevectorize( M, k1, k2 ) creates a 3-D matrix of values 
%   where each new 2-D 'slice' is a k1xk2 submatrix, reconstructed
%   by the inverse process of vxVectorize32.
%
%   V = vxDevectorize( M, k1, k2, k3 ) creates a 4-D matrix of values 
%   where each new 3-D 'cube' is a k1xk2xk3 submatrix, reconstructed
%   by the inverse process of vxVectorize42.
%
%   The function uses the 'rs' versions for the core inner row-wise 
%   transformation (vxDevectorize32rs, vxDevectorize42rs).
%
%   See also: vxVectorize, vxVectorizeMap32, vxVectorizeMap42
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 27-May-2013

switch (nargin)
    case 3
        V=vxDevectorize32rs(M,k1,k2);
    case 4
        V=vxDevectorize42rs(M,k1,k2,k3);
    otherwise
        error('Error: vxDevectorize: Invalid input dimensions.');
end;
    
end
