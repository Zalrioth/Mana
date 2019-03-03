cmake_minimum_required(VERSION 3.12)
project(Mana VERSION 0.1.0)

#https://stackoverflow.com/questions/3201154/automatically-add-all-files-in-a-folder-to-a-target-using-cmake
#https://bytefreaks.net/programming-2/manually-set-the-cmake-output-folder
#https://stackoverflow.com/questions/49672264/cmake-add-d-suffix-for-debug-build-of-static-library

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_DEBUG_POSTFIX "")

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

set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${includePath}glfw-3.2.1 buildglfw)

set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(${includePath}assimp-4.1.0 buildassimp)

target_link_libraries(Mana ${includePath}grindstone/build/buildmana/Manad.lib
        ${includePath}cpython-3.7.2/PCbuild/amd64/python37_d.lib
        ${includePath}VulkanSDK/1.1.85.0/Lib/vulkan-1.lib
        glfw
        assimp)

SET(includeList
        ${includePath}grindstone/Mana/include
        ${includePath}glfw-3.2.1/include
        ${includePath}VulkanSDK/1.1.85.0/Include
        ${includePath}assimp-4.1.0/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../build/buildmana/buildassimp/include
        ${includePath}cpython-3.7.2/PC
        ${includePath}cpython-3.7.2/Include)

target_include_directories(Mana PUBLIC ${includeList})