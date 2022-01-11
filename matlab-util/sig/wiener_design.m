function [ W, Yp, d, err ] = wiener_design( X, Y, Norder )

rr=xcorr(X,Norder); 
rr=rr(Norder+1:end);
R=toeplitz(rr);

P=xcorr(X,Y,Norder);
P=P(Norder+1:end);

W=R\P;

Yp=filter(W,1,X);
err=sqrt(mean((Yp-Y).^2));
[~,d]=max(abs(W(2:end)));
