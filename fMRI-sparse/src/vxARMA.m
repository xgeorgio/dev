function C = vxARMA( D, K, noise )
%vxARMA - Perform 3-D/4-D ARMA (plus noise) filtering.
%
%   C = vxARMA( D, K ) performs full kernel-wise ARMA weighted sum without
%   added noise components, using matrix D as data and matrix K as the 
%   masking kernel. The kernel K must be of the same or lower dimensionality 
%   than D, i.e., NDIMS(K)<=NDIMS(D), in order to perform the ARMA weighted 
%   sum. The function works for NDIMS(D) equal to 3 or 4.
%
%   C = vxARMA( D, K, noise ) performs full kernel-wise ARMA weighted sum 
%   with added noise components (already generated/stored in the argument), 
%   using matrix D as data and matrix K as the masking kernel. The noise 
%   matrix must be of the same dimensions as of the matrix D, i.e., 
%   NDIMS(noise)==NDIMS(D) and SIZE(noise)==SIZE(D).
%
%   In all cases, the last dimension is taken as the 'time' axis and the
%   auto-regressive (AR) process is performed against this dimension, while 
%   all the other dimensions are assumed to be moving-average (MA) process.
%   In practice, the ARMA process that is performed is of the order of
%   AR( Kt ) and MA( Kr.Kc.Kz ), where Kj is the dimension of kernel K
%   along dimension-j respectively. 
%
%   For overall dimensionality of 3, i.e., when Kz=0, the input data matrix 
%   D is treated as a series of 2-D 'timeframes' (sequence of slices) and 
%   MA is performed as a typical masking filter K (2-D), in the current 
%   (Kt=1) or more slices (Kt>1). Likewise, For overall dimensionality of 4, 
%   i.e., when Kz>0, the input data matrix D is treated as a series of 3-D 
%   'timeframes' (sequence of cubes) and MA is performed as a typical 
%   masking filter K (3-D), in the current (Kt=1) or more slices (Kt>1).
%
%   In the 'timeseries' axis along Kt, the current instance is always the
%   largest index (t=Kt). This means that if AR (Kt>1) is to be performed,
%   the 2-D/3-D masks (slices or cubes) have to be already in reverse order
%   (t=Kt, t-1=Kt-1, ..., t-j=Kt-j, ..., t-Kt+1=1).
%
%   In 'time' dimension (Kt), the kernel AR application starts aligned at 
%   index=Kt, i.e., with no zero-padding. Hence, Kt-1 positions along this
%   dimension are retainned as-is from the original input matrix D, starting 
%   from index=1 to Kt-1. This formulation is compatible with the behavior 
%   of pattern- and -series-creation functions, which include the inclusion 
%   of 'slack' timeframes at the start and the end along the 'time' axis.
%   For every other dimension-j (j=[r,c,z]) the MA application is applied
%   centered/aligned at the edges, i.e., without zero-padding but with no
%   bias towards either size of the input matrix D ('slice' or 'cube').
%
%   Note: For similar processing in 1-D and 2-D signals, use filter() and
%   filter2() build-in functions with appropriate coefficient matrices.
%
%   Example:
%       A=ones(5,5,5,3);
%       K=A(1:3,1:3,1:3,1:2)/(3*3*3*2);
%       ns=randn(size(A));
%       C = vxARMA(A,K,ns);
%
%   See also: vxBoxSeriesCreate
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 09-May-2013

% preload input dimensions for cleaner/faster code
szD=size(D);  dimD=ndims(D);

if (nargin==3)
    hasNoise=1;
    issamedim=(ndims(noise)==dimD);
    k=1;
    while ((issamedim)&&(k<=dimD))
        issamedim=(issamedim)&&(size(noise,k)==szD(k));
        k=k+1;
    end;
    if (~issamedim)     % final check, abort if not everything ok
        error('Error: vxARMA: incompatible data and noise dimensions.');
    end;
else
    hasNoise=0;
end;
    
if ((dimD<3)||(dimD>4))
    error('Error: vxARMA: data dimensions must be 3 or 4.');
elseif (ndims(K)>dimD)
    error('Error: vxARMA: kernel dim must be <= data dim.');
end;

C=D;            % pre-allocate space and pre-copy all data (including margins)

if (dimD==3)  
    Nt=szD(3);     Nr=szD(1);     Nc=szD(2);
    Kt=size(K,3);  Kr=size(K,1);  Kc=size(K,2);
    
    limKt=ceil((Kt+1)/2);
    limKr=ceil((Kr+1)/2);
    limKc=ceil((Kc+1)/2);
    
    %fprintf('Nt=%d/Nr=%d/Nc=%d : Kt=%d/Kr=%d/Kc=%d : limKt=%d/limKr=%d/limKc=%d',Nt,Nr,Nc,Kt,Kr,Kc,limKt,limKr,limKc);
    
    for t=Kt:Nt         % for every 'time frame' (k)
        for r=limKr:Nr-limKr+1         % for each row (r) in current frame 
            for c=limKc:Nc-limKc+1         % for each column (c) in current row
                % perform full kernel-wise AR (Kt) and MA (Kr.Kc) weighted sum
                C(r,c,t)=sum(sum(sum(D(r-limKr+1:r-limKr+Kr,c-limKc+1:c-limKc+Kc,t-Kt+1:t).*K(1:Kr,1:Kc,1:Kt))));
                %fprintf('(%d,%d,%d) : %.3f -> %.3f\n',r,c,t,D(r,c,t),C(r,c,t));
            end;
        end;
        fprintf('\n\t\t%.2f%% processed... ',(t-Kt+1)/(Nt-Kt+1)*100);
    end;
    
    if (hasNoise)        % if noise matrix is provided, add it in the result
        C=C+noise;
    end;
else  % (dimD==4)
    Nt=szD(4);     Nr=szD(1);     Nc=szD(2);     Nz=szD(3);
    Kt=size(K,4);  Kr=size(K,1);  Kc=size(K,2);  Kz=size(K,3);
    
    for t=Kt:Nt         % for every 'time frame' (k)
        for z=Kz:Nz         % for each plane (z) in current cube
            for r=Kr:Nr         % for each row (r) in current frame 
                for c=Kc:Nc         % for each column (c) in current row
                    % perform full kernel-wise AR (Kt) and MA (Kr.Kc.Kz) weighted sum
                    C(r,c,z,t)=sum(sum(sum(sum(D(r-Kr+1:r,c-Kc+1:c,z-Kz+1:z,t-Kt+1:t).*K(1:Kr,1:Kc,1:Kz,1:Kt)))));
                    %fprintf('(%d,%d,%d,%d) : %.3f -> %.3f\n',r,c,z,t,D(r,c,z,t),C(r,c,z,t));
                end;
            end;
        end;
    end;
    
    if (hasNoise)        % if noise matrix is provided, add it in the result
        C=C+noise;
    end;
end;

end

