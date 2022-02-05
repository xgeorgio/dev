function [ status, value, elapsed ] = gpiosim_read( num )
%gpiosim_read - Read the value from an existing a GPIOsim I/O device
%    [status,value,elapsed] = gpiosim_read( num ) reads the I/O value
%    from the filename, i.e., the actual instead of the internal (buffered);
%    elapsed is the time (secs) since the latest actual I/O update;
%    return status determines if it was successful or not (true/false).

% version 1.0b / 26-Feb-2021
% Harris Georgiou (c) 2021


global gpiosim_const gpiosim_pool;     % use constants from globals (should be already initialized outside)

L=dir(strcat(gpiosim_pool.pin(num).path,'*.',gpiosim_pool.pin(num).filext));    % get all files for pin number
status=(size(L,1)==1);      % assume it is a single, otherwise flag as error (path not cleared)
if (status)                 % everything ok, I/O device can be read
    % get filename (no path, no extension), convert to proper double (internal)
    gpiosim_pool.pin(num).lastvalue=double(str2num(strcat('0x',L(1).name(1:end-4))));
    value=gpiosim_pool.pin(num).lastvalue;                   % update from internal buffer
    elapsed=(now-gpiosim_pool.pin(num).timestamp)*86400;     % convert to seconds (x24x3600);
else
    value=0;         % default return value on error
    elapsed=-1;      % default elapsed time on error (flag)
end
