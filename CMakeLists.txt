cmake_minimum_required(VERSION 3.2)

# set the project name
SET(PROJECT_NAME document_scanner)
project(${PROJECT_NAME})

add_definitions( -DTEST_DATA_PATH="${CMAKE_CURRENT_SOURCE_DIR}/test_data")

# set open cv directory
SET(OpenCV_DIR "put your opencv lib dir path")

# add the executable
add_executable(${PROJECT_NAME} src/doc_s.cpp)


# Common Instructions for all Users
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} stdc++fs)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
