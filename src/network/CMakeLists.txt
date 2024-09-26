# ----------------------------- Components ------------------------------
set(BOOST_LIBRARIES system serialization log log_setup)
set(OPENSSL_INCLUDE_LIBRARIES SSL Crypto)
set(QT_NETWORK_INCLUDE_LIBRARIES Core Gui Widgets)

# --------------------------- Find packages -----------------------------
find_package(Boost REQUIRED COMPONENTS ${BOOST_LIBRARIES})
find_package(OpenSSL REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS ${QT_NETWORK_INCLUDE_LIBRARIES})

# ------------------------------ Sources --------------------------------
file(GLOB_RECURSE NETWORK_SOURCES
    "*.cpp"
    "*.hpp"
)

# ------------------------------ Library --------------------------------
add_library(network
    ${NETWORK_SOURCES}
)

# ----------------------- Include directories ---------------------------
target_include_directories(network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# -------------------------- Library linking ----------------------------
list(TRANSFORM OPENSSL_INCLUDE_LIBRARIES PREPEND "OpenSSL::")
list(TRANSFORM QT_NETWORK_INCLUDE_LIBRARIES PREPEND "Qt6::")
list(TRANSFORM BOOST_LIBRARIES PREPEND "Boost::")
target_link_libraries(network PUBLIC
    common
    ${BOOST_LIBRARIES}
    ${OPENSSL_INCLUDE_LIBRARIES}
    ${QT_NETWORK_INCLUDE_LIBRARIES}
)

# -------------------- Windows-specific settings ------------------------
if(WIN32)
    target_link_libraries(network PRIVATE ws2_32 wsock32)
endif()
