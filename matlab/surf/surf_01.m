
% Load image
objectImage = (rgb2gray(imread('teclado.png')));
% objectImage = imrotate(objectImage,90);

% figure;
% imshow(objectImage);
% title('Image of a Object');

% Load scene
sceneImage = (rgb2gray(imread('webcam.jpg')));
% figure;
% imshow(sceneImage);
% title('Image of a Cluttered Scene');

% Detect feature points in both images
ObjectPoints = detectSURFFeatures(objectImage,'NumOctaves',4);
scenePoints = detectSURFFeatures(sceneImage,'NumOctaves',4);

% Shows the strongest feature points found in the reference image.
figure;
imshow(objectImage);
title('100 Strongest Feature Points from Box Image');
hold on;
plot(selectStrongest(ObjectPoints, 1000));

% Visualize the strongest feature points found in the target image
figure;
imshow(sceneImage);
title('300 Strongest Feature Points from Scene Image');
hold on;
plot(selectStrongest(scenePoints, 1000));

% Extract feature descriptors at the interest points in both images
[ObjectFeatures, ObjectPoints] = extractFeatures(objectImage, ObjectPoints);
[sceneFeatures, scenePoints] = extractFeatures(sceneImage, scenePoints);

% Match the features using their descriptors
ObjectPairs = matchFeatures(ObjectFeatures, sceneFeatures);

% Display putatively matched features
matchedObjectPoints = ObjectPoints(ObjectPairs(:, 1), :);
matchedScenePoints = scenePoints(ObjectPairs(:, 2), :);
figure;
showMatchedFeatures(objectImage, sceneImage, matchedObjectPoints, ...
    matchedScenePoints, 'montage');
title('Putatively Matched Points (Including Outliers)');


% estimateGeometricTransform calculates the transformation relating the 
% matched points, while eliminating outliers. This transformation allows 
% us to localize the object in the scene.

[tform, inlierObjectPoints, inlierScenePoints, status] = ...
    estimateGeometricTransform(matchedObjectPoints, matchedScenePoints, 'affine');

disp(status);


% Display the matching point pairs with the outliers removed
figure;
showMatchedFeatures(objectImage, sceneImage, inlierObjectPoints, ...
    inlierScenePoints, 'montage');
title('Matched Points (Inliers Only)');

% Get the bounding polygon of the reference image.
ObjectPolygon = [1, 1;...                           % top-left
        size(objectImage, 2), 1;...                 % top-right
        size(objectImage, 2), size(objectImage, 1);... % bottom-right
        1, size(objectImage, 1);...                 % bottom-left
        1, 1];                   % top-left again to close the polygon

% Transform the polygon into the coordinate system of the target image. 
% The transformed polygon indicates the location of the object in the scene.

newObjectPolygon = transformPointsForward(tform, ObjectPolygon);

% Display the detected object.
figure;
imshow(sceneImage);
hold on;
line(newObjectPolygon(:, 1), newObjectPolygon(:, 2), 'Color', 'r');
title('Detected Box');

