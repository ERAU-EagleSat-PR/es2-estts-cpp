# Set ES2-GSTTS source files.
# Source file list is used by the shared and static libraries.
# Please add any additions alphabetically to avoid duplication and make
# finding things easier for future maintainers.

set(
        estts-src
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

set(estts_include_directories ${include_directories} PARENT_SCOPE)

# Configure build targets
add_library(
        estts
        SHARED
        ${estts-src}
)
target_include_directories(
        estts
        PUBLIC
        ${include_directories}
)

if(BUILD_DEBUG)
    target_compile_definitions(
            estts
            PRIVATE
    )
    target_compile_options(
            estts
            PRIVATE
            -Wall
    )
else()
    target_compile_definitions(
            estts
            PRIVATE
    )
    target_compile_options(
            estts
            PRIVATE
            -w
    )
endif()