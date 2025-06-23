cmake_minimum_required(VERSION 3.22)
set(RUST_CRATE_NAME "rust_part")
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

set(RUST_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(RUST_PART_CXX "${RUST_OUT_DIR}/${RUST_CRATE_NAME}.cpp")
set(RUST_PART_H   "${RUST_OUT_DIR}/${RUST_CRATE_NAME}.h")
set(RUST_PART_LIB "${RUST_OUT_DIR}/${TARGET_DIR}/${RUST_CRATE_NAME}.lib")
set(RUST_PART_LIB "${RUST_PART_LIB}" PARENT_SCOPE)


add_library(${RUST_CRATE_NAME} STATIC ${RUST_PART_CXX})


add_custom_command(
        OUTPUT  ${RUST_PART_CXX} ${RUST_PART_H}
        COMMAND cargo clean
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

add_dependencies(${RUST_CRATE_NAME} rust_part_files)