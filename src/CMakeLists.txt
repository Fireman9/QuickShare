# ------------------------- Project structure ---------------------------
add_subdirectory(network)
add_subdirectory(common)

# ------------------------------ Sources --------------------------------
add_executable(main_executable main.cpp)

# -------------------------- Library linking ----------------------------
target_link_libraries(main_executable PRIVATE
    common
    network
)
