%%---------------------------------------------------------------------------
%%
%%    PROJECT:        RBEM/CDM
%%    PACKAGE:        Weighted Majority Games (WMG)
%%    FILE:           'signal_smooth.m'
%%
%%    PURPOSE:        Eliminate sharp 'peaks' and 'sinks' in a 1-D signal
%%    VERSION:        1.0
%%
%%    UPDATED:        21-Jun-2006/10:00
%%
%%    HISTORY:        version 1.0: implemented core functionality (21-Jun-2006/10:00)
%%
%%    DESCRIPTION:    This function implements a standard FIR filter design for a smoothing
%%                    kernel of specific length and cutoff point, and it is applied to an
%%                    input signal in order to eliminate any sharp 'peaks' and 'sinks' of
%%                    width up to a specific limit (Wmax). The size of the kernel (fN)
%%                    determines the granularity (resolution) of the filter and the last
%%                    parameter (phasecorr) determines whether phase correction is applied.
%%
%%    INPUT:          X          : [dim x N]  vector(s) of input signal(s) of length N
%%                    fN         :  size of the FIR convolution kernel
%%                    Wmax       :  maximum width of peaks/sinks to be eliminated
%%                    phasecorr  :  phase correction by left shift of output
%%                          1 = apply left shift (phase/2) places
%%                          0 = do not apply any phase correction
%%
%%    OUTPUT:         Y   :  [dim x N]  vector(s) of output filtered signals(s) of length N
%%                    h   :  [1 x fN]  impulse response function of the designed FIR kernel
%%
%%    COPYRIGHT:      Harris Georgiou (c) 2006
%%
%%---------------------------------------------------------------------------


function [ Y, h ] = signal_smooth( X, fN, Wmax, phasecorr )

[dim,N]=size(X);
if ((fN>0)&&(Wmax<N)),
    Y = zeros(size(X));
    h = zeros(size(X,1),fN);
    for xi=1:dim,
        fc = N/Wmax;
        wc = fc * pi / (N/2);         %  wc = 2*pi/Wmax;
        
        Hd = zeros(fN,fN);
        wk = round( fN * wc/(2*pi) );
        wmid = fN/2;
        wmidr = round(wmid);
        wstp = wk/2;
        wfr = round(wmid-wstp+1);
        wto = round(wmid+wstp);
        Hd(wfr:wto,wfr:wto)=1;
        
        h2 = fwind1(Hd,hamming(fN));
        h(xi,:) = h2(wmidr,:);
        
        if (phasecorr>0),
            phc = floor(wmidr/2);
            XZ = [X(xi,:) zeros(1,round(phc))];
            YZ = filter(h,1,XZ);
            Y(xi,:) = YZ(1,phc+1:size(YZ,2));
        else
            Y(xi,:) = filter(h,1,X(xi,:));
        end;
        
        Y(xi,:) = abs( Y(xi,:) * ( sum(X(xi,:))/sum(Y(xi,:)) ) );
    end;
end;

