# ----------------------------- Components ------------------------------
set(BOOST_LIBRARIES system serialization)
set(OPENSSL_INCLUDE_LIBRARIES SSL Crypto)

# --------------------------- Find packages -----------------------------
find_package(Boost REQUIRED COMPONENTS ${BOOST_LIBRARIES})
find_package(OpenSSL REQUIRED)

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
target_link_libraries(network PRIVATE
    common
    ${Boost_LIBRARIES}
    ${OPENSSL_INCLUDE_LIBRARIES}
)

# -------------------- Windows-specific settings ------------------------
if(WIN32)
    target_link_libraries(network PRIVATE ws2_32 wsock32)
    target_compile_definitions(network PRIVATE WIN32_LEAN_AND_MEAN)
endif()
