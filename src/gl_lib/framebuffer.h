#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <memory>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

class framebuffer
{
public:
    unsigned int ID_framebuffer;
    //TODO switch to glm::vec2 size?
    unsigned int width;
    unsigned int height;

    std::shared_ptr<texture> fb_texture;

    framebuffer(unsigned int width, unsigned int height, std::shared_ptr<texture> tex);
    void bind_and_predraw();
    void unbind();
    ~framebuffer();
};

#endif