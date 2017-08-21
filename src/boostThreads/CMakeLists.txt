cmake_minimum_required(VERSION 2.6)
project(boostthreads)

# set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

# Explicitly tell find-package to search for Static Boost libs (if needed)
find_package( Boost REQUIRED COMPONENTS thread )

include_directories( ${Boost_INCLUDE_DIRS} )

add_executable(boostthreads main.cpp)

target_link_libraries(boostthreads ${Boost_LIBRARIES})


install(TARGETS boostthreads RUNTIME DESTINATION bin)
