
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


set(RUST_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(RUST_PART_CXX "${RUST_OUT_DIR}/${RUST_CRATE_NAME}.cpp")
set(RUST_PART_H   "${RUST_OUT_DIR}/${RUST_CRATE_NAME}.h")
set(RUST_PART_LIB "${RUST_OUT_DIR}/${TARGET_DIR}/${RUST_CRATE_NAME}.lib")


add_library(${RUST_CRATE_NAME} STATIC ${RUST_PART_CXX})

add_custom_command(
        OUTPUT  ${RUST_PART_CXX} ${RUST_PART_H}
        COMMAND ${CARGO_CMD}  --target-dir=${RUST_OUT_DIR}           # e.g. cargo build --release
        COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/cxxbridge/${RUST_CRATE_NAME}/src/lib.rs.cc ${RUST_PART_CXX}
        COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/cxxbridge/${RUST_CRATE_NAME}/src/lib.rs.h ${CMAKE_CURRENT_BINARY_DIR}/rust_part.h
        BYPRODUCTS ${RUST_PART_LIB}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS   ${CMAKE_CURRENT_SOURCE_DIR}/src/lib.rs
)

target_link_libraries(${RUST_CRATE_NAME} ${RUST_PART_LIB})


