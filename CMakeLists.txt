cmake_minimum_required(VERSION 3.5)
project(DelaunayTriangulation3D)

set(CMAKE_CXX_STANDARD 11)

find_package(CGAL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

set(SOURCES main.cpp shader.h)

add_executable(triangulation ${SOURCES})
target_link_libraries(triangulation -lglfw -lGLEW -lGL -lCGAL -lgmp)
