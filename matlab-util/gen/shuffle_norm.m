%%---------------------------------------------------------------------------
%%
%%    FILE:           'shuffle_norm.m'
%%
%%    PURPOSE:        Randomly shuffle rows and normalize per column
%%    VERSION:        1.0
%%
%%    UPDATED:        21-Jun-2006/10:00
%%
%%    HISTORY:        version 1.0: implemented core functionality (21-Jun-2006/10:00)
%%
%%    DESCRIPTION:    This function can be used for pre-processing a dataset prior to
%%                    classifier training. It randomly shuffles the samples (rows) and
%%                    normalizes each feature (column) in a specific formulation. The
%%                    function also returns the shuffling index column, in case any
%%                    other matrix (external) has to follow the same row permutation.
%%
%%    INPUT:          data      : [N x M]  original N samples of M features
%%                    normmode  :  data normalization mode
%%                          'minmax'  = use the minimum-maximum normalization
%%                          'meanstd' = use the mean-stdev normalization
%%                          'none'    = do not apply any normalization
%%
%%    OUTPUT:         SNdata   :  [N x M]  shuffled/normalized N samples of M features
%%                    shuffle  :  [N x 1]  index column used in dataset shuffling
%%
%%    COPYRIGHT:      Harris Georgiou (c) 2006
%%
%%---------------------------------------------------------------------------


function [SNdata,shuffle] = shuffle_norm( data, normmode )

[RN,CN]=size(data);
if ((RN>0)&&(CN>0)),
    shuffle=randperm(RN)';
    data=data(shuffle,:);
    
    if (strcmpi(normmode,'minmax')),
        Vmin=min(data);
        Vmax=max(data);
        for i=1:size(data,2),
            Vscale=Vmax(i)-Vmin(i);
            if (Vscale==0), Vscale=1;  end;
            data(:,i)=(data(:,i)-Vmin(i))/Vscale;
        end;
    elseif (strcmpi(normmode,'meanstd')),
        Vmean=mean(data);
        Vstd=std(data);
        for i=1:size(data,2),
            Vscale=Vstd(i);
            if (Vscale==0), Vscale=1;  end;
            data(:,i)=(data(:,i)-Vmean(i))/Vscale;
        end;
    elseif (strcmpi(normmode,'none')),
        % do not normalize data
    else
        disp('Error: shuffle_norm: invalid normalization mode');
    end;
end;
    
SNdata=data;
