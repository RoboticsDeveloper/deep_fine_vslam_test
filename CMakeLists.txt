cmake_minimum_required(VERSION 3.5)
project(glim_extension_test)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake")

cmake_policy(SET CMP0072 NEW)
set(OpenGL_GL_PREFERENCE GLVND)

#################
## ROS-related ##
#################
if(DEFINED ENV{ROS_VERSION})
  if($ENV{ROS_VERSION} EQUAL 2)
    find_package(ament_cmake REQUIRED)
  elseif($ENV{ROS_VERSION} EQUAL 1)
    find_package(catkin REQUIRED)
  else()
    message(FATAL_ERROR "Unsupported ROS version")
  endif()
endif()

find_package(ament_cmake REQUIRED)
find_package(glim REQUIRED)
find_package(GTSAM REQUIRED)
find_package(Pangolin REQUIRED)
find_package(OpenCV REQUIRED)
find_package(ORB_SLAM3 REQUIRED)

include_directories(
  include
  ${Pangolin_INCLUDE_DIRS}
  ${ORB_SLAM3_ROOT_DIR}/Thirdparty/Sophus
  ${EIGEN3_INCLUDE_DIR}
  ${ORB_SLAM3_INCLUDE_DIRS}  
  ${ORB_SLAM3_ROOT_DIR}/include/CameraModels
)

# Add this section to configure RPATH
set(CMAKE_INSTALL_RPATH "${ORB_SLAM3_ROOT_DIR}/lib;${ORB_SLAM3_ROOT_DIR}/Thirdparty/DBoW2/lib;${ORB_SLAM3_ROOT_DIR}/Thirdparty/g2o/lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Ensure the libraries are found at runtime
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_RPATH "${ORB_SLAM3_ROOT_DIR}/lib;${ORB_SLAM3_ROOT_DIR}/Thirdparty/DBoW2/lib;${ORB_SLAM3_ROOT_DIR}/Thirdparty/g2o/lib")

# Library: glim_extension_test
add_library(glim_extension_test SHARED
  src/glim_extension_test/util/config_ext.cpp
)
target_link_libraries(glim_extension_test
  glim::glim
  ${ORB_SLAM3_LIBRARIES}  
  ${Pangolin_LIBRARIES}
  ${OpenCV_LIBS}
  ${ORB_SLAM3_ROOT_DIR}/Thirdparty/DBoW2/lib/libDBoW2.so
  ${ORB_SLAM3_ROOT_DIR}/Thirdparty/g2o/lib/libg2o.so
)
target_compile_options(glim_extension_test PRIVATE -std=c++17)

# Library: glim_callback_demo
add_library(glim_callback_demo SHARED
  src/glim_extension_test/glim_callback_demo/glim_callback_demo.cpp
)
target_link_libraries(glim_callback_demo glim::glim)

# Executable: orb_slam_test
add_executable(orb_slam_test
  src/glim_extension_test/orb_slam/orb_slam_test.cpp
)
target_link_libraries(orb_slam_test
  glim_extension_test
  glim_callback_demo
)

#############
## Install ##
#############
include(GNUInstallDirs)

install(TARGETS glim_extension_test glim_callback_demo orb_slam_test
  LIBRARY DESTINATION lib/${PROJECT_NAME}
  ARCHIVE DESTINATION lib/${PROJECT_NAME}
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

if(DEFINED ENV{ROS_VERSION})
  if($ENV{ROS_VERSION} EQUAL 2)
    install(DIRECTORY config DESTINATION share/glim_extension_test)
    ament_target_dependencies(glim_extension_test glim OpenCV ORB_SLAM3)
    ament_target_dependencies(orb_slam_test glim OpenCV ORB_SLAM3)
    ament_export_libraries(glim_extension_test glim_callback_demo)
    ament_package()
  elseif($ENV{ROS_VERSION} EQUAL 1)
    catkin_package(
      LIBRARIES glim_extension_test glim_callback_demo
    )
  endif()
endif()
