function status = gpiosim_writeDigital( num, value )
%gpiosim_writeAnalog - Write the digital value of an existing a GPIOsim I/O device
%    status = gpiosim_writeAnalog( num, value ) puts the value to pin number num;
%    it is a wrapper compatibility function that uses the generic gpiosim_update().

% version 1.0b / 27-Feb-2021
% Harris Georgiou (c) 2021


status = gpiosim_update(num,value);    % Note: no value checks, avoid delays
