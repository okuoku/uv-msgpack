PROJECT(msgpack-c)

cmake_minimum_required(VERSION 2.8)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
option(MSGPACK_BUILD_TESTS "Build msgpack tests(requires gtest)" OFF)
find_package(Ruby REQUIRED)

macro(process_erb nam)
    get_filename_component(_abs ${nam} ABSOLUTE)
    get_filename_component(_basename ${nam} NAME)
    make_directory(${CMAKE_CURRENT_BINARY_DIR}/generated/type)
    execute_process(
        COMMAND ${RUBY_EXECUTABLE} -r erb -e "puts ERB.new(ARGF.read).result"
        ${_abs}.erb
        OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/generated/type/${_basename})
    # Hack.
    configure_file(${nam}.erb
        ${CMAKE_CURRENT_BINARY_DIR}/${_basename}.current.erb
        COPYONLY)
endmacro(process_erb)

process_erb(src/msgpack/type/tuple.hpp)
process_erb(src/msgpack/type/define.hpp)
process_erb(src/msgpack/zone.hpp)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 5)
set(VERSION_PATCH 7) # CMake local
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

set(MSGPACK_SRCS_C
    src/unpack.c
    src/objectc.c
    src/version.c
    src/vrefbuffer.c
    src/zone.c)

set(MSGPACK_SRCS_CXX
    src/object.cpp)

if(MSGPACK_ENABLE_GCC_CXX_ATOMIC)
    list(APPEND 
        MSGPACK_SRCS_CXX src/gcc_atomic)
endif()

set(MSGPACK_ROOT_HDRS_C
    src/msgpack.h)

set(MSGPACK_ROOT_HDRS_CXX
    src/msgpack.hpp)

set(MSGPACK_HDRS_C
    ../msgpack/pack_define.h
    ../msgpack/pack_template.h
    ../msgpack/unpack_define.h
    ../msgpack/unpack_template.h
    ../msgpack/sysdep.h
    src/msgpack/sbuffer.h
    src/msgpack/version.h
    src/msgpack/vrefbuffer.h
    src/msgpack/zbuffer.h
    src/msgpack/pack.h
    src/msgpack/unpack.h
    src/msgpack/object.h
    src/msgpack/zone.h)

set(MSGPACK_HDRS_CXX
    src/msgpack/msgpack.hpp
    src/msgpack/sbuffer.hpp
    src/msgpack/vrefbuffer.hpp
    src/msgpack/zbuffer.hpp
    src/msgpack/pack.hpp
    src/msgpack/unpack.hpp
    src/msgpack/object.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/zone.hpp
    src/msgpack/type.hpp
    src/msgpack/type/bool.hpp
    src/msgpack/type/deque.hpp
    src/msgpack/type/float.hpp
    src/msgpack/type/fixint.hpp
    src/msgpack/type/int.hpp
    src/msgpack/type/list.hpp
    src/msgpack/type/map.hpp
    src/msgpack/type/nil.hpp
    src/msgpack/type/pair.hpp
    src/msgpack/type/raw.hpp
    src/msgpack/type/set.hpp
    src/msgpack/type/string.hpp
    src/msgpack/type/vector.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/tuple.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/define.hpp
    src/msgpack/type/tr1/unordered_map.hpp
    src/msgpack/type/tr1/unordered_set.hpp)

if(MSVC)
    # hack.
    set_source_files_properties(${MSGPACK_SRCS_C}
        PROPERTIES
        LANGUAGE CXX)
endif()

configure_file(src/msgpack/version.h.in 
    ${CMAKE_CURRENT_BINARY_DIR}/msgpack/version.h 
    @ONLY)

include_directories(
    src 
    src/msgpack
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/generated
    ${CMAKE_CURRENT_BINARY_DIR}/generated/type)

add_library(msgpackc ${MSGPACK_SRCS_C})
add_library(msgpack ${MSGPACK_SRCS_C} ${MSGPACK_SRCS_CXX})

install(TARGETS msgpackc msgpack DESTINATION lib)
install(FILES ${MSGPACK_HDRS_C} ${MSGPACK_HDRS_CXX}
    ${CMAKE_CURRENT_BINARY_DIR}/msgpack/version.h
    DESTINATION include/msgpack)
install(FILES ${MSGPACK_ROOT_HDRS_C} ${MSGPACK_ROOT_HDRS_CXX}
    ${CMAKE_CURRENT_BINARY_DIR}/msgpack/version.h
    DESTINATION include)

if(MSGPACK_ENABLE_TESTS)
    enable_testing()
    find_package(GTest)
macro(add_msgpack_test nam)
    add_executable(${nam} test/${nam})
    target_link_libraries(${nam} msgpack ${GTEST_BOTH_LIBRARIES})
    set_target_properties(${nam} PROPERTIES FOLDER Tests)
    add_test(test-${nam} ${nam})
endmacro(add_msgpack_test)
macro(add_msgpack_tests)
    foreach(tgt ${ARGN})
        add_msgpack_test(${tgt})
    endforeach()
endmacro(add_msgpack_tests)
    include_directories(${GTEST_INCLUDE_DIRS})
    add_msgpack_tests(
        zone
        pack_unpack
        pack_unpack_c
        streaming
        streaming_c
        object
        convert
        buffer
        cases
        fixint
        fixint_c
        version
        msgpackc_test
        msgpack_test)
endif()

