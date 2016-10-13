cmake_minimum_required(VERSION 2.6)
project(inmoov)
find_package( OpenCV REQUIRED )

add_executable(inmoov main.cpp StereoCapture.cpp)

target_link_libraries( inmoov ${OpenCV_LIBS} ) 

install(TARGETS inmoov RUNTIME DESTINATION bin)
