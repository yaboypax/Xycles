cmake_minimum_required(VERSION 3.22)
set(RUST_CRATE_NAME "rust_part")
set(RUST_CMAKE_TARGET_NAME "rust_part_crate")

set(_cargo_release_flag "$<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:--release>")

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

if(MSVC)
  set(_lib_name "${RUST_CRATE_NAME}.lib")
else()
  set(_lib_name "lib${RUST_CRATE_NAME}.a")
endif()

set(_rust_lib_debug   "${CMAKE_CURRENT_SOURCE_DIR}/target/debug/${_lib_name}")
set(_rust_lib_release "${CMAKE_CURRENT_SOURCE_DIR}/target/release/${_lib_name}")
set(RUST_PART_LIB
        "$<$<CONFIG:Debug>:${_rust_lib_debug}>$<$<CONFIG:Release>:${_rust_lib_release}>$<$<CONFIG:RelWithDebInfo>:${_rust_lib_release}>$<$<CONFIG:MinSizeRel>:${_rust_lib_release}>"
        PARENT_SCOPE
        )

add_custom_target(rust_part
    COMMAND cargo build ${_cargo_release_flag}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    BYPRODUCTS
        ${_rust_lib_debug}
        ${_rust_lib_release}
)

