%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%                                              %%%
%%%   Noise Cancelling using Wiener FIR filter   %%%
%%%   ----------------------------------------   %%%
%%%                                              %%%
%%%                                              %%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



disp('Noise Cancelling Using Wiener FIR Filter, Levinson Solution');

%... initialize constants and variables ...%
clear all;

disp('Initializing variables');
N=100;         % number of samples
M=6;           % filter order
c1=-0.5;       % noise v1(n)=c1*v1(n-1)+v(n)
c2=0.8;        % noise v2(n)=c2*v2(n-1)+v(n)
f0=0.075*pi;   % original signal s(n)=cos(f0*n)
nsc=1.0;       % scale noise range

v=zeros(1,N); v1=zeros(1,N); v2=zeros(1,N);
s=zeros(1,N);
rv=zeros(1,M); Pv=zeros(M,1);

%... create original (s(n)) and white noise (v(n),v1(n),v2(n)) signals ...%
v(1)=nsc*randn; v1(1)=v(1); v2(1)=v(1); s(1)=cos(f0);
for i=2:N,
    v(i)=nsc*randn;
    v1(i)=v1(i-1)*c1+v(i);
    v2(i)=v2(i-1)*c2+v(i);
    s(i)=cos(f0*i);
end

%... create correlation vectors rv(i),Pv(i) and matrix r(i) ...%
for i=1:M,
    rv(i)=(c2^(i-1))/(1-c2^2);
    Pv(i)=(c1^(i-1))/(1-c1*c2);
end


%----- Gauss algorithm (classic solution) -----%

disp('Calculating Gaussian solution');

r=toeplitz(rv(1:M));     % construct matrix r(i) from vector rv(i)
wg=(Pv\r)';              % calculate correct linear system solution


%----- Levinson algorithm -----%

disp('Calculating Levinson solution');

%... initialize variables ...%
a=1;
wl=Pv(1)/rv(1);
p=rv(1);

%... begin main loop ...%
for i=2:M;
    g(i-1)=-(rv(2:i)*flipud(a))/p;             % calculate (current) 'G' coefficient
    a=[a;0] + [0;conj(flipud(a))]*g(i-1);      % update 'A' matrix
    p=p*(1 - g(i-1)^2);                        % calculate 'P' coefficient
    k=(Pv(i)-(rv(2:i)*flipud(wl)))/p;          % calculate 'K' coefficient
    wl=[wl;0] + [conj(flipud(a))]*k;           % update 'W' matrix (solution of order 'i')
end

%... diplay Levinson and Gaussian solution vectors ...%
disp('Gaussian solution:');
wg
disp('Levinson solution:');
wl
disp('...<press any key to continue>...');
pause;


%... plot original signal ...%
disp('Creating original signal plot');
figure(1);
plot(s);
axis([1,N,-1.5,1.5]);
xlabel('time (n)');
ylabel('S(n)');
title('Original Signal');
disp('...<press any key to continue>...');
pause;


%... plot signal noise ...%
%disp('Creating signal noise plot');
%figure(6);
%plot(v1);
%axis([1,N,-1.5,1.5]);
%xlabel('time (n)');
%ylabel('v1(n)');
%title('White Noise V1');
%disp('...<press any key to continue>...');
%pause;


%... calculate and plot noisy signal ...%
disp('Creating noisy signal plot');

sv = s + v1;

figure(2);
plot(sv);
axis([1,N,-1.5,1.5]);
xlabel('time (n)');
ylabel('SV(n)');
title('Noisy Signal');
disp('...<press any key to continue>...');
pause;


%... calculate and plot restored signal (Gauss) ...%
%disp('Creating restored signal plot (Gaussian)');
%
%xe = filter(wg,1,v2);   % FIR filtering
%se = sv - xe;           % calculate estimate
%
%figure(5);
%plot(se);
%axis([1,N,-1.5,1.5]);
%xlabel('time (n)');
%ylabel('Se(n)');
%title('Estimated Signal (Gaussian)');
%disp('...<press any key to continue>...');
%pause;


%... calculate and plot restored signal (Levinson) ...%
disp('Creating restored signal plot (Levinson)');

xe = filter(wl,1,v2);   % FIR filtering
se = sv - xe;           % calculate estimate

figure(3);
plot(se);
axis([1,N,-1.5,1.5]);
xlabel('time (n)');
ylabel('Se(n)');
title('Estimated Signal (Levinson)');
disp('...<press any key to continue>...');


disp('Process completed.');


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


