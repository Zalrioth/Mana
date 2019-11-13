cmake_minimum_required(VERSION 3.12)
project(Mana LANGUAGES C VERSION 0.1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_DEBUG_POSTFIX "d")

# Might be needed for OpenMP on Mac/Liunux
#https://stackoverflow.com/questions/56202041/compiling-and-linking-against-openmp-with-appleclang-on-mac-os-x-mojave

file(GLOB_RECURSE mana_SRC
        include/*.h
        src/*.c)
        
add_library(mana STATIC ${mana_SRC})

set_target_properties(mana
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx2")

if (WIN32)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Xclang -fopenmp")
endif (WIN32)

if(APPLE)
        add_subdirectory(lib/MoltenVK buildmoltenvk)
        find_package(OpenMP)
        if (OPENMP_FOUND)
                set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        endif()
endif()

if(UNIX AND NOT APPLE)
        # for Linux, BSD, Solaris, Minix
        find_package(OpenMP)
        if (OPENMP_FOUND)
                set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        endif()
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/glfw buildglfw)

string(REPLACE " -w" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-pragma-pack")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-function")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-variable")

add_subdirectory(lib/chaos buildchaos)
add_subdirectory(lib/tinycthread buildtinycthread)

#TODO: Link properly on Apple and Linux
target_link_libraries(mana
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/VulkanSDK/1.1.101.0/Lib/vulkan-1.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/openmp/libomp.lib
        glfw
        chaos
        tinycthread)

set(includeList
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/cstorage/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/cnoise/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/cglm/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinycthread/source
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/VulkanSDK/1.1.101.0/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/Chaos/include
        ${OpenMP_CXX_LIBRARIES})

target_include_directories(mana PUBLIC ${includeList})
