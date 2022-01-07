% (demo script) Create block-based fMRI data series and perform decomposition
%----------------------------------------------------------------------------
%
%    PROJECT:        Sparse/fMRI
%    PACKAGE:        Code Demo/Template Routines
%    FILE:           'run1_createblockseries.m'
%
%    PURPOSE:        create block pattern series and decompose
%    VERSION:        2.0
%
%    STAGE:          BETA
%    UPDATED:        19-Sept-2013/20:55
%
%    HISTORY:        version 2.0: added KSVD processing section (19-Sept-2013/20:15)
%                    version 1.9: using modular series creation (27-May-2013/20:35)
%                    version 1.8: corr/p-value activation maps (12-May-2013/19:25)
%                    version 1.7: ARMA processing on pattern series (09-May-2013/17:35)  
%                    version 1.6: unified vxVectorize/vxDevectorize (08-May-2013/22:35)
%                    version 1.5: added MP processing section (05-May-2013/19:35)
%                    version 1.4: added ICA processing section (03-May-2013/20:30)
%                    version 1.3: added PCA processing section (01-May-2013/21:30)
%                    version 1.2: added multiple series types (02-May-2013/18:10)
%                    version 1.1: added PCA processing section (01-May-2013/21:10)
%                    version 1.0: implemented core functionality (30-Apr-2013/18:40)
%
%    DESCRIPTION:    Use basic routines from the package to create 1-D block templates,
%                    generate 2-D patterns with them and employ activation series to
%                    construct complete 2-D/3-D series to simulate fMRI 'slice' series.
%                    The 3-D data are then vectorized ('flattened') and devectorized
%                    ('deflattened'), enriched with noise and decomposed in various
%                    ways, including (partial) PCA, etc.
%                    Note: Almost all algorithms assume some noise component, which is
%                    normally removed. If no noise has been intruduced, some of the 
%                    reconstruction stages may not work properly.
%
%    INPUT:          (see source code for details)
%
%    OUTPUT:         (see source code for details)
%
%    SEE ALSO:       -
%
%    AUTHOR:         Harris Georgiou (MSc,PhD) - <a href="matlab:web('http://xgeorgio.info')">xgeorgio.info</a>
%    COMPANY:        Informatics & Telecommunications Department,
%                    Kapodistrian University of Athens (NKUA/UoA)
%    COPYRIGHT:      Harris Georgiou (CC-BY-SA) 19-Sept-2013
%
%---------------------------------------------------------------------------


%... initialize ...
clear all;
close all;

flag_Sdep=1;            % implement spatial dependency in pattern series
flag_Tdep=1;            % implement temporal dependency in pattern series

flag_imshow=1;          % show sequence of images ('slices')
flag_pause=0;           % pause between plots/results

flag_doARMA=0;          % apply ARMA filtering before full analysis
arma_kernelV=[1 4 1];   %   MA: base 1-D mask vector, converting to 2-D
arma_kernelT=[1 3 6];   %   AR: base timeseries weights (t=0: rightmost)

flag_noise=1;           % add Gaussian noise (with or without ARMA)
noise_m=0.0;
noise_s=0.15;

flag_calcCorr=0;        % show correlation map after analysis/decomposition
flag_calcPval=0;        % show p-value map after analysis/decomposition
Pval_limit=0.001;       % p-value activation threshold (<= means 'active')

flag_doHRF=1;           % apply HRF convolution (BOLD response)

flag_doPCA=1;           % perform PCA analysis/reconstruction
pca_Kp=2;               % number of PCA components to retain

flag_doGLM=0;           % perform GLM (mvregress) analysis/reconstruction

flag_doICA=1;           % perform ICA (fastica) analysis/reconstruction
ica_Kp=3;               % number of ICA components to retain

flag_doMP=0;            % perform MP (wmpalg) analysis/reconstruction
mp_alg='OMP';           % MP algorithm to use: 'BMP', 'OMP', 'WMP'
mp_Kp=2;                % Mp reconstruction components (use -1 for all)

flag_doKSVD=1;          % perform KSVD analysis/reconstruction
ksvd_Kp=5;              % number of total KSVD components (dictionary size)
ksvd_Lp=3;              % number of used KSVD components (sparsity level, Lp<=Kp)

