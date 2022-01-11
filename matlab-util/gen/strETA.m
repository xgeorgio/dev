function str = strETA( iter, lap, iterN )
%strETA - Translate remaining iterations to remaining time as a text string
%
%   str = strETA( iter, lap, iterN ) takes as the number of iterations finished
%   and the corresponding elapsed time in secs and estimates the remaining time
%   providing the full dd/hh/mm/ss format as a text string.
%
%   Example:
%       disp( strETA( 145, 2456.5, 1000 ) );
%
%   See also: time2eta
%

rem=lap/iter*(iterN-iter);
    
% days=floor(rem/(24*60*60));  rem=rem-days*(24*60*60);
% hours=floor(rem/(60*60));   rem=rem-hours*(60*60);
% mins=floor(rem/60);  rem=rem-mins*60;
% secs=round(rem);
    
[days,hours,mins,secs]=time2eta(rem);
    
if (days>0),
    str=sprintf('%d(d):%02d(h):%02d(min):%03.1f(sec)',days,hours,mins,secs);
elseif (hours>0),
    str=sprintf('%02d(h):%02d(min):%03.1f(sec)',hours,mins,secs);
elseif (mins>0),
    str=sprintf('%02d(min):%03.1f(sec)',mins,secs);
else
    str=sprintf('%03.1f(sec)',secs);
end;        

end
