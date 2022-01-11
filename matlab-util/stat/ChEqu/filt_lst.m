%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%                                             %%%
%%%   Adaptive Channel Equalization (LMS/RLS)   %%%
%%%   ---------------------------------------   %%%
%%%                                             %%%
%%%                 Harris Georgiou             %%%
%%%                                             %%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%--------  find best RLS lambda value  --------%

function [lsteps,lerrs,bestlpos] = filt_lst(Ins,Des,ftaps,dtaps,delta,ls_min,ls_max,ls_step,verbose)

N=size(Ins,2); M=ftaps; D=dtaps;
bestlpos=1;  besterr=inf; c=1;

for lstep=ls_min:ls_step:ls_max,
    [W,E]=filt_rls(Ins,Des,M,D,delta,lstep,0);
    errssum=0.0;
    for k=1:size(E,2), errssum=errssum+E(k)*E(k); end;
    errssum=errssum/size(E,2);
    if (errssum<=besterr) bestlpos=c; besterr=errssum; end;
    lsteps(c)=lstep; lerrs(c)=errssum; c=c+1;
    if (verbose~=0)
        lstep
        errssum
    end;
end;

if (verbose~=0)
    bestlpos
    bestlstep=lsteps(bestlpos)
    besterr=lerrs(bestlpos)
end;

