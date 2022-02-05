function status = gpiosim_update( num, dvalue )
%gpiosim_update - Update the value of an existing GPIOsim I/O device
%    status = gpiosim_update( n, value ) puts the value to pin number num
%    and updates the corresponding filename in path, 'touching' the file;
%    return status determines if it was successful or not (true/false).

% version 1.0b / 26-Feb-2021
% Harris Georgiou (c) 2021


global gpiosim_const gpiosim_pool;     % use constants from globals (should be already initialized outside)

value=uint32(floor(dvalue));           % convert to 32-bit unsigned int (default storage)
if (gpiosim_pool.pin(num).lastvalue==value)         % check if no file I/O is necessary
    status=true;
    gpiosim_pool.pin(num).timestamp=now;            % light update, touch only the timestamp
else
    % new value available, full update is necessary, create new filename
    newfilename=strcat(num2str(value,'%X'),'.',gpiosim_pool.pin(num).filext);
    str1=strcat(gpiosim_pool.pin(num).path,gpiosim_pool.pin(num).lastfilename);
    str2=strcat(gpiosim_pool.pin(num).path,newfilename);
    status=movefile(str1,str2);     % perform in-place filename update
    if (status)                     % check if file I/O was successful
        gpiosim_pool.pin(num).lastvalue=value;             % update internal value
        gpiosim_pool.pin(num).lastfilename=newfilename;    % update internal filename
        gpiosim_pool.pin(num).timestamp=now;               % touch the timestamp
    end
end
