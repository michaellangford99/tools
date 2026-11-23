#include <cmath>
#include <cstdio>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <implot.h>

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>

#include <gl_lib/base_window.h>
#include <xtensor/core/xmath.hpp>
#include <xtensor/misc/xmanipulation.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <istream>
#include <fstream>
#include <iostream>
#include <xtensor.hpp>
#include <xtensor/containers/xarray.hpp>
#include <xtensor/io/xnpy.hpp>

class dataset
{
    //xtensor container
    std::string name;
};