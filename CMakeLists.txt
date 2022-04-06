cmake_minimum_required(VERSION 3.13)

project(capstone)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")

#opencv stuff
find_package(OpenCV 4.1 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})
add_definitions(${OpenCV_DEFINITIONS})

#boost stuff
find_package( Boost COMPONENTS system filesystem REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )
link_directories(${Boost_LIBRARY_DIR})

# Find all executables
file(GLOB project_SRCS src/*.cpp) #src/*.h

# Add project executable
add_executable(capstone ${project_SRCS})
target_link_libraries(capstone ${OpenCV_LIBRARIES} ${Boost_LIBRARIES})