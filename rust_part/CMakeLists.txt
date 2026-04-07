cmake_minimum_required(VERSION 3.22)
set(RUST_CRATE_NAME "rust_part")
set(RUST_CMAKE_TARGET_NAME "rust_part_crate")

set(_cargo_release_flag "$<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:--release>")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug Build")
    set(CARGO_CMD cargo build)
    set(TARGET_DIR "debug")
else ()
    message(STATUS "Release Build")
    set(CARGO_CMD cargo build --release)
    set(TARGET_DIR "release")
endif ()

# If the user is cross compiling to a different target, update the location of the target directory
# *Note*: Make sure to install the target with rustup, as well as setting the proper environment variables, such as CC, HOST_CC, CARGO_TARGET_<triple>_LINKER, ...
# See https://doc.rust-lang.org/cargo/reference/environment-variables.html#configuration-environment-variables
if (DEFINED ENV{CARGO_BUILD_TARGET})
    set(TARGET_DIR "$ENV{CARGO_BUILD_TARGET}/${TARGET_DIR}")
endif()
set(ENV{RUST_BACKTRACE} "full")

set(RUST_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(RUST_PART_CXX "${RUST_OUT_DIR}/${RUST_CRATE_NAME}.cpp")
set(RUST_PART_H   "${RUST_OUT_DIR}/${RUST_CRATE_NAME}.h")

if (MSVC)
    set(_rust_lib_debug   "${CMAKE_CURRENT_SOURCE_DIR}/target/debug/${RUST_CRATE_NAME}.lib")
    set(_rust_lib_release "${CMAKE_CURRENT_SOURCE_DIR}/target/release/${RUST_CRATE_NAME}.lib")
else ()
    set(RUST_PART_LIB "${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_DIR}/lib${RUST_CRATE_NAME}.a")
endif ()

set(RUST_PART_LIB
    "$<$<CONFIG:Debug>:${_rust_lib_debug}>$<$<CONFIG:Release>:${_rust_lib_release}>$<$<CONFIG:RelWithDebInfo>:${_rust_lib_release}>$<$<CONFIG:MinSizeRel>:${_rust_lib_release}>"
    PARENT_SCOPE
)
add_library(${RUST_CMAKE_TARGET_NAME} STATIC
    ${RUST_PART_CXX}
)

target_include_directories(${RUST_CMAKE_TARGET_NAME} PUBLIC
    "${RUST_OUT_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/cxx/include"
)

add_custom_command(
        OUTPUT  ${RUST_PART_CXX} ${RUST_PART_H}
        COMMAND ${CARGO_CMD} --target-dir=${RUST_OUT_DIR}           # e.g. cargo build --release
        COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/cxxbridge/${RUST_CRATE_NAME}/src/lib.rs.cc ${RUST_PART_CXX}
        COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/cxxbridge/${RUST_CRATE_NAME}/src/lib.rs.h ${CMAKE_CURRENT_BINARY_DIR}/rust_part.h
        BYPRODUCTS ${RUST_PART_LIB}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS   ${CMAKE_CURRENT_SOURCE_DIR}/src/lib.rs
)

add_custom_target(rust_part_files
        DEPENDS
        ${RUST_PART_CXX}   # rust_part.cpp
        ${RUST_PART_H}     # rust_part.h
)

add_dependencies(${RUST_CMAKE_TARGET_NAME} rust_part_files)


