% Load VLFeat toolbox
vlfeat_ver =  exist(vl_version);

if (vlfeat_ver == 0)
    run('../../vlfeat-0.9.20/toolbox/vl_setup');
end

% load images
I = imread('duke390-2.jpg');
J = imread('roda.png');

I = single(rgb2gray(I)); % Conversion to single is recommended
J = single(rgb2gray(J)); % in the documentation

% create SIFT ids for two images
[F1 D1] = vl_sift(I);
[F2 D2] = vl_sift(J);

% Where 2 = ratio between euclidean distance of NN2/NN1
[matches score] = vl_ubcmatch(D1,D2,2); 

% plot
subplot(1,2,1);
imshow(uint8(I));
hold on;
plot(F1(1,matches(1,:)),F1(2,matches(1,:)),'b*');

subplot(1,2,2);
imshow(uint8(J));
hold on;
plot(F2(1,matches(2,:)),F2(2,matches(2,:)),'r*');
