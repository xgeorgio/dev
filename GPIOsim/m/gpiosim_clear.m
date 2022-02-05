function gpiosim_clear( num )
%gpiosim_clear - Clear status of a GPIOsim I/O device
%    gpiosim_clear( num ) resets the status of pin number num
%    and removes the corresponding file(s) from the path folder

% version 1.0b / 26-Feb-2021
% Harris Georgiou (c) 2021


global gpiosim_const gpiosim_pool;     % use constants from globals (should be already initialized outside)

delete(strcat(gpiosim_pool.pin(num).path,'*.',gpiosim_pool.pin(num).filext));
