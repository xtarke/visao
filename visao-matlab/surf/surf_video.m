vid = videoinput('linuxvideo', 1, 'BGR24_640x360');
src = getselectedsource(vid);

vid.FramesPerTrigger = 1;
vid.ReturnedColorspace = 'grayscale';

triggerconfig(vid, 'manual');

% clear classes;

%% Instantiate video device, face detector, and KLT object tracker
% vidObj = webcam;

start(vid);

%% Get a frame for frame-size information
frame = getsnapshot(vid);
frameSize = size(frame);

% %% Create a video player instance
videoPlayer  = vision.VideoPlayer('Position',[200 100 fliplr(frameSize(1:2)+30)]);

detectionImage = (rgb2gray(imread('fone.png')));
detectionImagePoints = detectSURFFeatures(detectionImage);
[detectionImageFeatures, detectionImagePoints] = extractFeatures(detectionImage, detectionImagePoints);

count = 0;

while (count < 200)
    frame = getsnapshot(vid);
        
    scenePoints = detectSURFFeatures(frame);  
     
    [sceneFeatures, scenePoints] = extractFeatures(frame, scenePoints);    
    
    detectionImagePairs = matchFeatures(detectionImageFeatures, sceneFeatures);
    
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
            imshow(frame); hold on;
            plot(selectStrongest(scenePoints, 300)); hold on
            
        end
        
%         figure;
%         hold on
%         showMatchedFeatures(detectionImage, frame, detectionImagePairs, ...
%         matchedScenePoints, 'montage');
%         hold on
        
    %nd

    
  
   
    % videoPlayer.step(frame);

    count = count + 1;
end


stop(vid);
release(videoPlayer);

% %% Iterate until we have successfully detected a face
% bboxes = [];
% while isempty(bboxes)
%     framergb = snapshot(vidObj);
%     frame = rgb2gray(framergb);
%     bboxes = faceDetector.step(frame);
% end
% tracker.addDetections(frame, bboxes);
% 
% %% And loop until the player is closed
% frameNumber = 0;
% keepRunning = true;
% disp('Press Ctrl-C to exit...');
% while keepRunning
%     
%     framergb = snapshot(vidObj);
%     frame = rgb2gray(framergb);
%     
%     if mod(frameNumber, 10) == 0
%         % (Re)detect faces.
%         %
%         % NOTE: face detection is more expensive than imresize; we can
%         % speed up the implementation by reacquiring faces using a
%         % downsampled frame:
%         % bboxes = faceDetector.step(frame);
%         bboxes = 2 * faceDetector.step(imresize(frame, 0.5));
%         if ~isempty(bboxes)
%             tracker.addDetections(frame, bboxes);
%         end
%     else
%         % Track faces
%         tracker.track(frame);
%     end
%     
%     % Display bounding boxes and tracked points.
%     displayFrame = insertObjectAnnotation(framergb, 'rectangle',...
%         tracker.Bboxes, tracker.BoxIds);
%     displayFrame = insertMarker(displayFrame, tracker.Points);
%     videoPlayer.step(displayFrame);
%     
%     frameNumber = frameNumber + 1;
% end
% 
% %% Clean up
% release(videoPlayer);