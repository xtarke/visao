cmake_minimum_required(VERSION 2.8.11)
project(inmoov_qt)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

add_compile_options(-std=c++11)

# Find the QtWidgets library
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
# find_package(QtSerialPort REQUIRED)
# find_package(OpenCV REQUIRED)

find_package(OpenCV REQUIRED PATHS "/opt/opencv3/")

set(inmoov_qt_SRC
  src/main.cpp
  src/inmoov_qt.cpp
  src/StereoCapture.cpp
  src/StereoVision.cpp
  src/FaceDetection.cpp
  src/serialsettingsdialog.cpp
  src/Communication.cpp
  src/remotecontrolwindow.cpp
  src/robot/Head.cpp
  src/SensorTread.cpp
  src/HeadTracking.cpp
  src/SingleCapture.cpp
)

# Create code from a list of Qt designer ui files.
#set(CMAKE_AUTOUIC ON) # use this if you have CMake 3.x instead of the following
qt5_wrap_ui(inmoov_qt_SRC src/inmoov_qt.ui src/serialsettingsdialog.ui src/remotecontrolwindow.ui)

# Tell CMake to create the helloworld executable
add_executable(inmoov_qt ${inmoov_qt_SRC})

# Use the Widgets module from Qt 5.
target_link_libraries(inmoov_qt ${OpenCV_LIBS} Qt5::Widgets Qt5Test)

# Install the executable
install(TARGETS inmoov_qt DESTINATION bin)

# Add QT modules
qt5_use_modules(inmoov_qt Core SerialPort)
