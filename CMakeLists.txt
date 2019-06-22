cmake_minimum_required(VERSION 3.12)
project(Mana VERSION 0.1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_DEBUG_POSTFIX d)

if (WIN32)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Xclang -fopenmp")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xclang -fopenmp")
endif (WIN32)

if(APPLE)
        add_subdirectory(lib/MoltenVK buildmoltenvk)
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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")

add_subdirectory(lib/openmp buildopenmp)

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
        lib/VulkanSDK/1.1.101.0/Lib/vulkan-1.lib
        openmp
        glfw
        assimp)

SET(includeList
        'include'
        lib/cglm/include
        lib/glfw/include
        lib/VulkanSDK/1.1.101.0/Include
        lib/Chaos/include
        lib/assimp/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../build/buildmana/buildassimp/include)

target_include_directories(Mana PUBLIC ${includeList})
