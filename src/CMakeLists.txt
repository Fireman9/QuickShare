# Project structure
add_subdirectory(network)
add_subdirectory(common)

# Sources
add_executable(main_executable main.cpp)

# Library linking
target_link_libraries(main_executable PRIVATE
    common
    network
)

# Set output directory for main_executable
set_target_properties(main_executable PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Copy build directory
set(COPY_DESTINATION "${CMAKE_SOURCE_DIR}/build2")
add_custom_command(
    TARGET main_executable POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_BINARY_DIR}
    ${COPY_DESTINATION}
    COMMENT "Copying build directory to ${COPY_DESTINATION}"
)
