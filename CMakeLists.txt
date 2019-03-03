cmake_minimum_required(VERSION 3.12)
project(Mana VERSION 0.1.0)

#https://stackoverflow.com/questions/3201154/automatically-add-all-files-in-a-folder-to-a-target-using-cmake
#https://bytefreaks.net/programming-2/manually-set-the-cmake-output-folder
#https://stackoverflow.com/questions/49672264/cmake-add-d-suffix-for-debug-build-of-static-library
#https://stackoverflow.com/questions/47303810/how-to-get-clang-with-openmp-working-on-msvc-2015?rq=1
#https://stackoverflow.com/questions/35064093/openmp-runs-single-threaded-on-my-mac
#https://stackoverflow.com/questions/46414660/macos-cmake-and-openmp
#https://stackoverflow.com/questions/12399422/how-to-set-linker-flags-for-openmp-in-cmakes-try-compile-function
#https://stackoverflow.com/questions/54749207/openmp-on-windows-with-clang-and-cmake
#https://stackoverflow.com/questions/37969440/clang-openmp-and-cmake
#https://stackoverflow.com/questions/9160335/os-specific-instructions-in-cmake-how-to

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_DEBUG_POSTFIX "")

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
        ${includePath}llvm/lib/libomp.lib
        glfw
        assimp)

SET(includeList
        ${includePath}grindstone/Mana/include
        ${includePath}glfw-3.2.1/include
        ${includePath}VulkanSDK/1.1.85.0/Include
        ${includePath}assimp-4.1.0/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../build/buildmana/buildassimp/include)

target_include_directories(Mana PUBLIC ${includeList})