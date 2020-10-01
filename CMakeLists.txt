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

set_target_properties(mana
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx -mavx2")

# /MDd
if (WIN32)
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

add_subdirectory(lib/chaos buildchaos)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
add_subdirectory(lib/mimalloc buildmimalloc)
string(REPLACE " -w" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations")

# TODO: Link properly on Apple and Linux
# On Windows link mimalloc statically for performance and dynamically for stability
# Unix-like can static override instead
set(MANA_BUILD_LIBRARIES mana
        vulkan
        glfw
        chaos
        #mimalloc
        mimalloc-static)

if (WIN32 AND CMAKE_C_COMPILER_ID MATCHES "Clang")
        STRING(REPLACE " " "\ " PATH_NO_SPACE "$ENV{ProgramFiles}")
        set(MANA_BUILD_LIBRARIES ${MANA_BUILD_LIBRARIES} "${PATH_NO_SPACE}/LLVM/lib/libomp.lib")
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
        ${OpenMP_CXX_LIBRARIES})

target_include_directories(mana PUBLIC ${includeList})
