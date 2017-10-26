cmake_minimum_required(VERSION 3.0)
project(encoding_detection)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")

if(NOT WIN32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wno-long-long -Wconversion")
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)

set(EXECUTABLE ${PROJECT_NAME})

add_executable(${EXECUTABLE}
    main.cpp)

add_custom_target(run
    COMMAND ${EXECUTABLE}
    DEPENDS ${EXECUTABLE}
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})
