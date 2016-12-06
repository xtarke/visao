cmake_minimum_required(VERSION 2.6)
project(stereocapture)
find_package( OpenCV REQUIRED )

add_executable(stereocapture main.cpp)

target_link_libraries( stereocapture ${OpenCV_LIBS} ) 

install(TARGETS stereocapture RUNTIME DESTINATION bin)
