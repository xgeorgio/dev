%---------------------------------------------------------------------------
%
%    PROJECT:        (General Purpose Routines)
%    PACKAGE:        General Signal Processing
%    FILE:           'signal_stats.m'
%
%    PURPOSE:        Calculate the standard statistics of a 1D signal
%    VERSION:        1.0
%
%    UPDATED:        16-Jul-2007/09:50
%
%    HISTORY:        version 1.0: implemented core functionality (16-Jul-2007/09:50)
%
%    DESCRIPTION:    Calculate the eight standard scalar statistics of a 1D signal,
%                    namely: minimum, maximum, range, mean, stdev, skewness, kurtosis
%                    and entropy.
%
%    INPUT:          X = [1xN] base signal values (data series)
%
%    OUTPUT:         mn = minimum value of input signal
%                    mx = maximum value of input signal
%                    rng = range of values of input signal
%                    avg = mean value of input signal
%                    sdv = stdev value of input signal
%                    skw = skewness value of input signal
%                    kur = kurtosis value of input signal
%                    ent = entropy value of input signal
%
%    COPYRIGHT:      Harris Georgiou (c) 2007
%
%---------------------------------------------------------------------------


function [ mn, mx, rng, avg, sdv, skw, kur, ent ] = signal_stats( X, nbins )

mn = min(X);
mx = max(X);
rng = mx-mn;
avg = mean(X);
sdv = std(X);
skw = skewness(X);
kur = kurtosis(X);
ent = signal_entropy(X,nbins);

