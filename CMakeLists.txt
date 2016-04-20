project(sdl2-multiplatform-base)
cmake_minimum_required(VERSION 2.8)

aux_source_directory(src SRC_LIST)

add_definitions(-DBUILD_LINUX)

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} SDL2 SDL2_mixer SDL2_image)

# Copy assets
#file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
add_custom_target(copy-runtime-files ALL
    COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR})
