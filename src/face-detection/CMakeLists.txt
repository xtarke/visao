cmake_minimum_required(VERSION 2.8)
project( face )
find_package( OpenCV REQUIRED )
add_executable( face ./src/objectDetection2.cpp )
target_link_libraries( face ${OpenCV_LIBS} ) 
