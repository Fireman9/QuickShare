# ----------------------------- Components ------------------------------
set(BOOST_LIBRARIES log log_setup)

# --------------------------- Find packages -----------------------------
find_package(Boost REQUIRED COMPONENTS ${BOOST_LIBRARIES})

# ------------------------------ Sources --------------------------------
file(GLOB_RECURSE COMMON_SOURCES
    "*.cpp"
    "*.hpp"
)

# ------------------------------ Library --------------------------------
add_library(common ${COMMON_SOURCES})
set_target_properties(common PROPERTIES LINKER_LANGUAGE CXX)

# ----------------------- Include directories ---------------------------
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# -------------------------- Library linking ----------------------------
target_link_libraries(common PUBLIC ${Boost_LIBRARIES})
