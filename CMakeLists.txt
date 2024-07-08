# ---------------------------- Project setup ----------------------------
cmake_minimum_required(VERSION 3.24)
project(QuickShare VERSION 0.1 LANGUAGES CXX)

# -------------------------- Set C++ settings ---------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------- Set CMake settings --------------------------
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# --------------------------- Boost settings ----------------------------
set(BOOST_ENABLE_CMAKE ON)
set(BOOST_COMPONENTS system serialization)

# ---------------------------- Qt settings ------------------------------
set(QT_INCLUDE_LIBRARIES Core Gui Widgets)
set(OPENSSL_INCLUDE_LIBRARIES SSL Crypto)

# --------------------------- Find packages -----------------------------
find_package(Qt6 REQUIRED COMPONENTS ${QT_INCLUDE_LIBRARIES})
find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})
find_package(OpenSSL REQUIRED)

# ------------------------- Project structure ---------------------------
file(GLOB_RECURSE PROJECT_SOURCES
    "src/*.cpp"
    "src/*.hpp"
)

# ---------------------------- Executable -------------------------------
add_executable(QuickShare ${PROJECT_SOURCES})

# ----------------------- Include directories ---------------------------
target_include_directories(QuickShare PRIVATE ${Boost_INCLUDE_DIRS})

# -------------------------- Library linking ----------------------------
list(TRANSFORM QT_INCLUDE_LIBRARIES PREPEND "Qt6::")
list(TRANSFORM OPENSSL_INCLUDE_LIBRARIES PREPEND "OpenSSL::")
target_link_libraries(QuickShare PRIVATE
    ${Boost_LIBRARIES}
    ${QT_INCLUDE_LIBRARIES}
    ${OPENSSL_INCLUDE_LIBRARIES}
)

# -------------------- Windows-specific settings ------------------------
if(WIN32)
    target_link_libraries(QuickShare PRIVATE ws2_32 wsock32)
    target_compile_definitions(QuickShare PRIVATE WIN32_LEAN_AND_MEAN)
endif()
