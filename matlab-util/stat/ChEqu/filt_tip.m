%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%                                             %%%
%%%   Adaptive Channel Equalization (LMS/RLS)   %%%
%%%   ---------------------------------------   %%%
%%%                                             %%%
%%%                 Harris Georgiou             %%%
%%%                                             %%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%--------  LMS tap input power  --------%


function [mstep_max] = filt_tip(Ins,ftaps)

N=size(Ins,2); M=ftaps;

%... calculate tap input power & max step-size value ...%
SSum=0;
for k=0:M-1,
    sum=0;
    for i=k+1:N, sum=sum+Ins(i)*Ins(i); end;
    SSum=SSum+sum/(N-k);
end;

mstep_max=2/SSum;

