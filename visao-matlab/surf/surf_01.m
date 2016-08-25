
% Load image
wheelImage = (rgb2gray(imread('roda.png')));
% wheelImage = imrotate(wheelImage,20);

% figure;
% imshow(wheelImage);
% title('Image of a Wheel');

% Load scene
sceneImage = (rgb2gray(imread('duke390-2.jpg')));
% figure;
% imshow(sceneImage);
% title('Image of a Cluttered Scene');

% Detect feature points in both images
wheelPoints = detectSURFFeatures(wheelImage);
scenePoints = detectSURFFeatures(sceneImage);

% Shows the strongest feature points found in the reference image.
figure;
imshow(wheelImage);
title('100 Strongest Feature Points from Box Image');
hold on;
plot(selectStrongest(wheelPoints, 100));

% Visualize the strongest feature points found in the target image
figure;
imshow(sceneImage);
title('300 Strongest Feature Points from Scene Image');
hold on;
plot(selectStrongest(scenePoints, 300));

% Extract feature descriptors at the interest points in both images
[wheelFeatures, wheelPoints] = extractFeatures(wheelImage, wheelPoints);
[sceneFeatures, scenePoints] = extractFeatures(sceneImage, scenePoints);

% Match the features using their descriptors
wheelPairs = matchFeatures(wheelFeatures, sceneFeatures);

% Display putatively matched features
matchedwheelPoints = wheelPoints(wheelPairs(:, 1), :);
matchedScenePoints = scenePoints(wheelPairs(:, 2), :);
figure;
showMatchedFeatures(wheelImage, sceneImage, matchedwheelPoints, ...
    matchedScenePoints, 'montage');
title('Putatively Matched Points (Including Outliers)');


% estimateGeometricTransform calculates the transformation relating the 
% matched points, while eliminating outliers. This transformation allows 
% us to localize the object in the scene.

[tform, inlierwheelPoints, inlierScenePoints] = ...
    estimateGeometricTransform(matchedwheelPoints, matchedScenePoints, 'affine');

% Display the matching point pairs with the outliers removed
figure;
showMatchedFeatures(wheelImage, sceneImage, inlierwheelPoints, ...
    inlierScenePoints, 'montage');
title('Matched Points (Inliers Only)');

% Get the bounding polygon of the reference image.
wheelPolygon = [1, 1;...                           % top-left
        size(wheelImage, 2), 1;...                 % top-right
        size(wheelImage, 2), size(wheelImage, 1);... % bottom-right
        1, size(wheelImage, 1);...                 % bottom-left
        1, 1];                   % top-left again to close the polygon

% Transform the polygon into the coordinate system of the target image. 
% The transformed polygon indicates the location of the object in the scene.

newwheelPolygon = transformPointsForward(tform, wheelPolygon);

% Display the detected object.
figure;
imshow(sceneImage);
hold on;
line(newwheelPolygon(:, 1), newwheelPolygon(:, 2), 'Color', 'r');
title('Detected Box');

