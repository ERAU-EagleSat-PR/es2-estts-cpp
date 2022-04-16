#
# estts-primary_cosmos_worker-demo1 build configuration.
#

# Add primary executable for estts-demo1
add_executable(estts-cosmos-demo1 src/main.cpp)

target_include_directories(
        estts-cosmos-demo1
        PUBLIC
        ${estts_include_directories}
)

# Link GSTTS library to build
target_link_libraries(estts-cosmos-demo1 PUBLIC estts)