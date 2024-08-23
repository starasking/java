cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_STANDARD 17)
project(graph)


file(GLOB SOURCES src/*.cpp)

set(EIGEN_ROOT_DIR "./eigen-3.4/")
find_package(Armadillo REQUIRED)

include_directories(
    ${ARMADILLO_INCLUDE_DIRS}
)

include_directories(
    ${EIGEN_ROOT_DIR}
)
find_package(Matplot++ REQUIRED)
#add_subdirectory(matplotplusplus)

if (BUILD_TESTS)
    add_definitions("-DTESTING")
    add_subdirectory(tests)
endif ()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
add_executable(${PROJECT_NAME} ${SOURCES})
#target_link_libraries(${PROJECT_NAME} PUBLIC matplot)
target_link_libraries(${PROJECT_NAME} Matplot++::matplot)
