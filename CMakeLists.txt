#######################################################################
#
# THIS FILE MAKES THE COMPILATION OF CODE DEVELOPED IN THIS SEMESTER AUTOMATIC.
# BY jesseh.barreto@gmail.com
#
#
# TO USE JUST ADD A NEW SUB_DIRECTORY WITH A CMAKELIST.TXT INSIDE.
# WITH:
#       add_executable(bin_name source_code)
#       target_link_libraries(bin_name libraries)
#
#######################################################################

cmake_minimum_required(VERSION 2.8)
project(TrafficSurveillanceSystem)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

## Find OpenCV
find_package(OpenCV REQUIRED)
set_target_properties(${OpenCV_LIBS} PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)
include_directories(SYSTEM "${OpenCV_INCLUDE_DIRS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -std=c++0x -pg")
set(CMAKE_EXE_LINKER_FLAGS="-pg")
set(CMAKE_SHARED_LINKER_FLAGS="-pg")
set(CMAKE_CXX_STANDARD 11)

# Add BGS Library
file(GLOB_RECURSE bgs_src package_bgs/*.cpp package_bgs/*.c)
file(GLOB_RECURSE bgs_inc package_bgs/*.h)
file(GLOB_RECURSE tracking_src package_tracking/*.cpp package_tracking/*.c)
file(GLOB_RECURSE tracking_inc package_tracking/*.h)
file(GLOB_RECURSE analysis_src package_analysis/*.cpp)
file(GLOB_RECURSE analysis_inc package_analysis/*.h)

## Copy data files from source folder to binary folder
file(COPY data DESTINATION .)

## Compile the BGS Library as Static
add_library(simple_vehicle_counting STATIC ${bgs_src} ${tracking_src} ${analysis_src})
target_link_libraries(simple_vehicle_counting ${OpenCV_LIBS})
set_property(TARGET simple_vehicle_counting PROPERTY PUBLIC_HEADER ${bgs_inc} ${tracking_inc} ${analysis_inc})

## Compile Executable
add_executable(	TrafficSurveillance #Executable Binary
                main.cpp #Source List
                imageline.cpp
                backgroundsubtractor.cpp
                camshifttracker.cpp
                car.cpp
                carkf.cpp
                carcs.cpp
                trafficsurveillancecommon.cpp
                trafficdebug.cpp
                trafficvideo.cpp
                trafficimagedataset.cpp
                flowdrawer.cpp
                motionsegmentationdetectorhs.cpp
                bgmog2detector.cpp
                bgmogdetector.cpp
                kalmanfiltertracker.cpp
                simpletracker.cpp
                virtualdetection.h #Includes
                virtualtrack.h
                )
target_link_libraries(TrafficSurveillance ${OpenCV_LIBS} simple_vehicle_counting)
