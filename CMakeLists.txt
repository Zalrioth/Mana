cmake_minimum_required(VERSION 3.12)
project(Mana VERSION 0.1.0)

#https://stackoverflow.com/questions/3201154/automatically-add-all-files-in-a-folder-to-a-target-using-cmake
#https://bytefreaks.net/programming-2/manually-set-the-cmake-output-folder

set(CMAKE_CXX_STANDARD 14)

file(GLOB_RECURSE mana_SRC
        include/*.h
        src/*.c
        include/*.hpp
        src/*.cpp)

add_library(Mana SHARED ${mana_SRC})

set(includePath ${CMAKE_CURRENT_SOURCE_DIR}/../../)
set_target_properties(Mana
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${includePath}grindstone/build/buildgrindstone
  LIBRARY_OUTPUT_DIRECTORY ${includePath}grindstone/build/buildgrindstone
  RUNTIME_OUTPUT_DIRECTORY ${includePath}grindstone/build/buildgrindstone
)

find_package(OpenGL REQUIRED)

set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${includePath}glfw-3.2.1 buildglfw)

set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
add_subdirectory(${includePath}glew-2.1.0/build/cmake buildglew)

set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(${includePath}assimp-4.1.0 buildassimp)

target_link_libraries(Mana ${includePath}cpython-3.7.2/PCbuild/amd64/python37_d.lib)
target_link_libraries(Mana OpenGL32)
target_link_libraries(Mana glfw)
target_link_libraries(Mana glew)
target_link_libraries(Mana assimp)

SET(includeList
        ${includePath}grindstone/Mana/include
        ${includePath}glfw-3.2.1/include
        ${includePath}glew-2.1.0/include
        ${includePath}assimp-4.1.0/include
        ${includePath}cglm/include
        ${includePath}cpython-3.7.2/PC
        ${includePath}cpython-3.7.2/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/../build/buildmana/buildassimp/include)
        #${includePath}libnoise-master/src)

target_include_directories(Mana PUBLIC ${includeList})