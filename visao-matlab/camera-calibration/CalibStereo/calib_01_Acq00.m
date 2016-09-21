% Auto-generated by stereoCalibrator app on 29-Aug-2016
%-------------------------------------------------------


% Define images to process
imageFileNames1 = {'/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Dir/testDir0',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Dir/testDir1',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Dir/testDir10',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Dir/testDir12',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Dir/testDir13',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Dir/testDir15',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Dir/testDir16',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Dir/testDir17',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Dir/testDir18',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Dir/testDir19',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Dir/testDir2',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Dir/testDir3',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Dir/testDir4',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Dir/testDir5',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Dir/testDir6',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Dir/testDir8',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Dir/testDir9',...
    };
imageFileNames2 = {'/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Esq/testEsq0',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Esq/testEsq1',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Esq/testEsq10',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Esq/testEsq12',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Esq/testEsq13',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Esq/testEsq15',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Esq/testEsq16',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Esq/testEsq17',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Esq/testEsq18',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Esq/testEsq19',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Esq/testEsq2',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Esq/testEsq3',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Esq/testEsq4',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Esq/testEsq5',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Esq/testEsq6',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Esq/testEsq8',...
    '/home/sergio/IFSC/projetos/visao/visao-matlab/camera-calibration/Acquisitions/Acq00/Esq/testEsq9',...
    };

% Detect checkerboards in images
[imagePoints, boardSize, imagesUsed] = detectCheckerboardPoints(imageFileNames1, imageFileNames2);

% Generate world coordinates of the checkerboard keypoints
squareSize = 26;  % in units of 'mm'
worldPoints = generateCheckerboardPoints(boardSize, squareSize);

% Calibrate the camera
[stereoParams, pairsUsed, estimationErrors] = estimateCameraParameters(imagePoints, worldPoints, ...
    'EstimateSkew', false, 'EstimateTangentialDistortion', false, ...
    'NumRadialDistortionCoefficients', 2, 'WorldUnits', 'mm');

% View reprojection errors
h1=figure; showReprojectionErrors(stereoParams, 'BarGraph');

% Visualize pattern locations
h2=figure; showExtrinsics(stereoParams, 'CameraCentric');

% Display parameter estimation errors
displayErrors(estimationErrors, stereoParams);

% You can use the calibration data to rectify stereo images.
I1 = imread(imageFileNames1{1});
I2 = imread(imageFileNames2{1});
[J1, J2] = rectifyStereoImages(I1, I2, stereoParams);

% See additional examples of how to use the calibration data.  At the prompt type:
% showdemo('StereoCalibrationAndSceneReconstructionExample')
