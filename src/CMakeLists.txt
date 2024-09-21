# Set CMake settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt libs
set(QT_TOP_INCLUDE_LIBRARIES Widgets)

# Find packages
find_package(Qt6 REQUIRED COMPONENTS ${QT_TOP_INCLUDE_LIBRARIES})

# Project structure
add_subdirectory(network)
add_subdirectory(common)
add_subdirectory(gui)

# Sources
add_executable(main_executable main.cpp)

# Library linking
list(TRANSFORM QT_TOP_INCLUDE_LIBRARIES PREPEND "Qt6::")
target_link_libraries(main_executable PRIVATE
    common
    network
    gui
    ${QT_TOP_INCLUDE_LIBRARIES}
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
