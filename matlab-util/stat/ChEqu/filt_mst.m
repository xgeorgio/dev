%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%                                             %%%
%%%   Adaptive Channel Equalization (LMS/RLS)   %%%
%%%   ---------------------------------------   %%%
%%%                                             %%%
%%%                 Harris Georgiou             %%%
%%%                                             %%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%--------  find best LMS step size  --------%

function [msteps,merrs,bestmpos] = filt_mst(Ins,Des,ftaps,dtaps,ms_min,ms_max,ms_step,verbose)

N=size(Ins,2); M=ftaps; D=dtaps;
bestmpos=1;  besterr=inf;  c=1;

for mstep=ms_min:ms_step:ms_min+ms_max,
    [W,E]=filt_lms(Ins,Des,M,D,mstep,0);
    errssum=0.0;
    for k=1:size(E,2), errssum=errssum+E(k)*E(k); end;
    errssum=errssum/size(E,2);
    if (errssum<=besterr) bestmpos=c; besterr=errssum; end;
    msteps(c)=mstep; merrs(c)=errssum; c=c+1;
    if (verbose~=0)
        mstep
        errssum
    end;
end;

if (verbose~=0)
    bestmpos
    bestmstep=msteps(bestmpos)
    besterr=merrs(bestmpos)
end;

