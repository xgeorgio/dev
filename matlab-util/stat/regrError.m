function [MAPE, RMSE] = regrError( T, X, Y, Z, ksz, method )

MAPE=[];  RMSE=[];

k=1;  N=length(T);
while (k+1+ksz<=N)
    idx=[1:k k+ksz+1:N]'; 
    idxq=[k+1:k+ksz]';    
    TT=T(idx);  
    TTq=T(idxq);
    XYZ=[X(idx) Y(idx) Z(idx)];
    XYZt=[X(idxq) Y(idxq) Z(idxq)];

    XYZq=interp1(TT,XYZ,TTq,method);
    
    Err1=mean(abs(XYZt-XYZq),1);
    Err2=sqrt(mean((XYZt-XYZq).^2,1));
    MAPE=[MAPE; Err1];
    RMSE=[RMSE; Err2];
    
    k=k+1;
end
