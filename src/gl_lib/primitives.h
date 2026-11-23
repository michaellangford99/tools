#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <vector>
#include "triangles.h"
#include <memory>

class primitives
{
public:
    static primitives& get()
    {
        static primitives instance;
        return instance;
    }

    primitives(const primitives&) = delete;
    primitives& operator=(const primitives&) = delete;

private:
    primitives() {
        std::vector<vertex> cube_vertices = {
            // Front face
            {{-1, -1,  1}, {0, 0,  1}, {0, 0}, {1, 1, 1}},
            {{ 1, -1,  1}, {0, 0,  1}, {1, 0}, {1, 1, 1}},
            {{ 1,  1,  1}, {0, 0,  1}, {1, 1}, {1, 1, 1}},
            {{-1,  1,  1}, {0, 0,  1}, {0, 1}, {1, 1, 1}},
            
            // Back face
            {{ 1, -1, -1}, {0, 0, -1}, {0, 0}, {1, 1, 1}},
            {{-1, -1, -1}, {0, 0, -1}, {1, 0}, {1, 1, 1}},
            {{-1,  1, -1}, {0, 0, -1}, {1, 1}, {1, 1, 1}},
            {{ 1,  1, -1}, {0, 0, -1}, {0, 1}, {1, 1, 1}},
            
            // Left face
            {{-1, -1, -1}, {-1, 0,  0}, {0, 0}, {1, 1, 1}},
            {{-1, -1,  1}, {-1, 0,  0}, {1, 0}, {1, 1, 1}},
            {{-1,  1,  1}, {-1, 0,  0}, {1, 1}, {1, 1, 1}},
            {{-1,  1, -1}, {-1, 0,  0}, {0, 1}, {1, 1, 1}},
            
            // Right face
            {{ 1, -1,  1}, {1, 0,  0}, {0, 0}, {1, 1, 1}},
            {{ 1, -1, -1}, {1, 0,  0}, {1, 0}, {1, 1, 1}},
            {{ 1,  1, -1}, {1, 0,  0}, {1, 1}, {1, 1, 1}},
            {{ 1,  1,  1}, {1, 0,  0}, {0, 1}, {1, 1, 1}},
            
            // Top face
            {{-1,  1,  1}, {0, 1,  0}, {0, 0}, {1, 1, 1}},
            {{ 1,  1,  1}, {0, 1,  0}, {1, 0}, {1, 1, 1}},
            {{ 1,  1, -1}, {0, 1,  0}, {1, 1}, {1, 1, 1}},
            {{-1,  1, -1}, {0, 1,  0}, {0, 1}, {1, 1, 1}},
            
            // Bottom face
            {{-1, -1, -1}, {0, -1,  0}, {0, 0}, {1, 1, 1}},
            {{ 1, -1, -1}, {0, -1,  0}, {1, 0}, {1, 1, 1}},
            {{ 1, -1,  1}, {0, -1,  0}, {1, 1}, {1, 1, 1}},
            {{-1, -1,  1}, {0, -1,  0}, {0, 1}, {1, 1, 1}}
        };

        for (auto& v : cube_vertices)
            v.position /= 2;

        std::vector<unsigned int> cube_indices = {
            0, 1, 2,  2, 3, 0,   // Front
            4, 5, 6,  6, 7, 4,   // Back
            8, 9, 10, 10, 11, 8, // Left
            12, 13, 14, 14, 15, 12, // Right
            16, 17, 18, 18, 19, 16, // Top
            20, 21, 22, 22, 23, 20  // Bottom
        };

        cube = std::unique_ptr<triangle_geometry>(new triangle_geometry(cube_vertices, cube_indices));

        //-----------------

        glm::vec3 plane_vertices_position[] = {
			// positions
			{1.0f,	1.0f,	0.0f},	// top right
			{1.0f,	-1.0f,	0.0f},	// bottom right
			{-1.0f,	-1.0f,	0.0f},	// bottom left
			{-1.0f,	1.0f,	0.0f}	// top left 
		};

		glm::vec2 plane_vertices_texcoord[] = {
			{1.0f, 1.0f },	// top right
			{1.0f, 0.0f},	// bottom right
			{0.0f, 0.0f},   // bottom left
			{0.0f, 1.0f}    // top left 
		};

		std::vector<vertex> plane_vertices = std::vector<vertex>(4);
		for (int i = 0; i < 4; i++)
			plane_vertices[i] = { plane_vertices_position[i],
								glm::vec3(0,1,0),
								plane_vertices_texcoord[i],
								glm::vec3(1,1,1) };

		std::vector<unsigned int> plane_indices = {
				0, 3, 1, // first triangle
				1, 3, 2  // second triangle
		};

		fullscreen_quad = std::make_unique<triangle_geometry>(plane_vertices, plane_indices);

        //-----------------

        std::vector<vertex> point_vertices = {{{0, 0, 0}, {0, 0,  0}, {0, 0}, {1, 1, 1}}};
    	std::vector<unsigned int> point_indices = {0};

		point_vertex = std::make_unique<triangle_geometry>(point_vertices, point_indices);
    }
    ~primitives() {}

public:
    std::unique_ptr<triangle_geometry> cube;
    std::unique_ptr<triangle_geometry> point_vertex;
    std::unique_ptr<triangle_geometry> fullscreen_quad;
};

#endif
