cmake_minimum_required(VERSION 3.26)
project(VGP240_External)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 13.0)
endif()

add_subdirectory(Pix)
add_subdirectory(X)