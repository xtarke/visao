img = imread('bird.jpg');

% Convert the image to L*a*b* color space using makecform and applycform.
cform = makecform('srgb2lab');
lab_img = applycform(img,cform);


ab = double(lab_img(:,:,2:3));
nrows = size(ab,1);
ncols = size(ab,2);
ab = reshape(ab,nrows*ncols,2);

nColors = 7;
% repeat the clustering 3 times to avoid local minima
[cluster_idx, cluster_center] = kmeans(ab,nColors,'distance','sqEuclidean', ...
                                      'Replicates',3);
      
                                  
pixel_labels = reshape(cluster_idx,nrows,ncols);
imshow(pixel_labels,[]), title('image labeled by cluster index');

segmented_images = cell(1,3);
rgb_label = repmat(pixel_labels,[1 1 3]);

for k = 1:nColors
    color = img;
    color(rgb_label ~= k) = 0;
    segmented_images{k} = color;
end


for k = 1:nColors    
    figure(k)
    imshow(segmented_images{k});
    hold on;
end

