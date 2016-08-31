vid = videoinput('linuxvideo', 1, 'BGR24_640x480');
src = getselectedsource(vid);

vid.FramesPerTrigger = 1;
vid.ReturnedColorspace = 'grayscale';

triggerconfig(vid, 'manual');

start(vid);

%% Get a frame for frame-size information
% frame = getsnapshot(vid);
% frameSize = size(frame);

% %% Create a video player instance
% videoPlayer  = vision.VideoPlayer('Position',[200 100 fliplr(frameSize(1:2)+30)]);

detectionImage = (rgb2gray(imread('teclado.png')));
% detectionImage = imrotate(detectionImage);

detectionImagePoints = detectSURFFeatures(detectionImage);
[detectionImageFeatures, detectionImagePoints] = extractFeatures(detectionImage, detectionImagePoints);

count = 0;

while (count < 200)
    frame = getsnapshot(vid);
        
    scenePoints = detectSURFFeatures(frame);  
     
    [sceneFeatures, scenePoints] = extractFeatures(frame, scenePoints);    
    
    detectionImagePairs = matchFeatures(detectionImageFeatures, sceneFeatures, 'MaxRatio', 0.9);
    
    matchedImagePoints = detectionImagePoints(detectionImagePairs(:, 1), :);
    matchedScenePoints = scenePoints(detectionImagePairs(:, 2), :);
    
    [tform, inlierImagePoints, inlierScenePoints, status] = ...
    estimateGeometricTransform(matchedImagePoints, matchedScenePoints, 'affine');


   % if (mod(count,10) == 0)    
        % imshow(frame); hold on;
        % plot(selectStrongest(scenePoints, 300)); hold on
        
        if (status == 0)
            disp('Ok');
            
%             Polygon = [1, 1;...                           % top-left
%             size(detectionImage, 2), 1;...                 % top-right
%             size(detectionImage, 2), size(detectionImage, 1);... % bottom-right
%             1, size(detectionImage, 1);...                 % bottom-left
%             1, 1];                          % top-left again to close the polygon
        
%             imagePolygon = transformPointsForward(tform, Polygon);
            
            % figure;
            % imshow(frame);
            % hold on;
            %line(imagePolygon(:, 1), imagePolygon(:, 2), 'Color', 'r');
            %hold on
%             imshow(frame); hold on;
%             plot(selectStrongest(scenePoints, 300)); hold on

            f = figure;
            hold on
            showMatchedFeatures(detectionImage, frame, detectionImagePairs, ...
            matchedScenePoints, 'montage');
            hold on
%             
%             uiwait(f);
            
            
            
        end
        

        
    %nd

    
  
   
    % videoPlayer.step(frame);

    count = count + 1;
end

stop(vid);
