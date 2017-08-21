% Load VLFeat toolbox
vlfeat_ver =  exist(vl_version);

if (vlfeat_ver == 0)
    run('../../vlfeat-0.9.20/toolbox/vl_setup');
end

base_image = imread('dog.png');
feature = imread('2.png');

% to show an image
% image(I);

% convert images to gray scale and single precision
base_image_gray = im2single(rgb2gray(base_image));
base_feature_gray = im2single(rgb2gray(feature));

% create SIFT ids for two images
[fa, da] = vl_sift(base_image_gray);
[fb, db] = vl_sift(base_feature_gray);

% compare them using using Euclidian distance
[matches, scores] = vl_ubcmatch(da, db);

% genera images
[drop, perm] = sort(scores, 'descend') ;
matches = matches(:, perm) ;
scores  = scores(perm) ;

% figure(1) ; clf ;
% imagesc(cat(2, base_image, feature)) ;
% axis image off ;

figure(2) ; clf ;
imagesc(cat(2, base_image, feature)) ;

xa = fa(1,matches(1,:)) ;
xb = fb(1,matches(2,:)) + size(feature,2) ;
ya = fa(2,matches(1,:)) ;
yb = fb(2,matches(2,:)) ;

hold on ;
h = line([xa ; xb], [ya ; yb]) ;
set(h,'linewidth', 1, 'color', 'b') ;

vl_plotframe(fa(:,matches(1,:))) ;
fb(1,:) = fb(1,:) + size(base_image,2) ;
vl_plotframe(fb(:,matches(2,:))) ;
axis image off ;