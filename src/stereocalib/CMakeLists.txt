cmake_minimum_required(VERSION 2.6)
project(stereocalib)
find_package( OpenCV REQUIRED )


add_executable(stereocalib main.cpp)

target_link_libraries( stereocalib ${OpenCV_LIBS} ) 

install(TARGETS stereocalib RUNTIME DESTINATION bin)
