cmake_minimum_required(VERSION 3.12)
project(mana LANGUAGES C VERSION 0.1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_DEBUG_POSTFIX "d")

# Might be needed for OpenMP on Mac/Liunux
#https://stackoverflow.com/questions/56202041/compiling-and-linking-against-openmp-with-appleclang-on-mac-os-x-mojave

file(GLOB_RECURSE mana_SRC
        include/*.h
        src/*.c)

add_library(mana STATIC ${mana_SRC})

message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_definitions(-DCHECK_MEMORY_LEAKS=1)
        message("Debug build.")
else()
        add_definitions(-DCUSTOM_ALLOCATOR=1)
endif()

set_target_properties(mana
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx2")

#if(CMAKE_BUILD_TYPE MATCHES Debug)
#        add_definitions(-DDEBUG)
#        message("Debug build.")
#elseif(CMAKE_BUILD_TYPE MATCHES Release)
#        message("Release build.")
#else()
#        message("Some other build type.")
#endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")

set(VULKAN_HEADERS_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/Vulkan-Headers/include)
set(VulkanRegistry_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/Vulkan-Headers/include)
set(VulkanHeaders_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/Vulkan-Headers/include)
add_subdirectory(lib/Vulkan-Loader buildvulkan)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/glfw buildglfw)

string(REPLACE " -w" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-pragma-pack -Wno-unused-function -Wno-unused-variable")

set(MI_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(MI_BUILD_OBJECT OFF CACHE BOOL "" FORCE)
set(MI_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
add_subdirectory(lib/mimalloc buildmimalloc)
string(REPLACE " -w" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_PROGRAMS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ENABLE_CPACK OFF CACHE BOOL "" FORCE)
set(ENABLE_EXPERIMENTAL OFF CACHE BOOL "" FORCE)
set(ENABLE_BOW_DOCS OFF CACHE BOOL "" FORCE)
set(ENABLE_PACKAGE_CONFIG OFF CACHE BOOL "" FORCE)
set(INSTALL_PKGCONFIG_MODULE OFF CACHE BOOL "" FORCE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
add_subdirectory(lib/libsndfile buildlibsndfile)
string(REPLACE " -w" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

set(BUILD_DYNAMIC_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
set(BUILD_EXAMPLE_PROGRAMS OFF CACHE BOOL "" FORCE)
set(DOXYGEN_CONF_FILE OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
add_subdirectory(lib/libsoundio buildlibsoundio)
string(REPLACE " -w" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

# /MDd
if (WIN32)
        #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Xclang -fopenmp -qopenmp-link=static")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Xclang -fopenmp")
endif ()

# -fsanitize=address
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

add_subdirectory(lib/chaos buildchaos)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations")

# TODO: Link properly on Apple and Linux
set(MANA_BUILD_LIBRARIES mana
        vulkan
        glfw
        chaos
        #mimalloc
        mimalloc-static
        sndfile
        libsoundio_static)

if (WIN32 AND CMAKE_C_COMPILER_ID MATCHES "Clang")
        STRING(REPLACE " " "\ " PATH_NO_SPACE "$ENV{ProgramFiles}")
        set(MANA_BUILD_LIBRARIES ${MANA_BUILD_LIBRARIES} "${PATH_NO_SPACE}/LLVM/lib/libomp.lib" "${PATH_NO_SPACE}/LLVM/lib/libiomp5md.lib")
endif ()

target_link_libraries(${MANA_BUILD_LIBRARIES})

set(includeList
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/cstorage/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/cnoise/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/cthreads/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/uber-math/include/ubermath
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/stb
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/Vulkan-Headers/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/chaos/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mimalloc/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/libsndfile/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/libsoundio/soundio
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/cxml-parser/include
        ${OpenMP_CXX_LIBRARIES})

target_include_directories(mana PUBLIC ${includeList})
