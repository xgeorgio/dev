function status = gpiosim_pinMode( type, num, mode )
%gpiosim_pinMode - Reset the mode and value of a GPIOsim I/O device
%    status = gpiosim_pinMode( num, mode ) sets pin number num to 
%    type='A'/'D'/'C' (digital/analog/clock) and mode='I'/'O' (input/output)
%    and resets the I/O file;
%    it is a wrapper for creation and write functions internally.

% version 1.0b / 27-Feb-2021
% Harris Georgiou (c) 2021

gpiosim_clear(num);              % reset the GPIOsim I/O device (remove file)
gpiosim_new(type,num,mode);      % create a fresh GPIOsim I/I device with new specs
gpiosim_write(num,0);            % reset the filename with default value
