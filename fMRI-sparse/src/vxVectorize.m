function V = vxVectorize( M )
%vxVectorize - convert a 3-D/4-D grid into a 2-D 'flat' matrix.
%
%   V = vxVectorize( M ) creates a 2-D matrix of values where each
%   new row is a concatenation of all dimensions except the last
%   which is retainned as the row-wise dimension.
%
%   The new row-wise dimension is the last (3rd or 4th) original 
%   dimension and the contents are concatenated column-wise, 
%   then row-wise.
%
%   The function uses the 'rs' versions for the core inner row-wise 
%   transformation (vxVectorize32rs, vxVectorize42rs).
%
%   See also: vxDevectorize, vxVectorize32rs, vxVectorize42rs
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 27-May-2013

switch (ndims(M))
    case 3
        V=vxVectorize32rs(M);
    case 4
        V=vxVectorize42rs(M);
    otherwise
        error('Error: vxVectorize: Input dimension must be 3 or 4.');
end;
    
end
