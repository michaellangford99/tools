cmake_minimum_required(VERSION 3.16)
project(wave_plot VERSION 0.0 LANGUAGES C CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Freetype REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3)

include(FetchContent)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.6-docking
)
FetchContent_MakeAvailable(imgui)
add_library(imgui STATIC)

target_sources(imgui PRIVATE
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
)

target_sources(imgui PRIVATE
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC 
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC glfw)

#---- IMPlot ----

FetchContent_Declare(
    implot
    GIT_REPOSITORY https://github.com/epezent/implot.git
    GIT_TAG        v1.0  # Replace with your preferred release version
)
FetchContent_MakeAvailable(implot)

# Create the ImPlot library target manually
add_library(implot STATIC)

# Add core ImPlot source files
target_sources(implot PRIVATE
    ${implot_SOURCE_DIR}/implot.cpp
    ${implot_SOURCE_DIR}/implot_demo.cpp
    ${implot_SOURCE_DIR}/implot_items.cpp
)

# Configure include directories (PUBLIC so dependents can find implot.h)
target_include_directories(implot PUBLIC 
    ${implot_SOURCE_DIR}
)

target_link_libraries(implot PUBLIC imgui)

#---- GLM ----

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.3
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(glm)

#---- spdlog ----

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG        v1.17.0
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(spdlog)

#---- xtensor ----

FetchContent_Declare(
    xtl
    GIT_REPOSITORY https://github.com/xtensor-stack/xtl
    GIT_TAG        0.8.2
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(xtl)

FetchContent_Declare(
    xtensor
    GIT_REPOSITORY https://github.com/xtensor-stack/xtensor
    GIT_TAG        0.27.1
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(xtensor)

#-------------------------------------------------

file(GLOB_RECURSE WAVE_PLOT_SOURCES
    "${PROJECT_SOURCE_DIR}/src/wave_plot/*.cpp"
)

# add glad.c - a generated file.
list(APPEND WAVE_PLOT_SOURCES
    "${PROJECT_SOURCE_DIR}/src/wave_plot/glad.c"
)

add_library(wave_plot_lib STATIC
    ${WAVE_PLOT_SOURCES}
)

target_include_directories(wave_plot_lib PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/wave_plot
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/glad
    ${PROJECT_SOURCE_DIR}/include/stb_image
    ${PROJECT_SOURCE_DIR}/include/KHR
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
)

target_link_libraries(wave_plot_lib PRIVATE 
    imgui
    implot
    glm
    spdlog
    xtensor
)

target_compile_options(wave_plot_lib PRIVATE -Wall -Wextra)
# Only keep the line below if GLAD is being built as a shared (.so) library
#target_compile_definitions(wave_plot_lib PRIVATE GLAD_GLAPI_EXPORT_BUILD)

add_executable(wave_plot "${PROJECT_SOURCE_DIR}/src/wave_plot/main.cpp")
target_link_libraries(wave_plot PRIVATE wave_plot_lib)
target_link_libraries(wave_plot PRIVATE
    glfw
    Freetype::Freetype
    OpenGL::GL
    pthread
    dl
)
target_link_libraries(wave_plot PRIVATE 
    imgui
    implot
    glm
    spdlog
    xtensor
)