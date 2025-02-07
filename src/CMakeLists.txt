cmake_minimum_required(VERSION 3.15)
project(RDR1ExampleScript)

# Add source files
file(GLOB SOURCES "*.cpp" "*.h")

# Create the executable or library
add_library(ScriptHookRDR-Example SHARED ${SOURCES})

# Link libraries
target_link_libraries(ScriptHookRDR-Example
    ${CMAKE_SOURCE_DIR}/lib/ScriptHookRDR.lib
)

# Set output directory
set_target_properties(ScriptHookRDR-Example PROPERTIES
    SUFFIX ".asi"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)