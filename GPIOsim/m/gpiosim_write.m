function status = gpiosim_write( num, value )
%gpiosim_write - Write the value of an existing a GPIOsim I/O device
%    status = gpiosim_write( num, value ) puts the value to pin number num
%    and updates the corresponding filename in path, creating a fresh file;
%    return status determines if it was successful or not (true/false).

% version 1.0b / 26-Feb-2021
% Harris Georgiou (c) 2021


global gpiosim_const gpiosim_pool;     % use constants from globals (should be already initialized outside)

fid=fopen(strcat(gpiosim_pool.pin(num).path,gpiosim_pool.pin(num).lastfilename),'w+');    % open/truncate file (new)
status=(fid~=-1);      % check for create error (e.g. access denied)
if (status)
    fclose(fid);       % close file handler (not used in updates)
    status=gpiosim_update(num,value);     % call the standard pin update function
end
