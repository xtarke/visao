cmake_minimum_required(VERSION 2.8)
project( dlib_face )

# include(/usr/include/dlib/cmake_utils/release_build_by_default)
# include(/usr/include/dlib/cmake_utils/use_cpp_11.cmake)
include(/usr/include/dlib/cmake)

find_package( OpenCV REQUIRED )
# find_package( dlib REQUIRED )
add_executable( dlib_face ./src/face.cpp )
target_link_libraries( dlib_face ${OpenCV_LIBS} dlib ) 
