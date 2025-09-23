
cmake_minimum_required(VERSION 2.8.3)
project(lyec_camera)
add_definitions("-std=gnu++11")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(arena_sdk QUIET)
if (NOT ${arena_sdk_FOUND})
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Findarena_sdk.cmake")
endif()

#
# CATKIN
#

set(CATKIN_COMPONENTS
     actionlib
     camera_control_msgs 
     camera_info_manager
     cv_bridge
     diagnostic_updater
     image_geometry
     image_transport
     roscpp
     roslint
     roslaunch 
     sensor_msgs
)

find_package(
    catkin 
        REQUIRED
    COMPONENTS 
        ${CATKIN_COMPONENTS}
)
##
find_package(OpenCV REQUIRED)
if (OpenCV_FOUND)
    message(STATUS "OpenCV found")
    message(STATUS "Include dirs: ${OpenCV_INCLUDE_DIRS}")
    message(STATUS "Libraries: ${OpenCV_LIBRARIES}")
else()
    message(FATAL_ERROR "OpenCV not found")
endif()
catkin_python_setup()



catkin_package(
    INCLUDE_DIRS
        include
    LIBRARIES
        ${PROJECT_NAME}
    CATKIN_DEPENDS
        ${CATKIN_COMPONENTS} # package dep for build/run
    # DEPENDS # sys dep for build/run 
    #    <> 
    #    <> 
    
)


set(ROSLINT_CPP_OPTS
   "--extensions=cpp,h,hpp" "--filter=-runtime/references,-readability/todo,-build/include_what_you_use"
)

roslint_cpp( 
)

# roslaunch_add_file_check(launch)


set(arena_camera_node_name arena_camera_node)

add_executable(
    ${arena_camera_node_name} 
        src/main.cpp

)
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  src/lyec-camera-driver/include/lyec-var.h
)
link_directories(
	include
  ${OpenCV_LIBRARY_DIRS}
)
target_include_directories(
    ${arena_camera_node_name}
        PRIVATE ${OpenCV_LIBRARIES}
        PRIVATE ${catkin_INCLUDE_DIRS} 
        PRIVATE ${arena_sdk_INCLUDE_DIRS}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    ${arena_camera_node_name}
        ${catkin_LIBRARIES}
        ${arena_sdk_LIBRARIES}
        ${OpenCV_LIBRARIES}
        yaml-cpp
)

add_dependencies(
    ${arena_camera_node_name}
        ${catkin_EXPORTED_TARGETS} 
)






# add_executable(my_executable hdr_test/hdr.cpp)
# add_dependencies(my_executable ${catkin_EXPORTED_TARGETS} )
# target_include_directories(my_executable PRIVATE ${OpenCV_INCLUDE_DIRS})
# target_link_libraries(my_executable ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})


install(
    TARGETS
        ${arena_camera_node_name}
    LIBRARY DESTINATION
        ${CATKIN_PACKAGE_LIB_DESTINATION}
    RUNTIME DESTINATION
        ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(
    DIRECTORY
        include/${PROJECT_NAME}/
    DESTINATION
        ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
    FILES_MATCHING PATTERN
        "*.h"
    PATTERN "internal" EXCLUDE
)

file(GLOB children ${CMAKE_CURRENT_SOURCE_DIR}/*)
foreach(child ${children})
  if(IS_DIRECTORY ${child})
    file(GLOB_RECURSE dir_files "${child}/*")
    list(APPEND ${PROJECT_NAME}_extra_files ${dir_files})
  endif()
endforeach()
