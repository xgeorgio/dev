%---------------------------------------------------------------------------
%
%    PROJECT:        -
%    PACKAGE:        (general-purpose routines)
%    FILE:           normalize.m
%
%    PURPOSE:        Normalize matrix values  
%    VERSION:        0.1
%
%    UPDATED:        06-May-2004/17:07
%
%    HISTORY:        version 0.1: Implemented core functionality
%
%    DESCRIPTION:    The routine normalizes matrix data using the
%                    mean-stdev method or min-max method, based on
%                    the 'type' parameter.
%
%    COPYRIGHT:      Harris Georgiou (c) 2004
%
%---------------------------------------------------------------------------

function M=normalize( M0, type )

denom_limit = 1e-6;

switch (lower(type))
    case 'avgstd'
        M0avg=mean(M0(:));
        M0std=std(M0(:));
        M=(M0-M0avg)/max([M0std denom_limit]);
    case 'minmax'
        M0max=max(M0(:));
        M0min=min(M0(:));
        M=(M0-M0min)/max([(M0max-M0min) denom_limit]);
    otherwise
        M=M0;
end;
