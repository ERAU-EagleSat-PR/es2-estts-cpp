# Set ES2-GSTTS source files.
# Source file list is used by the shared and static libraries.
# Please add any additions alphabetically to avoid duplication and make
# finding things easier for future maintainers.

set(
        estts-src
        utils/bin_converter.cpp
        utils/helper.cpp
        utils/port.cpp
        utils/stack.cpp
        utils/queue.cpp
        fapi/automator.cpp
        fapi/command_handler/acs_command.cpp
        fapi/command_handler/crp_command.cpp
        fapi/command_handler/eps_command.cpp
        fapi/command_handler.cpp
        fapi/command_handler/mde_command.cpp
        fapi/command_handler/obc_command.cpp
        fapi/session_manager.cpp
        fapi/communication_handler.cpp
        tnc_emulator/ax25_ui_frame_constructor.cpp
        tnc_emulator/ax25_ui_frame_destructor.cpp
        tnc_emulator/info_field.cpp
        ti/ti_esttc.cpp
        ti/ti_serial_handler.cpp
        ti/ti_socket_handler.cpp
        ti/transmission_interface.cpp
)

set(
        include_directories
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/fapi
        ${CMAKE_CURRENT_SOURCE_DIR}/fapi/command_handler
        ${CMAKE_CURRENT_SOURCE_DIR}/tnc_emulator
        ${CMAKE_CURRENT_SOURCE_DIR}/ti
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

set(estts_include_directories ${include_directories} PARENT_SCOPE)

# Configure build targets
add_library(
        estts
        STATIC
        ${estts-src}
)
target_include_directories(
        estts
        PUBLIC
        ${include_directories}
)

target_link_libraries(estts PUBLIC spdlog::spdlog)
target_link_libraries(estts PUBLIC nlohmann_json::nlohmann_json)

if (BUILD_DEBUG)
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

if (TI_DEV_MODE)
    target_compile_definitions(
            estts
            PRIVATE
            __TI_DEV_MODE__
    )
endif()
if (ESTTS_COMMAND_STREAM_MODE)
    target_compile_definitions(
            estts
            PRIVATE
            __ESTTS_COMMAND_STREAM_MODE__
    )
endif()
