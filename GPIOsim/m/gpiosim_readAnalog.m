function value = gpiosim_readAnalog( num )
%gpiosim_readAnalog - Read the analog value from an existing a GPIOsim I/O device
%    value = gpiosim_read( num ) reads the I/O value from the filename, i.e., the 
%    actual instead of the internal (buffered), for I/O pin num;
%    it is a wrapper compatibility function that uses the generic gpiosim_read().

% version 1.0b / 26-Feb-2021
% Harris Georgiou (c) 2021


[~,value,~] = gpiosim_read(num);
