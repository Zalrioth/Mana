cmake_minimum_required(VERSION 3.12)
project(Mana VERSION 0.1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_DEBUG_POSTFIX d)

file(GLOB_RECURSE mana_SRC
        include/*.h
        src/*.c
        include/*.hpp
        src/*.cpp)
        
add_library(Mana SHARED ${mana_SRC})

set_target_properties(Mana
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

if (WIN32)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Xclang -fopenmp")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xclang -fopenmp")
endif (WIN32)

if(APPLE)
        add_subdirectory(lib/MoltenVK buildmoltenvk)
        find_package(OpenMP)
        if (OPENMP_FOUND)
                set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        endif()
endif()

if(UNIX AND NOT APPLE)
        # for Linux, BSD, Solaris, Minix
        find_package(OpenMP)
        if (OPENMP_FOUND)
                set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        endif()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")

set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/glfw buildglfw)

set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/assimp buildassimp)

string(REPLACE " -w" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REPLACE " -w" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-pragma-pack")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-pragma-pack")

#TODO: Link properly on Apple and Linux
target_link_libraries(Mana
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/VulkanSDK/1.1.101.0/Lib/vulkan-1.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/openmp/libomp.lib
        glfw
        assimp)

SET(includeList
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/cglm/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/VulkanSDK/1.1.101.0/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/Chaos/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../build/buildmana/buildassimp/include
        ${OpenMP_CXX_LIBRARIES})

target_include_directories(Mana PUBLIC ${includeList})
