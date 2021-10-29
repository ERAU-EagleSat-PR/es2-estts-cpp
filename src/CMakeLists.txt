# Set ES2-GSTTS source files.
# Source file list is used by the shared and static libraries.
# Please add any additions alphabetically to avoid duplication and make
# finding things easier for future maintainers.

set(
        es2-libestts-src
        fapi/feature_api.cpp
        sapi/system_api.cpp
        ti/transmission_interface.cpp
)

set(
        include_directories
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/fapi
        ${CMAKE_CURRENT_SOURCE_DIR}/sapi
        ${CMAKE_CURRENT_SOURCE_DIR}/ti
)

set(es2-libestts_include_directories ${include_directories} PARENT_SCOPE)

# Configure build targets
add_library(
        es2-libestts
        SHARED
        ${es2-libestts-src}
)
target_include_directories(
        es2-libestts
        PUBLIC
        ${include_directories}
)

if(BUILD_DEBUG)
    target_compile_definitions(
            es2-libestts
            PRIVATE
    )
    target_compile_options(
            es2-libestts
            PRIVATE
            -Wall
    )
else()
    target_compile_definitions(
            es2-libestts
            PRIVATE
    )
    target_compile_options(
            es2-libestts
            PRIVATE
            -w
    )
endif()