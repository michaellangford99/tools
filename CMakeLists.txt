cmake_minimum_required(VERSION 3.16)
project(wave_plot VERSION 0.0 LANGUAGES C CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Freetype REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Freetype REQUIRED)

include(FetchContent)

add_library(imgui_user_config INTERFACE)
target_include_directories(imgui_user_config INTERFACE
    "${PROJECT_SOURCE_DIR}/include"
)
target_compile_definitions(imgui_user_config INTERFACE
    IMGUI_USER_CONFIG="custom_imconfig.h"
)

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
    ${imgui_SOURCE_DIR}/misc/freetype/imgui_freetype.cpp
)

target_include_directories(imgui PUBLIC 
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC glfw imgui_user_config Freetype::Freetype)

#---- ImGuiFileDialog ----

FetchContent_Declare(
    imgui_file_dialog
    GIT_REPOSITORY https://github.com/aiekick/ImGuiFileDialog.git
    GIT_TAG        v0.6.7
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(imgui_file_dialog)
target_link_libraries(ImGuiFileDialog PUBLIC imgui Freetype::Freetype)

#TODO: pull the repo myself
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE
    "${imgui_file_dialog_SOURCE_DIR}/stb"
)

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

target_link_libraries(implot PUBLIC imgui imgui_user_config Freetype::Freetype)

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

add_library(glad STATIC
    "${PROJECT_SOURCE_DIR}/src/gl_lib/glad.c"
)

target_include_directories(glad PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
)
target_link_libraries(glad PUBLIC
    ${CMAKE_DL_LIBS}
)

function(copy_glsl_files source_dir target)
    file(GLOB_RECURSE GLSL_FILES CONFIGURE_DEPENDS
        "${source_dir}/*.glsl"
    )

    set(GLSL_OUTPUTS)
    foreach(GLSL_FILE IN LISTS GLSL_FILES)
        file(RELATIVE_PATH GLSL_RELATIVE_PATH "${source_dir}" "${GLSL_FILE}")
        set(OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${GLSL_RELATIVE_PATH}")
        get_filename_component(OUTPUT_DIRECTORY "${OUTPUT_FILE}" DIRECTORY)

        add_custom_command(
            OUTPUT "${OUTPUT_FILE}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${OUTPUT_DIRECTORY}"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${GLSL_FILE}" "${OUTPUT_FILE}"
            DEPENDS "${GLSL_FILE}"
            VERBATIM
        )
        list(APPEND GLSL_OUTPUTS "${OUTPUT_FILE}")
    endforeach()

    add_custom_target("${target}_glsl" DEPENDS ${GLSL_OUTPUTS})
    add_dependencies("${target}" "${target}_glsl")
endfunction()

add_subdirectory(src/gl_lib)
add_subdirectory(src/wave_plot)
add_subdirectory(src/ocean_model)
