# Components
set(QT_COMMON_INCLUDE_LIBRARIES Core)

# Find packages
find_package(Qt6 REQUIRED COMPONENTS ${QT_COMMON_INCLUDE_LIBRARIES})

# Sources
file(GLOB_RECURSE COMMON_SOURCES Logger.hpp Logger.cpp)

# Library
add_library(common ${COMMON_SOURCES})

# Include directories
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Library linking
list(TRANSFORM QT_COMMON_INCLUDE_LIBRARIES PREPEND "Qt6::")
target_link_libraries(common PUBLIC ${QT_COMMON_INCLUDE_LIBRARIES})
