function [ szR, szC ] = findSubplotGridSize( N, ratio )
%findSubplotGridSize - Calculate optimal grid size for subplot().
%
%   [ szR, szC ] = findSubplotGridSize( N, ratio ) takes a positive N 
%   and a ratio and returns the closest-match RxC grid with cells >=N.
%
%   Parameter ratio is defined as sizeX/sizeY and must be positive, 
%   typically <1 for landscape mode. If omitted, the default ratio is
%   9/16 which is optimal for 16:9 displays.
%
%   The optimization is performed as a simple linear search (O(N)) of
%   a weighted error function, including goodness-of-fit against ratio
%   and count of cells in the final grid versus N.
%
%   See also: subplot

% check arguments, apply default actions
if (nargin<2),
    ratio=9/16;
elseif (ratio<0)
    error('Error: findSubplotGridSize: ratio must be positive');
end;

if (N<0),
    error('Error: findSubplotGridSize: input number N must be positive');
else    
    err=zeros(1,N);
    for k=1:N,
        % heuristic penalty: ratio deviation + (normalized) difference of count
        err(k) = 0.8*(k/ceil(N/k)-ratio)^2 + 0.2*(1/sqrt(N))*(k*ceil(N/k)-N)^2;
    end;
    [~,mi]=min(err);
    szR=mi;
    szC=ceil(N/szR);
end;

end

