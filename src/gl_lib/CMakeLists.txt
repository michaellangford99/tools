add_library(gl_lib STATIC
    base_window.cpp
)

target_include_directories(gl_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(gl_lib
    PUBLIC
        glad
        glfw
        glm
        stb_image
    PRIVATE
        OpenGL::GL
        imgui
        implot
        spdlog
)
