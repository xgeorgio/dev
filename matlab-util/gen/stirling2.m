%    Stirling numbers of 2nd kind 
%    ............................
%
%    Stirling numbers of 2nd kind calculate the
%    total number of ways to partition a set of
%    (n) items into (m) non-empty distinct
%    partitions, i.e. number of ways to distribute
%    (n) distinct items into (m) non-distinct holes.
%    Note: if empty partitions are allowed, the 
%    total number of partitions is:  (m)^n
%
%    Harris Georgiou (c) 2006
%:::::::::::::::::::::::::::::::::::::::::::::::::::::::

function s = stirling2( n, m )

if ((n==m)||(m==1)),
    s = 1;
else
    s = stirling2(n-1,m-1) + m*stirling2(n-1,m);
end;
