function V = vxVectorize42rs( M )
%vxVectorize42rs - convert a 4-D grid into a 2-D 'flat' matrix.
%
%   V = vxVectorize42rs( M ) creates a 2-D matrix of values where 
%   each new row is a concatenation of all dimensions except the 
%   last which is retainned as the row-wise dimension.
%
%   The new row-wise dimension is the last (4th) original dimension
%   and the contents are concatenated column-wise, then row-wise
%   into multiple vectors and finally into one complete row.
%
%   This 'rs' version uses reshape() for the core inner row-wise 
%   transformation for maximum speed.
%
%   See also: vxDevectorize42rs, vxVectorize42
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 27-May-2013

if (length(size(M))~=4)
    error('Error: vxVectorize42rs: Input dimension must be 4.');
else
    [sz1,sz2,sz3,sz4]=size(M);
    V=zeros(sz4,sz1*sz2*sz3);               % new rows are original 4th dim
        
    for k=1:sz4
        Q=vxVectorize32rs(M(:,:,:,k));          % convert to rows-per-3rd-dim
        Q=reshape(Q',1,size(Q,1)*size(Q,2));    % translate to flat vector
        V(k,:)=Q;                               % add as row into final matrix
    end;
end;

end
