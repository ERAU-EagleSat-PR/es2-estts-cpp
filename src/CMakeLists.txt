# Set ES2-GSTTS source files.
# Source file list is used by the shared and static libraries.
# Please add any additions alphabetically to avoid duplication and make
# finding things easier for future maintainers.

set(
        estts-src
        utils/bin_converter.cpp
        utils/helper.cpp
        fapi/command_handler.cpp
        fapi/obc_session_manager.cpp
        fapi/cosmos_handler.cpp
        fapi/cosmos_groundstation_handler.cpp
        fapi/cosmos_satellite_txvr_handler.cpp
        fapi/satellite_txvr_cmdtelem_manager.cpp
        fapi/groundstation_cmdtelem_manager.cpp
        #tnc_emulator/ax25_ui_frame_constructor.cpp
        #tnc_emulator/ax25_ui_frame_destructor.cpp
        #tnc_emulator/info_field.cpp
        ti/esttc.cpp
        ti/serial_handler.cpp
        ti/socket_handler.cpp
        ti/transmission_interface.cpp
)

set(Boost_USE_MULTITHREADED ON)
find_package(
        Boost
        1.67.0
        REQUIRED
        COMPONENTS system
)

set(
        include_directories
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/fapi
        ${CMAKE_CURRENT_SOURCE_DIR}/fapi/command_handler
        ${CMAKE_CURRENT_SOURCE_DIR}/tnc_emulator
        ${CMAKE_CURRENT_SOURCE_DIR}/ti
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
        ${Boost_INCLUDE_DIRS}
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

target_link_libraries(
        estts
        PUBLIC
        spdlog::spdlog
)
target_link_libraries(
        estts
        PUBLIC
        nlohmann_json::nlohmann_json
)

message(STATUS "Boost version: ${Boost_VERSION}")
message(STATUS "Using Boost libraries: ${Boost_LIBRARIES}")
message(STATUS "Boost include directories: ${Boost_INCLUDE_DIRS}")
if(NOT TARGET ${Boost_LIBRARIES})
    add_library(${Boost_LIBRARIES} IMPORTED INTERFACE)
    set_property(TARGET ${Boost_LIBRARIES} PROPERTY
            INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
    set_property(TARGET ${Boost_LIBRARIES} PROPERTY
            INTERFACE_LINK_LIBRARIES ${Boost_LIBRARIES})
endif()

target_link_libraries(
        estts
        PUBLIC
        ${Boost_LIBRARIES}
)

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

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_compile_definitions(
            estts
            PRIVATE
            __ESTTS_OS_LINUX__
    )
endif()