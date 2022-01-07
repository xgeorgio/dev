function [ cCMA, cCM1, cW, cSC ] = vxCreateComplexData( szR, szC, szT, flag_verbose, str_Sresz, str_Tresz )
%vxCreateComplexData - create complex-valued fMRI-like test data series.
%
%   [ cCMA, cCM1, cW, cSC ] = vxCreateComplexData() creates a sequence of 
%   fMRI-like 2-D matrices ('slices') based on typical components, 
%   including task-related, transient and artifacts. The default
%   (minimum) size for each slice is 60x60 and the default (minimum)
%   timecourse length is 100 frames. By default, no output/display
%   is provided during the calculations.
%
%   The 'slices' are fMRI-like images that are the result of eight
%   components, arranged as a series of 2-D spatial maps in matrix cSC:
%      S1 = task-related, main course (stimulus)
%      S2, S6 = task-related, transient
%      S3, S4, S5, S8 = artifact
%      S7 = artifact, scanner drift
%   Matrix cW is the mixture, i.e., the coefficients that are used for 
%   combining sources from S in order to construct a full 'slice' in 
%   each time step.
%   Matrix cCMA is the complete sequence of 'slices' with all the
%   'mixtures', converted into a full 2-D/3-D time series. 
%   Matrix cCM1 contains a similar time sequence, but only with the
%   main component included (task-related), often used as a simple
%   reference baseline for reconstruction and activation maps.
%
%   Due to internal specifics for various parameters and statistics,
%   the initial 'slice' size is set to 60x60 and the initial timecourse 
%   length to 100 frames. These sizes are rescaled, if necessary, by
%   appropriate methods with minimal artifacts introduction.
%
%   ... = vxCreateRealData( szR, szC ) creates data with the size of each 
%   'slice' rescaled to szRxszC by using 'imresize' with the algorithm 
%   specified in str_Sresz (string) if provided or 'lanczos3' by default
%   For very large sizes 'bilinear' can be used instead for faster but 
%   lower-quality resize. 
%
%   ... = vxCreateRealData( szR, szC, szT ) creates the length of the 
%   timecourse rescaled to szT frames by using 'interp1' with the algorithm
%   specified in str_Tresz (string) if provided or 'pchip' by default.
%   This is the 1-D piecewise shape-preserving cubic spline (min artifacts).
%
%   ... = vxCreateRealData( szR, szC, szT ) the 'flag_verbose' option 
%   determines if the CMA and CM1 series are plotted as side-by-side 
%   'animated' fMRI acquisition 'slices'. The verbose mode is active for 
%   any input <>0 and the value itself is used as a figure handler, so 
%   that it can be set not to overwrite any existing window.
%
%   ... = vxCreateRealData( szR, szC, szT, flag_verbose, str_Sresz, str_Tresz )
%   is the full calling convention with all input arguments specified.
%
%   Note: Due to stretching of the initial 100-frame timecourse, excessive 
%   resizing of the time scale may affect the statistical properties of some
%   of the sources' timeseries, i.e., equivalent to low-pass filtering and 
%   packing of covariance matrix around the diagonal. However, the noise
%   elements are added only after the final rescaling, so that similar
%   distortions are limited only to the general shape of each component's
%   activation pattern, e.g. 'stretching' the pulse activations in source-1
%   rather that adding more pulses in the series. Therefore, if this is 
%   not desired (experimentally important), the timecourses themselves can 
%   be expanded by concatenation of multiples of the initial 100 time frames 
%   and then the mixtures can be reconstructed manually.
%   Example:
%         [CA,C1,W,SC]=vxCreateComplexData();
%         W1=repmat(W(:,1),3,1);
%         C1_t300=W1*SC(1,:);
%
%   Note: This function is a wrapper for a script developed by MLSP
%   (http://mlsp.umbc.edu) and with some cleanup/optimizations:
%   *Source file (original): 'simulate_fmri_sources.m'
%   *Full reference (BIB):
%   N. Correa, Y.-O. Li, T. Adali, and V. Calhoun, "Comparison of blind
%   source separation algorithms for fMRI using a new Matlab toolbox: GIFT," 
%   in Proc. IEEE Int. Conf. Acoust., Speech, Signal Processing (ICASSP),
%   Philadelphia, PA, vol. 5, pp. 401-404, March 2005.
%
%   Ref: Harris Georgiou - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a> (CC-BY-SA) 29-May-2013

%... initialization / internal constants ...
xdim=60;
ydim=60;
xy_dim=xdim*ydim;
timeset=100;

if (nargin==0)          % no inputs, use all defaults
    szR=ydim;
    szC=xdim;
    szT=timeset;
    flag_verbose=0;
    str_Sresz='';
    str_Tresz='';
elseif (nargin==2)      % only frame size specified
    szT=timeset;
    flag_verbose=0;
    str_Sresz='lanczos3';
    str_Tresz='';
elseif (nargin==3)      % frame size and timecourse length specified
    flag_verbose=0;
    str_Sresz='lanczos3';
    str_Tresz='pchip';
elseif (nargin==4)      % sizes specified, verbose mode
    str_Sresz='lanczos3';   % 'bilinear': faster / 'lanczos3': high-quality
    str_Tresz='pchip';      % 'pchip': piecewise shape-preserving cubic spline
end;                    % otherwise, use all input agruments as-is

szR=max([ydim szR]);        % use at least 'ydim' rows
szC=max([xdim szC]);        % use at least 'xdim' columns
szT=max([timeset szT]);     % use at least 'timeset' frames

disp_fps=5;             % frames-per-second (approx) when displaying results

if (flag_verbose),
    fprintf('creating Complex-valued fMRI-like series:\n');
    fprintf('\tcreating components (#sources=8 / %dx%d)... ',szR,szC'); 
end;

%... Source 1: task-related (main) ...
pd1=2;
pd2=3;
w=0:2*pi/(xdim-1):2*pi;
S1=sin(pd1.*w')*sin(pd2.*w);
S1n=zeros(60,60);
for j=41:50
    for k=31:45
        S1n(k,j)=S1(k,j);
    end;
    for k=1:15
        S1n(k,j)=S1(k,j);
    end;
end;
S1=S1n;
phase = (rand(xdim)*20-10)/180*pi;      % pixel-wise random phase shift for spatial map (S1)
cS1 = S1.*exp(1i*phase);

%... Source 2: task-related (transient) ...
pd1=3;
pd2=4;
w=0:2*pi/(xdim-1):2*pi;
S2=sin(pd1.*w')*sin(pd2.*w);
S2n=zeros(60,60);
for k=11:20
    for j=9:15
        S2n(k,j)=S2(k,j);
    end;
end;
for k=31:40
    for j=9:15
        S2n(k,j)=S2(k,j);
    end;
end;
for k=21:30
    for j=31:37
        S2n(k,j)=S2(k,j);
    end;
end;
S2n=S2n+0.5;
S2=S2n;
A=S2(11:20, 9:15);
S2(31:40, 39:45)=A;
phase = (rand(xdim)*20-10)/180*pi;      % pixel-wise random phase shift for spatial map (S2)
S2 = S2-0.5;
cS2 = S2.*exp(1i*phase);

%... Source 3: artifact ...
pd1=0.5;
pd2=0.4;
w=0:3*pi/(xdim-1):3*pi;
S3=sin(pd1.*w')*cos(pd2.*w);
temp=zeros(60,60);
temp(1:38,1:26)=S3(1:38,1:26);
S3=fliplr(temp);
phase = (rand(xdim)*20-10)/180*pi;      % pixel-wise random phase shift for spatial map (S3)
S3 = abs(S3);
cS3 = S3.*exp(1i*phase);

%... Source 4: artifact ...
S4 = zeros(60,60);
cnt=1;
for k=20-cnt:-1:16
    for j=20+cnt:40-cnt
        S4(k,j)=0.25;
    end
    cnt=cnt+2;
end
cnt=1;
for j=20:40
    for k=25:-1:20
       S4(k,j)=0.25;
    end
end 
for k=22+cnt:1:25
    for j=30-cnt:30+cnt
        S4(k,j)=S4(k,j)-0.20;
    end
    cnt=cnt+3;
end
cnt=1;
 for k=40+cnt:1:44
    for j=20+cnt:40-cnt
        S4(k,j)=-0.25;
    end
    cnt=cnt+2;
end       
 cnt=1;
for j=20:40
    for k=40:-1:34
       S4(k,j)=-0.25;
    end
end 
for k=38-cnt:-1:34
    for j=30-cnt:30+cnt
        S4(k,j)=S4(k,j)+0.20;
    end
    cnt=cnt+3;
end       
S_4=zeros(60,60);
for k=2:59
    for j=2:59
        for s=1:3
            for t=1:3
                S_4(k,j)=S_4(k,j)+S4(k-2+s,j-2+t);
                %t=t+1;
            end
            %s=s+1;
        end
    end
end
S4=S_4;
S4 = abs(S4);
phase = (rand(xdim)*20-10)/180*pi;      % pixel-wise random phase shift for spatial map (S4)
cS4 = S4.*exp(1i*phase);

%... Source 5: artifact ...
pd1=.5;
pd2=.9;
w=0:4*pi/(xdim-1):4*pi;
t3=sin(pd1.*w')*sin(pd2.*w);
test=t3(33:57,35:50);
B=imresize(test,[21 11], 'bilinear');
S5=zeros(60,60);
S5(40:60,50:60)=-5*B;
phase = (rand(xdim)*20-10)/180*pi;      % pixel-wise random phase shift for spatial map (S5)
S5 = abs(S5);
cS5 = S5.*exp(1i*phase);

%... Source 6: task-related (transient)
pd1=.5;
pd2=.9;
w=0:4*pi/(xdim-1):4*pi;
S6=sin(pd1.*w')*sin(pd2.*w);
S6(:,1:33)=0;
S6(:,51:end)=0;
S6=rot90(rot90(S6));
s6=zeros(60,60);
s6(15:44,1:20)=S6(31:60,11:30);
s6(15:44,41:60)=S6(31:60,11:30);
s6=rot90(s6);
S6=s6;
phase = (rand(xdim)*20-10)/180*pi;      % pixel-wise random phase shift for spatial map (S6)
cS6 = S6.*exp(1i*phase);

%... Source 7: artifact (scanner drift)
S7 = zeros(60,60);
a=0:pi/5:pi;
b=0:pi/40:pi;
c = sin(b')*sin(a);
S7(10:50,27:32) = c;
S7(10:50,47:52) = c;
phase = (rand(xdim)*20-10)/180*pi;      % pixel-wise random phase shift for spatial map (S7)
cS7 = S7.*exp(1i*phase);

%... Source 8: artifact ...
S8=zeros(60,60);
for j=59:-1:1
    for k=1:60
        S8(k,j)=S8(k,j+1)+1/60;
    end;
end;
phase = (rand(xdim)*20-10)/180*pi;      % pixel-wise random phase shift for spatial map (S8)
cS8 = S8.*exp(1i*phase);

%... reshaping & resizing the sources ...

if ((szR==ydim)&&(szC==xdim))            % check for no-resize for speed enhancement
    S(1,:)=reshape(S1',1,xy_dim);   
    cS(1,:)=reshape(cS1',1,xy_dim);
    
    S(2,:)=reshape(S2',1,xy_dim);   
    cS(2,:)=reshape(cS2',1,xy_dim);
    
    S(3,:)=reshape(S3',1,xy_dim);   
    cS(3,:)=reshape(cS3',1,xy_dim);
    
    S(4,:)=reshape(S4',1,xy_dim);   
    cS(4,:)=reshape(cS4',1,xy_dim);
    
    S(5,:)=reshape(S5',1,xy_dim);   
    cS(5,:)=reshape(cS5',1,xy_dim);
    
    S(6,:)=reshape(S6',1,xy_dim);   
    cS(6,:)=reshape(cS6',1,xy_dim);
    
    S(7,:)=reshape(S7',1,xy_dim);   
    cS(7,:)=reshape(cS7',1,xy_dim);
    
    S(8,:)=reshape(S8',1,xy_dim);   
    cS(8,:)=reshape(cS8',1,xy_dim);
else                                    % otherwise resize all components (frames)
    S(1,:)=reshape(imresize(S1,[szR szC],str_Sresz)',1,szR*szC);     % use column-wise concat
    cS(1,:)=reshape(imresize(cS1,[szR szC],str_Sresz)',1,szR*szC);   %   of resized frames
    
    S(2,:)=reshape(imresize(S2,[szR szC],str_Sresz)',1,szR*szC);
    cS(2,:)=reshape(imresize(cS2,[szR szC],str_Sresz)',1,szR*szC);

    S(3,:)=reshape(imresize(S3,[szR szC],str_Sresz)',1,szR*szC);
    cS(3,:)=reshape(imresize(cS3,[szR szC],str_Sresz)',1,szR*szC);

    S(4,:)=reshape(imresize(S4,[szR szC],str_Sresz)',1,szR*szC);
    cS(4,:)=reshape(imresize(cS4,[szR szC],str_Sresz)',1,szR*szC);

    S(5,:)=reshape(imresize(S5,[szR szC],str_Sresz)',1,szR*szC);
    cS(5,:)=reshape(imresize(cS5,[szR szC],str_Sresz)',1,szR*szC);

    S(6,:)=reshape(imresize(S6,[szR szC],str_Sresz)',1,szR*szC);
    cS(6,:)=reshape(imresize(cS6,[szR szC],str_Sresz)',1,szR*szC);

    S(7,:)=reshape(imresize(S7,[szR szC],str_Sresz)',1,szR*szC);
    cS(7,:)=reshape(imresize(cS7,[szR szC],str_Sresz)',1,szR*szC);

    S(8,:)=reshape(imresize(S8,[szR szC],str_Sresz)',1,szR*szC);
    cS(8,:)=reshape(imresize(cS8,[szR szC],str_Sresz)',1,szR*szC);
end;


%... Create all timecourses ...
if (flag_verbose),  fprintf('done\n\tcreating time courses (T=%d)... ',szT);  end;

a_col    = ones([timeset+20 1]);
constant = 0.25;

% Time course 1
a_col_n = ones([timeset+20 1]);
for k=1:2:10
    a_col_n(12*k+1:12*k+12)=-1;
end
x = (1:1:12);
y = normpdf(x,6,0.8);
y=y';
z=conv(a_col_n,y);
alternating_col_new=size(1,timeset);
for k=1:100
    alternating_col_new(k)=z(k+7);
end

for k=1:2:12
    a_col(10*k+1:10*k+10)=-1;
end

x = (1:1:12);
y = normpdf(x,6,0.8);
y=y';
z=conv(a_col,y);
alternating_col=size(1,timeset);
for k=1:timeset
    alternating_col(k)=z(k+7);
end

% Time course 2
alternating_col=alternating_col';
d=size(1,timeset+1);
for k=1:timeset
    d(k)=alternating_col(k);
end
d(1,101)=z(108,1);
dif=diff(d);
derivative_col=dif';

% Time course 3
w_col=0:2*pi/(timeset-1):2*pi;
sin_col= sin(w_col');
tmp=zeros(size(sin_col));
tmp(1:50,:)=sin_col(1:50,1);
tmp(51:100,:)=sin_col(51:100,1)-1;
sin_col=tmp;

% Time course 4
rand_plus_constant_col  = 0.5 + 0.05* randn([szT 1]);
idx=round(0.3*szT);
rand_plus_constant_col(szT-idx:szT)=rand_plus_constant_col(szT-idx:szT)+constant; 

% Time course 5
rand_col  = 0.75 + 0.07* randn([szT 1]);

% Time Course 6
triangle_col=zeros(100,1);
count=0;
for k=1:100
    value=count/10+ 0.05* randn;
    triangle_col(k,1)=value;
    count=count+1;
    count=mod(count,10);
end

x = (0.01:0.1:10);
p  = fpdf(x,5,20);
x1=zeros(1,20);
j=1;
for k=1: 20
    x1(1,k)=p(1,j);
    j=j+2;
end;
x2=zeros(1,100);
c=0;
for k=1:5
    for j=1:20
        x2(1,j+c)=x1(1,j);
    end;
    c=c+20;
end;
t=zeros(1,100);
t(1,1:20)=x2(1,1:20);
t(1,9:18)=t(1,9:18)+x2(1,21:30);
t(1,21:40)=t(1,1:20);
t(1,41:60)=t(1,1:20);
t(1,61:80)=t(1,1:20);
t(1,81:100)=t(1,1:20);

% Time Course 7
exponential_col=zeros(szT,1);
for k=1:szT
    exponential_col(k,1)=k^2/7000;
end
idx2=round(szT*2/3);
tmp=repmat(exponential_col(idx2:szT-1,1),3,1);
exponential_col(:,1)=interp1(1:size(tmp,1),tmp,linspace(1,size(tmp,1),szT),str_Tresz);
exponential_col(:,1) = exponential_col(:,1) + 0.03*randn(szT,1);

% Time Course 8
spike_col= 0.2 + 0.1* randn([szT 1]);
idx=round(0.35*szT);
spike_col(idx,1)=spike_col(idx,1)-2;

% populate W with all timecourses (cols), some rescaled
W(:,1)=interp1(1:timeset,(alternating_col_new'+2)*0.8,linspace(1,timeset,szT),str_Tresz);
W(:,2)=interp1(1:timeset,abs(derivative_col)*1.3,linspace(1,timeset,szT),str_Tresz);
W(:,3)=spike_col+2;
W(:,4)=rand_plus_constant_col;
W(:,5)=(rand_col+0.3*cos(0.05*[1:2:2*szT]'))*0.6;
W(:,6)=interp1(1:timeset,t'*1.2,linspace(1,timeset,szT),str_Tresz);
W(:,7)=(-1)*exponential_col+1.5;
W(:,8)=interp1(1:timeset,0.3*flipud(abs(sin_col)),linspace(1,timeset,szT),str_Tresz);

ph = 0.05;                  % random phase shift for time sequences
cW=zeros(size(W));
for j = 1:8
    cW(:,j)=W(:,j).*exp(i*ph*W(:,j));
end


%... Generate data & plot (if requested) ...
if (flag_verbose),  fprintf('done\n\treconstructing results... ');  end;

% Data = Mixmatrix * Sources
cX=cW*cS;                  % complete mixture (all sources)
cX1=cW(:,1)*cS(1,:);       % only main task-related component

cCMA=vxDevectorize(cX,szR,szC);         % convert to complete 2-D/3-D series
cCM1=vxDevectorize(cX1,szR,szC);        %   using row-wise reshape (custom)
cSC=vxDevectorize(cS,szR,szC);          % convert components too (spatial maps)

if (flag_verbose),  fprintf('done\n');  end;

if (flag_verbose)
    fprintf('\tdisplaying results (mixtures) at %d fps... ',disp_fps);
    for k=1:size(cX,1)
        figure(flag_verbose);
	
    	subplot(1,2,1);
        %imagesc(abs(cCM1(:,:,k)));
        %colormap gray;
        imshow(abs(cCM1(:,:,k)));
        title('clean input (source-1)');
	
        subplot(1,2,2);
        imagesc(abs(cCMA(:,:,k)));
        colormap gray;
        %imshow(abs(cCMA(:,:,k)));
        title('full mixture signal');
        
        pause(1/disp_fps);         % normally 4-5 samples per second
    end;
    fprintf('done\n');
end;
    
end
