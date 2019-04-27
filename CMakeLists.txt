cmake_minimum_required(VERSION 3.12)
project(Mana VERSION 0.1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_DEBUG_POSTFIX d)

if (WIN32)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Xclang -fopenmp")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xclang -fopenmp")
endif (WIN32)

# NOTE: Something like this might be needed for OpenMP on Linux/Mac
if(APPLE)
        find_package(OpenMP)
        if (OPENMP_FOUND)
                set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
                set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        endif()
endif()

if(UNIX AND NOT APPLE)
        # for Linux, BSD, Solaris, Minix
        find_package(OpenMP)
        if (OPENMP_FOUND)
                set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
                set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        endif()
endif()

file(GLOB_RECURSE mana_SRC
        include/*.h
        src/*.c
        include/*.hpp
        src/*.cpp)

add_library(Mana STATIC ${mana_SRC})

set(includePath ${CMAKE_CURRENT_SOURCE_DIR}/../../)

#set_target_properties(Mana
#  PROPERTIES
#  ARCHIVE_OUTPUT_DIRECTORY ${includePath}grindstone/build/buildgrindstone
#  LIBRARY_OUTPUT_DIRECTORY ${includePath}grindstone/build/buildgrindstone
#  RUNTIME_OUTPUT_DIRECTORY ${includePath}grindstone/build/buildgrindstone
#)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -Wno-pragma-pack")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w -Wno-pragma-pack")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${includePath}glfw-3.2.1 buildglfw)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(${includePath}glew-2.1.0/build/cmake buildglew)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(${includePath}assimp-4.1.0 buildassimp)

set(LIBNOISE_VERSION "1.0.0-cmake" )
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
set(BUILD_LIBNOISE_DOCUMENTATION OFF CACHE BOOL "" FORCE)
set(BUILD_LIBNOISE_UTILS ON CACHE BOOL "" FORCE)
set(BUILD_LIBNOISE_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${includePath}libnoise-master/src buildlibnoise)

#string(REPLACE " -w -Wno-pragma-pack" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
#string(REPLACE " -w -Wno-pragma-pack" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
#string(REPLACE " -w" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
#string(REPLACE " -w" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
#
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-pragma-pack")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-pragma-pack")

SET(includeList
        ${includePath}grindstone/Mana/include
        ${includePath}grindstone/Chaos/include
        ${includePath}glew-2.1.0/include
        ${includePath}glm-0.9.9.3
        ${includePath}glfw-3.2.1/include
        ${includePath}VulkanSDK/1.1.101.0/Include
        ${includePath}assimp-4.1.0/include
        ${includePath}libnoise-master/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../build/buildmana/buildassimp/include)

target_include_directories(Mana PUBLIC ${includeList})
