I = imread('dog.png');
imshow(I)
I = rgb2gray(I);
imshow(I)

BW1 = edge(I,'sobel');      % Edge detectors: only gray-scale images
BW2 = edge(I,'canny');

figure;
imshowpair(BW1,BW2,'montage')
title('Sobel Filter                                   Canny Filter');
