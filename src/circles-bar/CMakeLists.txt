cmake_minimum_required(VERSION 2.8)
project( circles )
find_package( OpenCV REQUIRED )
add_executable( circles ./src/circles.cpp )
target_link_libraries( circles ${OpenCV_LIBS} ) 
