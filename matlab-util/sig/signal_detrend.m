function [ Yw, B ] = signal_detrend( X, Y )

Xt=[X ones(length(X),1)];
B=Xt\Y;
Yw=Y-Xt*B;

end
