function [RC, PV] = vxCalcActivMap( G, X, PVlim )
%vxCalcActivMap - calculate correlation/p-value maps
%
%   [RC, PV] = vxCalcActivMap( G, X, 0 ) correlates each column of
%   a 'template' N-by-P1 matrix G with a response N-by-P2 matrix X
%   and returns a P1-by-P2 correlation matrix RC and a P1-by-P2
%   p-value (stat.signif.) matrix PV.
%
%   If PVlim>0 then p-values are thresholded and resulting PV matrix
%   is boolean (0/1), otherwise p-values are returned as-is.
%
%   See also: corr
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 12-May-2013

if (size(G,1)~=size(X,1))
    error('Error: vxCalcActivMap: Input dimensions must agree.');
else
    [RC,PV]=corr(G,X);
    
    if (PVlim>0)
        PVZ=zeros(size(PV));
        for k=1:size(PV,1)
            pidx1=(PV(k,:)<=PVlim);     % logical indexing instead of find()
            PVZ(k,pidx1)=1;
        end;
        PV=PVZ;
    end;
end

end