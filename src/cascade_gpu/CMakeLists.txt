cmake_minimum_required(VERSION 2.8)
project( cascade )
find_package( OpenCV REQUIRED )
add_executable( cascade ./cascadeclassifier.cpp )
target_link_libraries( cascade ${OpenCV_LIBS} ) 
