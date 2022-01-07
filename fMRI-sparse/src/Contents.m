% WORK Sparse/fMRI processing toolbox
% Version 1.11 25-Sept-2013
% Harris Georgiou - http://xgeorgio.info (CC-BY-SA)
%
% Files
%   run1_createblockseries - (demo script) Create block-based fMRI data series and perform decomposition
%   run2_createRealSeries  - (demo script) Create realistic fMRI data series and perform decomposition
%   calcSpersity         - calculate per-column sparsity of a matrix
%   signal_rescale         - signal_rescale - Perform simple rescaling (windowing) to a data series.
%   signal_white         - signal_rescale - Perform simple mean/std whitening to a data series.
%   vxARMA                 - vxARMA - Perform 3-D/4-D ARMA (plus noise) filtering.
%   vxBOLDtwogammaHRF      - vxBOLDtwogammaHRF - Create a sequence of Gamma-based HRF for BOLD (fMRI).
%   vxBoxPatternCreate     - vxBoxPatternCreate - Create a 2-D grid of a specified pattern.
%   vxBoxSeriesCreate      - vxBoxSeriesCreate - Create a sequence of 2-D specified patterns.
%   vxCalcActivMap         - vxCalcActivMap - calculate correlation/p-value maps
%   vxCreateComplexData    - vxCreateComplexData - create complex-valued fMRI-like test data series.
%   vxCreateRealData       - vxCreateRealData - create real-valued fMRI-like test data series.
%   vxCreateSimData        - vxCreateSimData - create standard fMRI-like test data series.
%   vxDevectorize          - vxDevectorize - convert a 2-D matrix back to 3-D/4-D grid
%   vxDevectorize32        - vxDevectorize32 - convert a 2-D 'flat' matrix back to 3-D grid.
%   vxDevectorize32rs      - vxDevectorize32rs - convert a 2-D 'flat' matrix back to 3-D grid.
%   vxDevectorize42        - vxDevectorize42 - convert a 2-D 'flat' matrix back to 4-D grid.
%   vxDevectorize42rs      - vxDevectorize42rs - convert a 2-D 'flat' matrix back to 4-D grid.
%   vxSeriesBlockCreate    - vxSeriesBlockCreate - Create a block-series vector of a specified pattern.
%   vxSeriesEventCreate    - vxSeriesEventCreate - Create a event-series vector of a specified pattern.
%   vxVectorize            - vxVectorize - convert a 3-D/4-D grid into a 2-D 'flat' matrix.
%   vxVectorize32          - vxVectorize32 - convert a 3-D grid into a 2-D 'flat' matrix.
%   vxVectorize32rs        - vxVectorize32rs - convert a 3-D grid into a 2-D 'flat' matrix.
%   vxVectorize42          - vxVectorize42 - convert a 4-D grid into a 2-D 'flat' matrix.
%   vxVectorize42rs        - vxVectorize42rs - convert a 4-D grid into a 2-D 'flat' matrix.
%   vxVectorizeMap         - vxVectorizeMap - create a translation map between 3-D/4-D and 2-D maps.
%   vxVectorizeMap32       - vxVectorizeMap32 - create a translation map between 3-D and 2-D maps.
%   vxVectorizeMap42       - vxVectorizeMap42 - create a translation map between 4-D and 2-D maps.
