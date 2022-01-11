%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%                                             %%%
%%%   Adaptive Channel Equalization (LMS/RLS)   %%%
%%%   ---------------------------------------   %%%
%%%                                             %%%
%%%                 Harris Georgiou             %%%
%%%                                             %%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



disp('Adaptive Channel Equalization, LMS & RLS adaptive filters');

%... initialize constants and variables ...%
clear all;

disp('Initializing variables');

N=200;         % number of samples
M=31;          % filter order
D=25;          % tap delay

H=[0.04 -0.05 0.07 -0.21 -0.5 0.72 0.36 0.0 0.21 0.03 0.07];
HN=size(H);
SNRdb=10;      % selected SNR level for receiver noise

for i=1:N, s(i)=cos((i-1)*0.075*pi+0.001*randn); end;
for i=1:N,
    if (s(i)>=0) s(i)=1; else s(i)=-1; end;
end;

%... plot original signal ...%
disp('Creating original signal plot');
figure(1);
plot(s);
axis([1,N,-2,2]);
xlabel('time (n)');
ylabel('S(n)');
title('Original Signal');
disp('...<press any key to continue>...');
pause;

ns=filter(H,1,s);

%... plot noisy signal signal ...%
%Hsqr=H.^2; HSS=0;
%for i=1:HN, HSS=HSS+Hsqr(i); end
%v=sqrt(HSS/(10^(SNRdb/10)));
%HSS
v=1/(10^(SNRdb/10));
rcvn_var=v
for i=1:N, nns(i)=ns(i)+randn*v; end;

%... plot noisy input signal ...%
disp('Creating noisy input signal');
figure(1);
plot(nns);
axis([1,N,-2,2]);
xlabel('time (n)');
ylabel('NNS(n)');
title('Noisy Input Signal');
disp('...<press any key to continue>...');
pause;


%--------  LMS recursive adaptation  --------%

%... begin LMS iterations ...%
disp(' ');
disp('Beginning LMS iterations...');

Ins=nns; Des=s;

min_mstep=0.001; max_mstep=filt_tip(Ins,M);
if max_mstep>1 max_mstep=1; end;
step_mstep=(max_mstep-min_mstep)/20;

[MS,ME,BMP]=filt_mst(Ins,Des,M,D,min_mstep,max_mstep,step_mstep,1);
%... plot LMS error vs mstep ...%
figure(2);
plot(MS,ME,'g');
xlabel('mstep');
ylabel('MSE');
title('LMS Error');
disp('...<press any key to continue>...');
pause;

mstep=MS(BMP);
[W1,E1]=filt_lms(Ins,Des,M,D,mstep,1);

disp('Best LMS configuration:');
mstep
W1

%... plot LMS error ...%
ens=abs(E1); fns=ens(M+D:N);
disp('Creating LMS error plot');
figure(2);
plot(fns);
xlabel('time (n)');
ylabel('MSE(n)');
title('LMS Error');
disp('...<press any key to continue>...');
pause;

%... plot restored signal ...%
rs=filter(W1,1,nns);
disp('Creating restored signal');
figure(3);
plot(rs);
axis([1,N,-2,2]);
xlabel('time (n)');
ylabel('rs(n)');
title('Restored Signal');
disp('...<press any key to continue>...');
pause;


%--------  RLS recursive adaptation  --------%

%... begin RLS iterations ...%
disp('Beginning RLS iterations...');

delta=0.001;  lambda = 1.0;
Ins=nns; Des=s;

min_lstep=0.5; step_lstep=0.1; max_lstep=1.0;

[LS,LE,BLP]=filt_lst(Ins,Des,M,D,delta,min_lstep,max_lstep,step_lstep,1);
%... plot RLS error vs mstep ...%
figure(2);
plot(LS,LE,'c');
xlabel('lstep');
ylabel('MSE');
title('RLS Error');
disp('...<press any key to continue>...');
pause;

lambda=LS(BLP);
[W2,E2]=filt_rls(Ins,Des,M,D,delta,lambda,1);

disp('Best RLS configuration:');
lambda
W2

%... plot RLS error ...%
ens=abs(E2); fns=ens(M+D:N);
disp('Creating RLS error plot');
figure(2);
plot(fns);
xlabel('time (n)');
ylabel('MSE(n)');
title('RLS Error');
disp('...<press any key to continue>...');
pause;

%... plot restored signal ...%
rs=filter(W2,1,nns);
disp('Creating restored signal');
figure(3);
plot(rs);
axis([1,N,-2,2]);
xlabel('time (n)');
ylabel('rs(n)');
title('Restored Signal');
disp('...<press any key to continue>...');
pause;


%... plot combined error graph ...%
disp('Creating combined error graph');
idx=[1:1:N-M-D+1]; LMSerr=abs(E1(M+D:N)); RLSerr=abs(E2(M+D:N));
figure(2);
plot(idx,LMSerr,'g',idx,RLSerr,'c');
xlabel('iterations (k)');
ylabel('error(k)');
title('LMS/RLS Error');
%disp('...<press any key to continue>...');
%pause;


disp('Process completed.');


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


