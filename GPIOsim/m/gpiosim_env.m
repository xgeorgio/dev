%gpiosim_env - Create the GPIOsim environment, constants and pins pool
%    gpiosim_env : (call without parameters) creates the globals

% version 1.0b / 26-Feb-2021
% Harris Georgiou (c) 2021


global gpiosim_const gpiosim_pool;

gpiosim_const.pinsN=10;                   % total number of pins to allocate
gpiosim_const.path='';                    % global full path for I/O pool (files)

gpiosim_const.typeDigital='D';            % define digital pins tag
gpiosim_const.numDigitalMin=0x00;         % min pin mumber
gpiosim_const.numDigitalMax=gpiosim_const.pinsN;    % max pin number
gpiosim_const.valDigitalMin=0x00;         % min value (1 bit)
gpiosim_const.valDigitalMax=0x01;         % max value (1 bit)

gpiosim_const.typeAnalog='A';             % define analog pins tag
gpiosim_const.numAnalogMin=0x00;          % min pin number
gpiosim_const.numAnalogMax=gpiosim_const.pinsN;     % max pin number
gpiosim_const.valAnalogMin=0x00;          % min value (32 bits)
gpiosim_const.valAnalogMax=0xFFFFFFFF;    % max value (32 bits)

gpiosim_const.typeClock='C';              % define clock pins tag
gpiosim_const.numClockMin=0x00;           % min pin number
gpiosim_const.numClockMax=gpiosim_const.pinsN;      % max pin number
gpiosim_const.valClockMin=0x00;           % min value (1 bit)
gpiosim_const.valClockMax=0x01;           % max value (1 bit)

gpiosim_const.modeInput='I';              % define input mode
gpiosim_const.modeOutput='O';             % define output mode

gpiosim_const.typeUndefined='-';          % undefined type (for init)
gpiosim_const.modeUndefined='-';          % undefined mode (for init)

gpiosim_pool.pin=[];     % pins pool available as pre-allocated global for fast access
for i=1:gpiosim_const.pinsN
    gpiosim_pool.pin(i).type=gpiosim_const.typeUndefined;      % I/O type
    gpiosim_pool.pin(i).number=i;                              % I/O pin number
    gpiosim_pool.pin(i).mode=gpiosim_const.modeUndefined;      % I/O mode
    gpiosim_pool.pin(i).path=gpiosim_const.path;               % set as global path
    gpiosim_pool.pin(i).filext='';                             % file extension
    gpiosim_pool.pin(i).lastvalue=-1;               % I/O value to be used in updates
    gpiosim_pool.pin(i).lastfilename='';            % last filename used (after update)
    gpiosim_pool.pin(i).timestamp=-1;               % timestamp to be used in updates
end    
clear i;     % leave the workspace clean of everything except globals
