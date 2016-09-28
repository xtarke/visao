%% Programa para tentar estimar distância até rolo circular preso no quadro
% Junta os programas 'example_00.m' que estima distancia de uma pessoa e
% 'testeCirculoQuadro.m' que encontra um circulo em uma imagem

% Load the stereoParameters object.
%load('calib_01_Acq02.mat');
% load('calib_01_Acq02.mat');

% Visualize camera extrinsics.
% showExtrinsics(stereoParams);

% Começa carregando as imagens
frameRight = imread('Fame_r_00002.png');
frameLeft = imread('Frame_l_00002.png');

% Retifica as imagens e mostra para estimar visualmente a qualidade do resultado
[frameLeftRect, frameRightRect] = rectifyStereoImages(frameLeft, frameRight, stereoParams);
imshow(stereoAnaglyph(frameLeftRect, frameRightRect));
title('Rectified Video Frames');

% Faz o mapa de disparidade
frameLeftGray  = rgb2gray(frameLeftRect);
frameRightGray = rgb2gray(frameRightRect);
disparityMap = disparity(frameLeftGray, frameRightGray);

% Reconstroi a cena 3D utilizando o mapa de disparidade e parametros das
% cameras
points3D = reconstructScene(disparityMap, stereoParams);

% Convert to meters OBS: Não sei exatamente se essa é a relação correta de
% pixels para metros, nem sei como o exemplo chegou nesse valor.
% points3D = points3D ./ 1000;
points3D = points3D; % ./ 1000;

% Detecta os centros e raio do circulo na imagem da esquerda.
[centers, radii] = imfindcircles(frameLeftRect, [50, 60], 'ObjectPolarity', 'dark', 'Sensitivity', 0.9);
disp(centers);
disp(radii);
imshow(frameLeftRect);
d = imdistline
h = viscircles(centers,radii);


% Pega os 'centroids' o circulo
centroids = [round(centers(1)), round(centers(2))];

% A partir daqui a parte "nebulosa"
% Find the 3-D world coordinates of the centroids.
centroidsIdx = sub2ind(size(disparityMap), centroids(:, 1), centroids(:, 2));
X = points3D(:, :, 1);
Y = points3D(:, :, 2);
Z = points3D(:, :, 3);
centroids3D = [X(centroidsIdx)'; Y(centroidsIdx)'; Z(centroidsIdx)'];

% Find the distances from the camera in meters.
dists = sqrt(sum(centroids3D .^ 2))/1000;

% Display the detected people and their distances.
labels = cell(1, numel(dists));
for i = 1:numel(dists)
    labels{i} = sprintf('%0.2f meters', dists(i));
end
figure;
imshow(insertObjectAnnotation(frameLeftRect, 'circle', [centers, radii], labels));
title('Detected Circle');