tic;

%... create basic patterns and series ...
fprintf('Generating box series: two boxes, custom spatial/temporal dependence\n\n');

[BS1,BT1]=vxCreateSimData(flag_Sdep,flag_Tdep);     % create sequence for default sizes

k1=200;  k2=200;            % default 'slice' size is 200x200
T1=BT1(:,1);                % default timeseries length is 100
T2=BT1(:,2);
Nelem=size(BS1,1)*size(BS1,2)*size(BS1,3);      % total number of data elements (cells)

%... apply ARMA/noise in series ...
if (flag_doARMA)       % employ full ARMA (noiseless)
    Asz1=length(arma_kernelV);          % get 2-D mask size
    Asz2=length(arma_kernelT);
    fprintf('\tARMA preprocessing (Kernel:[%dx%d]x[%d]) into patterns series... ',Asz1,Asz1,Asz2);
    kernelBS=zeros(Asz1,Asz1,Asz2);     % create 2-D/3-D ARMA kernel
    for k=1:Asz2        % convert 1-D vector to 2-D mask, weighted along time axis 
        kernelBS(:,:,k)=(arma_kernelV'*arma_kernelV)*arma_kernelT(k);
    end;
    kernelBS=kernelBS/(Asz1*Asz1*Asz2);     % normalize ARMA kernel before any use
    BS1=vxARMA(BS1,kernelBS);               % perform ARMA processing (no noise added here)
end;

if (flag_noise)    % add noise (after ARMA, if any)
    fprintf('done\n\tadding noise (m=%.2f/s=%.2f) into patterns series... ',noise_m,noise_s);
    BS1=BS1 + (randn(size(BS1))*noise_s+noise_m);
end;

%... prepare series for processing (flatten) ...
fprintf('done\n\tgenerating vectorized data series... ');
BV1=vxVectorize(BS1);                         % translate into 2-D 'flat' matrix (vxVectorize32)


%... basic HRF convolution block ...
if (flag_doHRF)
    fprintf('done\n\tperforming HRF convolution into patterns series... ');
    H=vxBOLDtwogammaHRF(33);
    H=signal_rescale(H,0,1);                % rescale HRF kernel to correct very low magnitude
    %H=H./max(H);                           %    simpler rescaling when assuming min(HRF)=0
    for k=1:size(BV1,2)
        BVtmp=conv(BV1(:,k),H);             % perform convolution per-column (voxels-timeseries)
        BV1(:,k)=BVtmp(1:size(BV1,1));      % replace column vector with new (HRF-convolved)
    end;
    BV1=signal_rescale(BV1,0,1);            % global rescaling of data back to 0...1 range
end;

if (flag_imshow)
    fprintf('done\n\treconstructing de-vectorized patterns series... ');
    BSR1=vxDevectorize(BV1,k1,k2);              % using: vxDevectorize32()

    fprintf('done\n\tdisplaying original pattern series... ');
    for k=1:size(BT1,1),
        figure(1);
        imshow(mat2gray(BSR1(:,:,k)'));
        title('Box Pattern Series: original');
    end;
end;

% calculate correlation/p-value maps
if ((flag_calcCorr)||(flag_calcPval))
    fprintf('done\n\tcalculating Correlation/P-value maps... ');
    % calculate correlation (RC) and p-value (PV) maps, convert to 2-D 'images' for plot
    % Note: PV should be examined along with sign(RC) for full component-wise inference
    [RC,PV]=vxCalcActivMap([T1 T2],BV1,Pval_limit);
    RCM=vxDevectorize(RC,k1,k2);
    PVM=vxDevectorize(PV,k1,k2);
    
    if (flag_calcCorr)
        figure(2);
        mesh(RCM(:,:,1));
        title('Correlation matrix: original series / comp.T1');
        
        figure(3);
        mesh(RCM(:,:,2));
        title('Correlation matrix: original series / comp.T2');
    end;
    
    if (flag_calcPval)
        figure(4);
        mesh(PVM(:,:,1));
        title('P-value matrix: original series / comp.T1');
        
        figure(5);
        mesh(PVM(:,:,2));
        title('P-value matrix: original series / comp.T2');
    end;
end;  

% calculate reconstruction inconsistency
fprintf('done\n\tSum.Abs.Diff: (transl)-(orig) = %.3f\n',sum(sum(sum(abs(BSR1-BS1))))/Nelem);

if (flag_pause)
    fprintf('\n\t...<press any key to continue>...\n');
    pause;
end;


%... basic PCA analysis/reconstruction block ...
if (flag_doPCA)
    fprintf('\n\tperforming PCA (Kp=%d) decomposition of patterns series... ',pca_Kp);
    % BV1 (NxL): rows N are samples, cols L are variables (dim)
    % COEFF (LxN): cols N are principal components (vectors) in descending signifficance
    % SCORE (NxN): rows N are projection of input rows onto COEFF components (vectors)
    % Note: disable normalization, for easier reconstruction
    [COEFF, SCORE, LATENT, TSQUARED, EXPLAINED, MU] = pca(BV1,'Centered',false);
    
    if (pca_Kp<=0),  pca_Kp=length(COEFF);  end;
    BVR2=SCORE(:,1:pca_Kp)*COEFF(:,1:pca_Kp)';   % PCA-reconstruct using only Kp largest components (no rescaling)
    
    if (flag_imshow)
        fprintf('done\n\tdisplaying PCA-reconstructed (Kp=%d/Vexpl=%.2f) pattern series... ',pca_Kp,sum(EXPLAINED(1:pca_Kp)));
        BSR2=vxDevectorize(BVR2,k1,k2);              % using: vxDevectorize32()
        for k=1:size(BT1,1),
            figure(1);
            imshow(mat2gray(BSR2(:,:,k)'));
            title('Box Pattern Series: PCA (reconstruction)');
        end;
    end;

    % calculate correlation/p-value maps
    if ((flag_calcCorr)||(flag_calcPval))
        fprintf('done\n\tcalculating Correlation/P-value maps... ');
        % calculate correlation (RC) and p-value (PV) maps, convert to 2-D 'images' for plot
        % Note: PV should be examined along with sign(RC) for full component-wise inference
        [RC,PV]=vxCalcActivMap([T1 T2],BVR2,Pval_limit);
        RCM=vxDevectorize(RC,k1,k2);
        PVM=vxDevectorize(PV,k1,k2);
    
        if (flag_calcCorr)
            figure(2);
            mesh(RCM(:,:,1));
            title('Correlation matrix: PCA series / comp.T1');
        
            figure(3);
            mesh(RCM(:,:,2));
            title('Correlation matrix: PCA series / comp.T2');
        end;
    
        if (flag_calcPval)
            figure(4);
            mesh(PVM(:,:,1));
            title('P-value matrix: PCA series / comp.T1');
        
            figure(5);
            mesh(PVM(:,:,2));
            title('P-value matrix: PCA series / comp.T2');
        end;
    end;

    % calculate reconstruction inconsistency
    fprintf('done\n\tSum.Abs.Diff: (PCA)-(orig) = %.3f\n',sum(sum(sum(abs(BSR2-BS1))))/Nelem);
    
    if (flag_pause)
        fprintf('\n\t...<press any key to continue>...\n');
        pause;
    end;
end;

%... basic GLM analysis/reconstruction block ...
if (flag_doGLM)
    fprintf('\n\tperforming GLM (mvregress) decomposition of patterns series... ');
    D=ones(length(T1),3);        % create standard design matrix (K cols) 1st is for B0
    D(:,2)=T1(:,1);              % add column: ideal response for box-1
    D(:,3)=T2(:,1);              % add column: ideal response for box-2  
    % BV1 (NxL): rows N are samples, cols L are variables (dim)
    % BETA (KxN): cols are GLM-beta coeffs (vectors), one for each data row (N)
    % Note: perform linear regression per-column (assummed independence)
    warning('off','MATLAB:rankDeficientMatrix');            % turn off warning about bad-conditioned matrices
    BETA=zeros(size(D,2),size(BV1,2));
    for c=1:size(BV1,2)                                     % treat each data column (L) independently
        [BETA(:,c),SIGMA,RESID]=mvregress(D,BV1(:,c),'algorithm','cwls');      % perform GLM on 'flatttened' box pattern series
    end;                                                    % using the same design matrix
    warning('on','MATLAB:rankDeficientMatrix');            % restore warning about bad-conditioned matrices
    
    BVR3=D*BETA;                        % GLM-reconstruct using the same design matrix (no rescaling)
    
    if (flag_imshow)
        fprintf('done\n\tdisplaying GLM-reconstructed (K=%d) pattern series... ',3);
        BSR3=vxDevectorize(BVR3,k1,k2);     % using: vxDevectorize32()
        for k=1:size(BT1,1),
            figure(1);
            imshow(mat2gray(BSR3(:,:,k)'));
            title('Box Pattern Series: GLM (reconstruction)');
        end;
    end;
    
    % calculate correlation/p-value maps
    if ((flag_calcCorr)||(flag_calcPval))
        fprintf('done\n\tcalculating Correlation/P-value maps... ');
        % calculate correlation (RC) and p-value (PV) maps, convert to 2-D 'images' for plot
        % Note: PV should be examined along with sign(RC) for full component-wise inference
        [RC,PV]=vxCalcActivMap([T1 T2],BVR3,Pval_limit);
        RCM=vxDevectorize(RC,k1,k2);
        PVM=vxDevectorize(PV,k1,k2);
    
        if (flag_calcCorr)
            figure(2);
            mesh(RCM(:,:,1));
            title('Correlation matrix: GLM series / comp.T1');
        
            figure(3);
            mesh(RCM(:,:,2));
            title('Correlation matrix: GLM series / comp.T2');
        end;
    
        if (flag_calcPval)
            figure(4);
            mesh(PVM(:,:,1));
            title('P-value matrix: GLM series / comp.T1');
        
            figure(5);
            mesh(PVM(:,:,2));
            title('P-value matrix: GLM series / comp.T2');
        end;
    end;
    
    % calculate reconstruction inconsistency
    fprintf('done\n\tSum.Abs.Diff: (GLM)-(orig) = %.3f\n',sum(sum(sum(abs(BSR3-BS1))))/Nelem);

    if (flag_pause)
        fprintf('\n\t...<press any key to continue>...\n');
        pause;
    end;    
end;

%... basic ICA analysis/reconstruction block ...
if (flag_doICA)
    fprintf('\n\tperforming ICA (fastica) decomposition of patterns series... ');
    % BV1 (NxL): rows N are samples, cols L are variables (dim)
    % A (NxK): ICA mixing matrix for K components
    % icasig (KxL): rows K are ICA components (vectors) of length L
    % Note: decompose into two main ICA components plus one more for noise
    [icasig, A, Demix] = fastica (BV1,'numOfIC',ica_Kp,'g','tanh','stabilization','on');

    CP=zeros(1,size(icasig,1));             % perform ICA components sorting and selection:
    for c=1:size(icasig,1)
        CP(c)=sum(icasig(c,:).^2);          %   (1) calculate total energy of each component
    end;
    [CPS,IS]=sort(CP,'descend');            %   (2) sort component in descending power order
    A=A(:,IS);                              %   (3) rearrange components in matrices
    icasig=icasig(IS',:);
    if (size(icasig,1)>1)
        ica_Kp=size(icasig,1)-1;            %   (4) remove min-power component (assummed noise)
    else                                    %       warning: bad reconstruction if no noise
        ica_Kp=1;
    end;
    
    BVR4=A(:,1:ica_Kp)*icasig(1:ica_Kp,:);      % ICA-reconstruct using the (reduced) components
    
    if (flag_imshow)
        fprintf('done\n\tdisplaying ICA-reconstructed (Kp=%d) pattern series... ',ica_Kp);
        BSR4=vxDevectorize(BVR4,k1,k2);             % using: vxDevectorize32()
        for k=1:size(BT1,1),
            figure(1);
            imshow(mat2gray(BSR4(:,:,k)'));
            title('Box Pattern Series: ICA (reconstruction)');
        end;
    end;

    % calculate correlation/p-value maps
    if ((flag_calcCorr)||(flag_calcPval))
        fprintf('done\n\tcalculating Correlation/P-value maps... ');
        % calculate correlation (RC) and p-value (PV) maps, convert to 2-D 'images' for plot
        % Note: PV should be examined along with sign(RC) for full component-wise inference
        [RC,PV]=vxCalcActivMap([T1 T2],BVR4,Pval_limit);
        RCM=vxDevectorize(RC,k1,k2);
        PVM=vxDevectorize(PV,k1,k2);
    
        if (flag_calcCorr)
            figure(2);
            mesh(RCM(:,:,1));
            title('Correlation matrix: ICA series / comp.T1');
        
            figure(3);
            mesh(RCM(:,:,2));
            title('Correlation matrix: ICA series / comp.T2');
        end;
    
        if (flag_calcPval)
            figure(4);
            mesh(PVM(:,:,1));
            title('P-value matrix: ICA series / comp.T1');
        
            figure(5);
            mesh(PVM(:,:,2));
            title('P-value matrix: ICA series / comp.T2');
        end;
    end;
    
    % calculate reconstruction inconsistency
    fprintf('done\n\tSum.Abs.Diff: (ICA)-(orig) = %.3f\n',sum(sum(sum(abs(BSR4-BS1))))/Nelem);

    if (flag_pause)
        fprintf('\n\t...<press any key to continue>...\n');
        pause;
    end;    
end;

%... basic MP analysis/reconstruction block ...
if (flag_doMP)
    fprintf('\n\tperforming MP:%s (wmpalg) decomposition of patterns series... ',mp_alg);
    D=ones(length(T1),3);        % create standard dictionary matrix (K cols) 1st is for B0
    D(:,2)=T1(:,1);              % add column: ideal response for box-1
    D(:,3)=T2(:,1);              % add column: ideal response for box-2  
    % BV1 (NxL): rows N are samples, cols L are variables (dim)
    % MPC (KxL): rows K are dictionary components (coeffs) calculated by MP
    % MPI (KxL): rows K are dictionary components (indices) calculated by MP
    % Note: perform MP decomposition per-column (assummed independence)
        
    MPC=zeros(size(D,2),size(BV1,2));
    MPI=zeros(size(D,2),size(BV1,2));
    for c=1:size(BV1,2)                                     % treat each data column (L) independently        
        [YFIT,R,MPC(:,c),IOPT,QUAL,DD] = wmpalg(mp_alg,BV1(:,c),D);      % perform MP on 'flatttened' box pattern series
        MPI(:,c)=IOPT';
    end;                                                    % using the same dictionary matrix
    
    if ((mp_Kp<1)||(mp_Kp>size(BV1,2)))
        mp_Kp=size(BV1,2);
    end;
    BVR5=zeros(size(BV1));
    for c=1:size(BV1,2)                 % MP-reconstruct using the same (original) dictionary matrix (normalized)
        BVR5(:,c)=DD(:,MPI(1:mp_Kp,c)')*MPC(MPI(1:mp_Kp,c),c);      % using only the top Kp (sparse) components
    end;
    %BVR5=DD*MPC;                       % alternatively, use for full MP reconstruction (all components)
    
    if (flag_imshow)
        fprintf('done\n\tdisplaying MP:%s-reconstructed (Kp=%d) pattern series... ',mp_alg,mp_Kp);
        BSR5=vxDevectorize(BVR5,k1,k2);     % using: vxDevectorize32()
        for k=1:size(BT1,1),
            figure(1);
            imshow(mat2gray(BSR5(:,:,k)'));
            title('Box Pattern Series: MP (reconstruction)');
        end;
    end;
    
    % calculate correlation/p-value maps
    if ((flag_calcCorr)||(flag_calcPval))
        fprintf('done\n\tcalculating Correlation/P-value maps... ');
        % calculate correlation (RC) and p-value (PV) maps, convert to 2-D 'images' for plot
        % Note: PV should be examined along with sign(RC) for full component-wise inference
        [RC,PV]=vxCalcActivMap([T1 T2],BVR5,Pval_limit);
        RCM=vxDevectorize(RC,k1,k2);
        PVM=vxDevectorize(PV,k1,k2);
    
        if (flag_calcCorr)
            figure(2);
            mesh(RCM(:,:,1));
            title('Correlation matrix: MP series / comp.T1');
        
            figure(3);
            mesh(RCM(:,:,2));
            title('Correlation matrix: MP series / comp.T2');
        end;
    
        if (flag_calcPval)
            figure(4);
            mesh(PVM(:,:,1));
            title('P-value matrix: MP series / comp.T1');
        
            figure(5);
            mesh(PVM(:,:,2));
            title('P-value matrix: MP series / comp.T2');
        end;
    end;

    % calculate reconstruction inconsistency
    fprintf('done\n\tSum.Abs.Diff: (MP:%s)-(orig) = %.3f\n',mp_alg,sum(sum(sum(abs(BSR5-BS1))))/Nelem);
    
    if (flag_pause)
        fprintf('\n\t...<press any key to continue>...\n');
        pause;
    end;    
end;

%... basic KSVD analysis/reconstruction block ...
if (flag_doKSVD)
    fprintf('\n\tperforming KSVD decomposition of patterns series...\n');
    % BV1 (NxL): rows N are samples, cols L are variables (dim)
    % Dict (NxK): ICA mixing matrix for K components
    % outpar.CoefMatrix (KxL): rows K are ICA components (vectors) of length L
    % Note: sparse-encode into param.K dictionary atoms, use param.L atoms per signal (L<=K)
    
    param.K=ksvd_Kp;
    param.numIteration=10;
    param.errorFlag=0;          % fixed # atoms used (param.L)
    param.preserveDCAtom=1;
    param.L=ksvd_Lp;
    param.InitializationMethod='DataElements';
    param.displayProgress=1;
    param.errorGoal=1e-3;       % not used when param.errorFlag=0
    
    [Dict,outpar] = KSVD(BV1,param);
   
    BVR6=Dict*outpar.CoefMatrix;      % KSVD-reconstruct using the dictionary encoding
    
    if (flag_imshow)
        fprintf('done\n\tdisplaying KSVD-reconstructed (Kp=%d/Lp=%d) pattern series... ',ksvd_Kp,ksvd_Lp);
        BSR6=vxDevectorize(BVR6,k1,k2);             % using: vxDevectorize32()
        for k=1:size(BT1,1),
            figure(1);
            imshow(mat2gray(BSR6(:,:,k)'));
            title('Box Pattern Series: KSVD (reconstruction)');
        end;
    end;

    % calculate correlation/p-value maps
    if ((flag_calcCorr)||(flag_calcPval))
        fprintf('done\n\tcalculating Correlation/P-value maps... ');
        % calculate correlation (RC) and p-value (PV) maps, convert to 2-D 'images' for plot
        % Note: PV should be examined along with sign(RC) for full component-wise inference
        [RC,PV]=vxCalcActivMap([T1 T2],BVR6,Pval_limit);
        RCM=vxDevectorize(RC,k1,k2);
        PVM=vxDevectorize(PV,k1,k2);
    
        if (flag_calcCorr)
            figure(2);
            mesh(RCM(:,:,1));
            title('Correlation matrix: KSVD series / comp.T1');
        
            figure(3);
            mesh(RCM(:,:,2));
            title('Correlation matrix: KSVD series / comp.T2');
        end;
    
        if (flag_calcPval)
            figure(4);
            mesh(PVM(:,:,1));
            title('P-value matrix: KSVD series / comp.T1');
        
            figure(5);
            mesh(PVM(:,:,2));
            title('P-value matrix: KSVD series / comp.T2');
        end;
    end;
    
    % calculate reconstruction inconsistency
    fprintf('done\n\tSum.Abs.Diff: (KSVD)-(orig) = %.3f\n',sum(sum(sum(abs(BSR6-BS1))))/Nelem);

    if (flag_pause)
        fprintf('\n\t...<press any key to continue>...\n');
        pause;
    end;    
end;


%... cleanup / exit ...
fprintf('\n');

tt=toc;
fprintf('All finished in %.2f seconds\n',toc);
