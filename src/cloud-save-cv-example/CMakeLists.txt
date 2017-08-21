cmake_minimum_required(VERSION 2.6)
project(match)
find_package( OpenCV REQUIRED )

add_executable(match stereo_match.cpp)

target_link_libraries( match ${OpenCV_LIBS} ) 

install(TARGETS match RUNTIME DESTINATION bin)
