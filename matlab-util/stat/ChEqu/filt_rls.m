%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%                                             %%%
%%%   Adaptive Channel Equalization (LMS/RLS)   %%%
%%%   ---------------------------------------   %%%
%%%                                             %%%
%%%                 Harris Georgiou             %%%
%%%                                             %%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%--------  RLS recursive adaptation  --------%


function [W,E] = filt_rls(Ins,Des,ftaps,dtaps,delta,lambda,verbose)

%... begin RLS iterations ...%

N=size(Ins,2); M=ftaps; D=dtaps;
P=eye(M)/delta;
W=zeros(1,M); Uest=zeros(1,N);
ERRx=zeros(1,N); z=zeros(1,M); g=z;

for i=M:N-D,
    if (verbose~=0) disp(N-D-i+1); end;

    for j=1:M,
        z(j)=0;
        for k=1:M, z(j)=z(j)+P(j,k)*Ins(i-j+1); end;
    end;
    zz=0;  for j=1:M, zz=zz+Ins(i-j+1)*z(j); end;    
    g=z/(lambda+zz);

    ut=0;
    for j=1:M, ut=ut+W(j)*Ins(i-j+1); end;    % calculate current estimate
    Uest(i+D)=ut;

    ERRx(i)=Des(i+D)-Uest(i+D);               % calculate current error

    for j=1:M,
        W(j)=W(j)+ERRx(i)*g(j);       % adjust tap weights
    end;

    P=(P-g'*z)/lambda;
end;

E=Des-Uest;

