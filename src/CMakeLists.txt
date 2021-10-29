# Set ES2-GSTTS source files.
# Source file list is used by the shared and static libraries.
# Please add any additions alphabetically to avoid duplication and make
# finding things easier for future maintainers.

set(
        libes2-estts-src
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

set(libes2-estts_include_directories ${include_directories} PARENT_SCOPE)

# Configure build targets
add_library(
        libes2-estts
        SHARED
        ${libes2-estts-src}
)
target_include_directories(
        libes2-estts
        PUBLIC
        ${include_directories}
)

if(BUILD_DEBUG)
    target_compile_definitions(
            libes2-estts
            PRIVATE
    )
    target_compile_options(
            libes2-estts
            PRIVATE
            -Wall
    )
else()
    target_compile_definitions(
            libes2-estts
            PRIVATE
    )
    target_compile_options(
            libes2-estts
            PRIVATE
            -w
    )
endif()