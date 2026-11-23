file(GLOB_RECURSE WAVE_PLOT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(wave_plot_lib STATIC
    ${WAVE_PLOT_SOURCES}
)

target_include_directories(wave_plot_lib PUBLIC
    "${PROJECT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/include/glad"
    "${PROJECT_SOURCE_DIR}/include/KHR"
)

target_include_directories(wave_plot_lib PRIVATE
    ${FREETYPE_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
)

target_link_libraries(wave_plot_lib PRIVATE
    glfw
    Freetype::Freetype
    OpenGL::GL
    pthread
    dl
    glad
    imgui
    ImGuiFileDialog
    implot
    glm
    spdlog
    xtensor
    gl_lib
    Freetype::Freetype
)

copy_glsl_files("${CMAKE_CURRENT_SOURCE_DIR}" wave_plot_lib)

target_compile_options(wave_plot_lib PRIVATE -Wall -Wextra)
# Only keep the line below if GLAD is being built as a shared (.so) library
#target_compile_definitions(wave_plot_lib PRIVATE GLAD_GLAPI_EXPORT_BUILD)

add_executable(wave_plot
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

target_link_libraries(wave_plot PRIVATE
    wave_plot_lib
    glfw
    Freetype::Freetype
    OpenGL::GL
    pthread
    dl
    imgui
    ImGuiFileDialog
    implot
    glm
    spdlog
    xtensor
    gl_lib
)
