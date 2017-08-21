vidDir = videoinput('linuxvideo', 2, 'BGR24_640x480');
vidEsq = videoinput('linuxvideo', 1, 'BGR24_640x480');
%src = getselectedsource(vidDir);

vidDir.FramesPerTrigger = 20;
vidDir.ReturnedColorspace = 'rgb';

vidEsq.FramesPerTrigger = 20;
vidEsq.ReturnedColorspace = 'rgb';

triggerconfig(vidDir, 'manual');
triggerconfig(vidEsq, 'manual');

% clear classes;

%% Instantiate video device, face detector, and KLT object tracker
% vidObj = webcam;

start(vidDir);
start(vidEsq);

%% Get a frame for frame-size information
frame = getsnapshot(vidDir);
frameSize = size(frame);

% %% Create a video player instance
videoPlayerDir  = vision.VideoPlayer('Position',[200 100 fliplr(frameSize(1:2)+30)]);
videoPlayerEsq  = vision.VideoPlayer('Position',[200 100 fliplr(frameSize(1:2)+30)]);

count = 0;
imageName = 'test';
tic;
position = [10, 10];
while (count<30)
    frameDir = getsnapshot(vidDir);
    frameEsq = getsnapshot(vidEsq);
     
%     if count==0
%         imshow(frameDir);
%     end
    
    strCount = num2str(count);
    if toc > 5.0
        tic;
        imwrite(frameDir, strcat(imageName, 'Acq_Dir', strCount), 'png');
        imwrite(frameEsq, strcat(imageName, 'Acq_Esq', strCount), 'png');
        count = count + 1;
    end
    
    % Obter uma melhor forma de fazer o intervalo de aquisição
    %pause(2); 
    
    frameDir = insertText(frameDir, position, strCount);
    
    videoPlayerDir.step(frameDir);
    videoPlayerEsq.step(frameEsq);

end

stop(vidDir);
stop(vidEsq);
release(videoPlayerDir);
release(videoPlayerEsq);
