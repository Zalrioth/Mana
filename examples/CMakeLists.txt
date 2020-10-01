cmake_minimum_required(VERSION 3.12)
project(HelloWorld LANGUAGES C VERSION 0.1.0)

set(ignoreMe "${CMAKE_CXX_COMPILER}")

file(GLOB hello_world_SRC *.c)

add_executable(HelloWorld ${hello_world_SRC})

get_filename_component(PARENT_DIR ../ ABSOLUTE)

add_subdirectory(${PARENT_DIR} buildmana)

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

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-visibility -Wno-pragma-pack -Wno-deprecated-declarations -mavx -mavx2")

message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE MATCHES Debug)
        message("Debug build.")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
        message("Release build.")
else()
        message("Some other build type.")
endif()

target_link_libraries(HelloWorld mana)

set(includeList
        ${PARENT_DIR}/include
        ${PARENT_DIR}/lib/cstorage/include
        ${PARENT_DIR}/lib/cnoise/include
        ${PARENT_DIR}/lib/cthreads/include
        ${PARENT_DIR}/lib/uber-math/include
        ${PARENT_DIR}/lib/stb
        ${PARENT_DIR}/lib/mimalloc/include
        ${PARENT_DIR}/lib/glfw/include
        ${PARENT_DIR}/lib/Vulkan-Headers/include
        ${PARENT_DIR}/lib/chaos/include
        ${OpenMP_CXX_LIBRARIES})

target_include_directories(HelloWorld PUBLIC ${includeList})

if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
        set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe")
else()
        set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin32/glslangValidator.exe")
endif()

file(GLOB_RECURSE GLSL_SOURCE_FILES
        ${PARENT_DIR}/assets/shaders/opengl/*.frag
        ${PARENT_DIR}/assets/shaders/opengl/*.vert)

foreach(GLSL ${GLSL_SOURCE_FILES})
        get_filename_component(FILE_NAME ${GLSL} NAME)
        set(SPIRV ${PARENT_DIR}/assets/shaders/spirv/${FILE_NAME}.spv)
        add_custom_command(
                OUTPUT ${SPIRV}
                COMMAND ${CMAKE_COMMAND} -E make_directory ${PARENT_DIR}/assets/shaders/spirv
                COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
                DEPENDS ${GLSL})
        list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(Shaders DEPENDS ${SPIRV_BINARY_FILES})
add_dependencies(HelloWorld Shaders)

file(GLOB_RECURSE ALL_ASSETS
        ${PARENT_DIR}/assets/*)

add_custom_target(Assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${PARENT_DIR}/assets ${PROJECT_BINARY_DIR}/assets)

add_dependencies(HelloWorld Assets)
