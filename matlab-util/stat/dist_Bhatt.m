%%---------------------------------------------------------------------------
%%
%%    PROJECT:        RBEM/CDM
%%    PACKAGE:        Weighted Majority Games (WMG)
%%    FILE:           'dist_Bhatt.m'
%%
%%    PURPOSE:        Calculate the Bhattacharyya distance and Chernoff bound for dataset
%%    VERSION:        1.0
%%
%%    UPDATED:        27-Jun-2006/06:20
%%
%%    HISTORY:        version 1.0: implemented core functionality (27-Jun-2006/06:20)
%%
%%    DESCRIPTION:    The function calculates the Bhattacharyya distance (metric B) for the
%%                    given dataset of a two-class separation problem, as well as the common
%%                    Chernoff bound or minimum Bayesian error estimation for this B-value.
%%                    Both values can be used to characterize the separability of the two
%%                    class distributions of the given problem that the dataset describes.
%%
%%    INPUT:          X  = [NxL] input data set of N samples with L features each 
%%                    Y  = [Nx1] classID column vector for N samples (C1:minY...C2:maxY)
%%
%%    OUTPUT:         CB = Chernoff Bound value for the separation condition (C1/C2)
%%                    B  = Bhattacharyya distance between the two-class subsets (C1/C2)
%%
%%    COPYRIGHT:      Harris Georgiou (c) 2006
%%
%%---------------------------------------------------------------------------


function [CB,B]=dist_Bhatt( X, Y )

tagC1=min(Y(:,1));
tagC2=max(Y(:,1));

C1=find(Y(:,1)==tagC1);
C2=find(Y(:,1)==tagC2);

X1=X(C1,:);
X2=X(C2,:);

m1=mean(X1);  S1=cov(X1);
m2=mean(X2);  S2=cov(X2);

SM=(S1+S2)/2;

B = 1/8 * (m1-m2) * (SM^-1) * (m1-m2)' + 1/2 * log( det(SM) / sqrt( det(S1)*det(S2) ) );

CB = sqrt( size(X1,1)/size(X,1) * size(X2,1)/size(X,1) ) * exp(-B);
