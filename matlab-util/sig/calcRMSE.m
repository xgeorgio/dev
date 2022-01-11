function rmse = calcRMSE( X0, X )

rmse = sqrt( sum(sum( (X0-X).^2 )) / (size(X0,1)*size(X0,2)) );
