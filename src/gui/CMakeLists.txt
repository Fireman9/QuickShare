# Set CMake settings
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Qt libs
set(QT_INCLUDE_LIBRARIES Core Gui Widgets)

# Find packages
find_package(Qt6 REQUIRED COMPONENTS ${QT_INCLUDE_LIBRARIES})

# Sources
file(GLOB_RECURSE GUI_SOURCES
    "*.cpp"
    "*.hpp"
)

# Library
add_library(gui
    ${GUI_SOURCES}
)

# Include directories
target_include_directories(gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Library linking
list(TRANSFORM QT_INCLUDE_LIBRARIES PREPEND "Qt6::")
target_link_libraries(gui PUBLIC
    common
    network
    ${QT_INCLUDE_LIBRARIES}
)
