function gpiosim_new( type, num, mode )
%gpiosim_new - Create a new GPIOsim I/O device before use
%    S = gpiosim_new( type, num, mode ) sets the parameters:
%        type  : 'A'=analog, 'D'=digital, 'C'=clock
%        num   : Pin number, valid range defined in globals
%        mode  : 'I'=input, 'O'=output

% version 1.0b / 26-Feb-2021
% Harris Georgiou (c) 2021


global gpiosim_const gpiosim_pool;     % use constants from globals (should be already initialized outside)

% check for valid I/O type
if (upper(type)==gpiosim_const.typeDigital)
    stype=gpiosim_const.typeDigital;
    % check for valid digital I/O number
    if ((num>=gpiosim_const.numDigitalMin)&&(num<=gpiosim_const.numDigitalMax))
        snum=num;
    else
        error('gpiosim_new: Invalid digital I/O number, valid range: [%d,%d]',gpiosim_const.numDigitalMin,gpiosim_const.numDigitalMax);
    end
elseif (upper(type)==gpiosim_const.typeAnalog)
    stype=gpiosim_const.typeAnalog;
    % check for valid analog I/O number
    if ((num>=gpiosim_const.numAnalogMin)&&(num<=gpiosim_const.numAnalogMax))
        snum=num;
    else
        error('gpiosim_new: Invalid analog I/O number, valid range: [%d,%d]',gpiosim_const.numAnalogMin,gpiosim_const.numAnalogMax);
    end
elseif (upper(type)==gpiosim_const.typeClock)
    stype=gpiosim_const.typeDigital;
    % check for valid clock I/O number
    if ((num>=gpiosim_const.numClockMin)&&(num<=gpiosim_const.numClockMax))
        snum=num;
    else
        error('gpiosim_new: Invalid clock I/O number, valid range: [%d,%d]',gpiosim_const.numClockMin,gpiosim_const.numClockMax);
    end
else
    error('gpiosim_new: Invalid I/O type, valid options: {%s,%s,%s}',gpiosim_const.typeDigital,gpiosim_const.typeAnalog,gpiosim_const.typeClock);
end

% check for valid I/O mode
if (upper(mode)==gpiosim_const.modeInput)
    smode=gpiosim_const.modeInput;
elseif (upper(mode)==gpiosim_const.modeOutput)
    smode=gpiosim_const.modeOutput;
else
    error('gpiosim_new: Invalid I/O mode, valid options: {%s,%s,%s}',gpiosim_const.modeInput,gpiosim_const.modeOutput,gpiosim_const.modeUndefined);
end

% all validation check completed, construct output structure
gpiosim_pool.pin(snum).type=stype;                 % I/O type
gpiosim_pool.pin(snum).number=snum;                % I/O number (pin)
gpiosim_pool.pin(snum).mode=smode;                 % I/O mode
gpiosim_pool.pin(snum).path=gpiosim_const.path;    % full path for file
gpiosim_pool.pin(snum).filext=strcat(stype,num2str(snum,'%02X'));      % file extension to be used
gpiosim_pool.pin(snum).lastvalue=0;                % I/O value to be used in updates
gpiosim_pool.pin(snum).lastfilename=strcat(gpiosim_const.path,'0.',gpiosim_pool.pin(snum).filext);    % full filename to be used
gpiosim_pool.pin(snum).timestamp=-1;               % timestamp to be used in updates

end
