%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%                                             %%%
%%%   Adaptive Channel Equalization (LMS/RLS)   %%%
%%%   ---------------------------------------   %%%
%%%                                             %%%
%%%                 Harris Georgiou             %%%
%%%                                             %%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%--------  LMS recursive adaptation  --------%


function [W,E] = filt_lms(Ins,Des,ftaps,dtaps,mstep,verbose)

N=size(Ins,2); M=ftaps; D=dtaps;

%... begin LMS iterations ...%

W=zeros(1,M); Uest=zeros(1,N);
ERR=zeros(1,N);

for i=M:N-D,
    if (verbose~=0) disp(N-D-i+1); end;

    ut=0;
    for j=1:M, ut=ut+W(j)*Ins(i-j+1); end;    % calculate current estimate
    Uest(i+D)=ut;

    ERR(i)=Des(i+D)-Uest(i+D);                % calculate current error

    for j=1:M,
        W(j)=W(j)+mstep*ERR(i)*Ins(i-j+1);       % adjust tap weights
    end;
end;

E=Des-Uest;

