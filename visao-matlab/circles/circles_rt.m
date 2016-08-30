vid = videoinput('linuxvideo', 1, 'BGR24_640x480'); %Ps EYE
src = getselectedsource(vid);

vid.FramesPerTrigger = 1;
vid.ReturnedColorspace = 'grayscale';

triggerconfig(vid, 'manual');

start(vid);

%% Get a frame for frame-size information
frame = getsnapshot(vid);
frameSize = size(frame);



count = 0;

while (count < 1000)
    % rame = rgb2gray(imread('imagem.png'));
    
    frame = getsnapshot(vid);
        
    [centers, radii] = imfindcircles(frame,[20 25],'ObjectPolarity','dark', ...
    'Sensitivity',0.95);
            
    imshow(frame);  
    h = viscircles(centers,radii);
    drawnow;
   
     
    count = count + 1;
end

disp(centers);

stop(vid);

