# Set ES2-GSTTS source files.
# Source file list is used by the shared and static libraries.
# Please add any additions alphabetically to avoid duplication and make
# finding things easier for future maintainers.

# Add new files in order
set(
        estts-src
        utils/bin_converter.cpp
<<<<<<< HEAD
        fapi/feature_api.cpp
        sapi/frame_constructor.cpp
        sapi/info_field.cpp
        utils/logger.cpp
        utils/port.cpp
        ti/transmission_interface.cpp
=======
        sapi/endurosat_scrambling.cpp
        fapi/feature_api.cpp
        sapi/frame_constructor.cpp
        sapi/info_field.cpp
        utils/port.cpp
        sapi/system_api.cpp
        ti/ti_esttc.cpp
        ti/ti_serial_handler.cpp
>>>>>>> main
)

set(
        include_directories
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/fapi
        ${CMAKE_CURRENT_SOURCE_DIR}/sapi
        ${CMAKE_CURRENT_SOURCE_DIR}/ti
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
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

target_link_libraries(estts PUBLIC spdlog::spdlog)

<<<<<<< HEAD
if (BUILD_DEBUG)
=======
if(BUILD_DEBUG)
>>>>>>> main
    target_compile_definitions(
            estts
            PRIVATE
    )
    target_compile_options(
            estts
            PRIVATE
            -Wall
    )
else ()
    target_compile_definitions(
            estts
            PRIVATE
    )
    target_compile_options(
            estts
            PRIVATE
            -w
    )
endif ()