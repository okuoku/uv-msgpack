project(uv-msgpack)
cmake_minimum_required(VERSION 2.8)

option(UVMSGPACK_BUILD_SAMPLES "Build uv-msgpack samples" OFF)

# FIXME: Why??
configure_file(msgpack-c/sysdep.h
    ${CMAKE_CURRENT_BINARY_DIR}/msgpack/sysdep.h
    COPYONLY)
configure_file(msgpack-c/unpack_define.h
    ${CMAKE_CURRENT_BINARY_DIR}/msgpack/unpack_define.h
    COPYONLY)
configure_file(msgpack-c/unpack_template.h
    ${CMAKE_CURRENT_BINARY_DIR}/msgpack/unpack_template.h
    COPYONLY)
configure_file(msgpack-c/pack_define.h
    ${CMAKE_CURRENT_BINARY_DIR}/msgpack/pack_define.h
    COPYONLY)
configure_file(msgpack-c/pack_template.h
    ${CMAKE_CURRENT_BINARY_DIR}/msgpack/pack_template.h
    COPYONLY)

# FIXME: Make sure msgpack will look into them
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    )

# NB: We use these subdirectories later...
add_subdirectory(uv uv EXCLUDE_FROM_ALL)
add_subdirectory(msgpack-c msgpack EXCLUDE_FROM_ALL) 

set(DEP_LIBS msgpack uv)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/msgpack
    msgpack-c
    msgpack-c/src
    include
    uv/include
    )

if(WIN32)
    list(APPEND DEP_LIBS ws2_32)
else(UNIX)
    list(APPEND DEP_LIBS pthread m)
endif()

if(APPLE)
    find_library(core_services CoreServices)
    list(APPEND DEP_LIBS ${core_services})
endif()

# lib
set(srcs
    src/writer.c
    src/reader.c)

add_library(uvmsgpack
    STATIC
    ${srcs})

list(APPEND DEP_LIBS uvmsgpack)

macro(add_sample nam)
    add_executable(${nam} ${ARGN})
    target_link_libraries(${nam} ${DEP_LIBS})
endmacro(add_sample)

if(UVMSGPACK_BUILD_SAMPLES)
    add_sample(addserv samples/addserv.c)
endif()
