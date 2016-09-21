%%% Necessário revisar esse ponto:
%% Load the stereoParameters object.
%load('calib_01_Acq00.m');

% Visualize camera extrinsics.
%showExtrinsics(stereoParams);

%videoFileLeft = 'handshake_left.avi';
%videoFileRight = 'handshake_right.avi';

%%vidDir = videoinput('linuxvideo', 2, 'BGR24_640x480');
%%vidEsq = videoinput('linuxvideo', 1, 'BGR24_640x480');
%src = getselectedsource(vidDir);

%%vidDir.FramesPerTrigger = 20;
%%vidDir.ReturnedColorspace = 'grayscale';

%%vidEsq.FramesPerTrigger = 20;
%%vidEsq.ReturnedColorspace = 'grayscale';

%%triggerconfig(vidDir, 'manual');
%%triggerconfig(vidEsq, 'manual');

% clear classes;

%% Instantiate video device, face detector, and KLT object tracker
% vidObj = webcam;

%start(vidDir);
%start(vidEsq);

%% Get a frame for frame-size information
%frame = getsnapshot(vidDir);
%frameSize = size(frame);

% Coloquei uma pausa para a camera "estabilizar" - não sei se é realmente
% necessário
pause(2);

%readerLeft = vision.VideoPlayer('Position',[200 100 fliplr(frameSize(1:2)+30)]);
%readerRight = vision.VideoPlayer('Position',[200 100 fliplr(frameSize(1:2)+30)]);

%frameRight = getsnapshot(vidDir);
frameRight = imread('testEsq0');
frameLeft = imread('testDir0');
%frameLeft = getsnapshot(vidEsq);

[frameLeftRect, frameRightRect] = rectifyStereoImages(frameLeft, frameRight, stereoParams);

% figure;
 imshow(stereoAnaglyph(frameLeftRect, frameRightRect));
 title('Rectified Video Frames');



frameLeftGray  = rgb2gray(frameLeftRect);
frameRightGray = rgb2gray(frameRightRect);

disparityMap = disparity(frameLeftGray, frameRightGray);
figure;
imshow(disparityMap, [0, 64]);
title('Disparity Map');
colormap jet
colorbar



points3D = reconstructScene(disparityMap, stereoParams);

% Convert to meters and create a pointCloud object
points3D = points3D ./ 1000;
ptCloud = pointCloud(points3D, 'color', frameLeftRect);

% Create a streaming point cloud viewer
%player3D = pcplayer([-3, 3], [-3, 3], [0, 8], 'VerticalAxis', 'y', ...
%    'VerticalAxisDir', 'down');

% Visualize the point cloud
%view(player3D, ptCloud);


% Create the people detector object. Limit the minimum object size for
% speed.
peopleDetector = vision.PeopleDetector();

% Detect people.
bboxes = peopleDetector.step(frameLeftGray);


% Find the centroids of detected people.
centroids = [round(bboxes(:, 1) + bboxes(:, 3) / 2), ...
    round(bboxes(:, 2) + bboxes(:, 4) / 2)];

% Find the 3-D world coordinates of the centroids.
centroidsIdx = sub2ind(size(disparityMap), centroids(:, 2), centroids(:, 1));
X = points3D(:, :, 1);
Y = points3D(:, :, 2);
Z = points3D(:, :, 3);
centroids3D = [X(centroidsIdx)'; Y(centroidsIdx)'; Z(centroidsIdx)'];

% Find the distances from the camera in meters.
dists = sqrt(sum(centroids3D .^ 2));

% Display the detected people and their distances.
labels = cell(1, numel(dists));
for i = 1:numel(dists)
    labels{i} = sprintf('%0.2f meters', dists(i));
end
figure;
imshow(insertObjectAnnotation(frameLeftRect, 'rectangle', bboxes, labels));
title('Detected People');


%stop(vidDir);
%stop(vidEsq);
