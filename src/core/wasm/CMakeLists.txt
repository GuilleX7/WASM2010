cmake_minimum_required(VERSION 3.23)
project(WASM2010 LANGUAGES C)

add_subdirectory("subprojects/ASM2010")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_SHARED_LIBRARY_PREFIX_C "")
set(CMAKE_STATIC_LIBRARY_PREFIX_C "lib")

set(WASM2010_SOURCE
"src/wasm_glue.c")

if(MSVC)
    set(WASM2010_COMPILE_OPTIONS "/W2" "$<$<CONFIG:RELEASE>:/O2>")
else()
    set(WASM2010_COMPILE_OPTIONS "-Wall" "-Wextra" "$<$<CONFIG:RELEASE>:-O3>")
endif()
set(WASM2010_PROPERTIES OUTPUT_NAME WASM2010 C_VISIBILITY_PRESET hidden)

add_executable(WASM2010 ${WASM2010_SOURCE})
set_target_properties(WASM2010 PROPERTIES ${WASM2010_PROPERTIES})
target_compile_options(WASM2010 PUBLIC ${WASM2010_COMPILE_OPTIONS})

target_link_libraries(WASM2010 "-Wl,--whole-archive" libASM2010 "-Wl,--no-whole-archive")
target_link_options(WASM2010 PUBLIC "-Wl,--no-entry" "-nostartfiles" "-Wl,--export-dynamic" "-Wl,--export=malloc" "-Wl,--export=free" "-Wl,--import-undefined")
set_target_properties(WASM2010 PROPERTIES SUFFIX ".wasm")

install(TARGETS WASM2010 DESTINATION ${CMAKE_BINARY_DIR})
