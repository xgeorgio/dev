%gpiosim - Skeleton/example of a main state loop
%    This script demonstrates how a physical model can be implemented 
%    and integrated within the framework, so that external GPIO sensing 
%    can be simulated. The example is a thrust-assisted falling object
%    that uses simple linear control to remain at a specific altitude.
%
% version 1.0b / 27-Feb-2021
% Harris Georgiou (c) 2021


state0.height=100;
state0.speed=0;
state0.accel=-9.81;
state0.time=0;

iterN=1000;
model.dT=0.1;
model.thrust=0;
model.height_noise=0.5;
model.thrust_noise=0.1;

gpiosim_env;
gpiosim_clear(1);  gpiosim_new('A',1,'I');  gpiosim_write(1,0);

iter=0;  tload=0;
state1=state0;
finished=false;
while (~finished)
    % initialize loop
    t0=now;
    iter=iter+1;
    
    % Input: retrieve state from sensors (=> controller output)
    [~,model.thrust,~]=gpiosim_read(1);    % EXTERNAL: read uint32 value from pin I/O (filename) to double
    %fprintf('\t=> reading: model.thrust = %g\n',model.thrust);
    
    curr_thrust = model.thrust + randn(1,1)*model.thrust_noise;
    
    % process: estimate state transition
    delta_state.accel = (curr_thrust+state0.accel) - state1.accel;
    delta_state.speed = state1.accel * model.dT;
    delta_state.height = state1.speed*model.dT + 0.5*state1.accel*model.dT^2;
    delta_state.time = model.dT;
    
    % process: update state
    state1.accel = state1.accel + delta_state.accel;
    state1.speed = state1.speed + delta_state.speed;
    state1.height = state1.height + delta_state.height;
    state1.time = state1.time + delta_state.time;
    
    % artificial delay within the loop (for artificial slowdown)
    %pause(0.05);
    
    % output: send state info to sensors
    fprintf('[%d]\t=> T=%.1f(sec): H=%.1f(m), U=%.1f(m/sec), A=%.1f(m/sec^2)\n',iter,state1.time,state1.height,state1.speed,state1.accel);
    figure(1);
    hold on;
    axis([0 iterN*model.dT -10 250]);
    plot(state1.time,state1.height,'bo-');
    
    % CONTROLLER: simulate controller sensing + decisions/actions
    %curr_height = state1.height + randn(1,1)*model.height_noise;
    curr_height = 5*floor((state1.height + randn(1,1)*model.height_noise)/5);
    if ((curr_height<state0.height*3/4)&&(state1.speed<5))
        %model.thrust=-state0.accel*2.35*(0+(state0.height-curr_height)/state0.height);
        %model.thrust=-state0.accel*2*(1+(state0.height-curr_height)/state0.height);
        model.thrust=5*floor(-state0.accel*2*(1+(state0.height-curr_height)/state0.height) / 5);
    else
        model.thrust=0;
    end
    
    %fprintf('\t=> writing: model.thrust = %g (%X)\n',model.thrust,thr_o);    
    st=gpiosim_write(1,model.thrust);           % EXTERNAL: write/update uint32 value to pin I/O (filename)
          
    % check for timing
    dTreal=(now-t0)*(24*3600);
    %tload=tload+dTreal/model.dT;  fprintf('\t=> dt(avg.load)=%.3f%%\n',tload/iter*100);
    tload=dTreal/model.dT;  fprintf('\t=> dt(avg.load)=%.3f%%\n',tload*100);
    if (dTreal<model.dT),  pause(model.dT-dTreal);  end
    
    % check for finish
    finished = (iter>iterN)||(state1.height<0);
end

