#
# estts-demo1 build configuration.
#

# Add primary executable for estts-demo1
add_executable(estts-demo1 src/main.cpp)

target_include_directories(
        estts-demo1
        PUBLIC
        ${es2-libestts_include_directories}
)

# Link GSTTS library to build
target_link_libraries(estts-demo1 PUBLIC es2-libestts)