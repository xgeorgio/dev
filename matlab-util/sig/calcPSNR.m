function R = calcPSNR( X0, X )
%calcPSNR - calculate Peak Signal-to-Noise Ratio in approximations.
%
%   R = calcPSNR( X0, X ) returns the estimated PSNR value between
%   the 'ideal' signal X0 and its approximation X, where both X0 and
%   X are NxL matrices. If L>1 then R is a 1xL vector of PSNR values.
%
%   See also: -
%

% check arguments, apply default actions
if ((size(X0,1)~=size(X,1))||(size(X0,2)~=size(X,2))),
    error('Error: calcPSNR: incompatible matrix dimensions');
else
    R=zeros(1,size(X0,2));
    for k=1:size(X0,2),
        % perform translation and rescaling for X
        X(:,k)=X(:,k)*sign(corr(X(:,k),X0(:,k)));       % match sign
        m0=mean(X0(:,k));  s0=std(X0(:,k));             % target scale
        m=mean(X(:,k));  s=std(X(:,k));                 % current scale
        XX=((X(:,k)-m)/s)*s0+m0;                        % convert
        
        %figure(15); plot(1:length(X0(:,k)),X0(:,k),'b-',1:length(XX),XX,'r-'); pause;
        
        % main calculation:  PSNR = 20*log10( 1/RSSE )
        R(k) = 20*log10( sqrt(sum(sum(X0(:,k).^2))) / sqrt(sum(sum((X0(:,k)-XX).^2)) ));
    end;
end;

end
