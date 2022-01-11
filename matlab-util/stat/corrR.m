function R = corrR( X, maxlag )

rr=xcorr(X,maxlag); 
rr=rr(maxlag+1:end);
R=toeplitz(rr);

end;
