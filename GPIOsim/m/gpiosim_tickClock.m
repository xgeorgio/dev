function status = gpiosim_tickClock( num, pulseT )
%gpiosim_tickClock - Write the next clock state (hi/lo) of an existing GPIOsim I/O device
%    status = gpiosim_writeClock( num, pulseT ) checks the elapsed time from last update
%    against pulseT limit and writes 'high' (1) if <= or 'low' (0) if > assuming return-to-zero
%    it is a wrapper compatibility function that uses the generic gpiosim_update().

% version 1.0b / 27-Feb-2021
% Harris Georgiou (c) 2021


[status,~,elapsed] = gpiosim_read(num);
if (status)
    if (elapsed>pulseT)
        status = gpiosim_update(num,0);
    else   % elapsed<=pulseT
        status = gpiosim_update(num,1);
    end
end
