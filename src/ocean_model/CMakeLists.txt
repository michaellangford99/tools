file(GLOB_RECURSE OCEAN_MODEL_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

set_target_properties(glad gl_lib imgui implot spdlog PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

add_library(ocean_model SHARED
    ${OCEAN_MODEL_SOURCES}
)

target_include_directories(ocean_model PUBLIC
    "${PROJECT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/include/glad"
    "${PROJECT_SOURCE_DIR}/include/KHR"
)

target_include_directories(ocean_model PRIVATE
    ${FREETYPE_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
)

target_link_libraries(ocean_model PRIVATE
    glfw
    OpenGL::GL
    pthread
    dl
    glad
    imgui
    implot
    glm
    spdlog
    gl_lib
)

copy_glsl_files("${CMAKE_CURRENT_SOURCE_DIR}" ocean_model)

add_executable(ocean_model_test
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

target_link_libraries(ocean_model_test PRIVATE
    ocean_model
    glfw
    OpenGL::GL
    pthread
    dl
    glad
    imgui
    implot
    glm
    spdlog
    gl_lib
)
