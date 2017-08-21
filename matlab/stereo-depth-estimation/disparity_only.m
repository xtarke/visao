%% Programa para tentar estimar distância até rolo circular preso no quadro
% Junta os programas 'example_00.m' que estima distancia de uma pessoa e
% 'testeCirculoQuadro.m' que encontra um circulo em uma imagem

% Load the stereoParameters object.
% load('calib_01_Acq02.mat');
% load('calib_01_Acq02.mat');

% Visualize camera extrinsics.
% showExtrinsics(stereoParams);

% Começa carregando as imagens
frameRight = imread('left_00000.png');
frameLeft = imread('right_00000.png');

% Retifica as imagens e mostra para estimar visualmente a qualidade do resultado
[frameLeftRect, frameRightRect] = rectifyStereoImages(frameLeft, frameRight, stereoParams);
imshow(stereoAnaglyph(frameLeftRect, frameRightRect));
title('Rectified Video Frames');

% Faz o mapa de disparidade
frameLeftGray  = rgb2gray(frameLeftRect);
frameRightGray = rgb2gray(frameRightRect);
disparityMap = disparity(frameLeftGray, frameRightGray);

imshow(disparityMap);