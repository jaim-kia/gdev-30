/*
 *   Controls:
 *       WASD: movement(W - forward, A - left, S - backward, D - right)
 *       Mouse: move camera perspective
 *       look up/down + W/S: go up or down
 *       1: white light
 *       2: red light
 *       [: decrease brightness
 *       ]: increase brightness
 *       <-: decrease specularity/shininess, divide by 2 per press and release (min 1.0f)
 *       ->: increase specularity/shininess, multiply by 2 per press and release (max 8192.0f)
 *   Note: Light position revolves around the origin, it is in fragment shader since time is already a uniform
 */

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <gdev.h>
#include <vector>
#include <random>
#include <cmath>

// change this to your desired window attributes
#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 360
#define WINDOW_TITLE  "Hello 3D World with Lighting"
GLFWwindow *pWindow;
#define PI 3.14159
#define SQUISH(offset) glm::clamp(2*sin(time + (PI/6) - offset*(PI/3)), 1.0, 2.0)
#define PARTICLE_LIFETIME 4.0f


float width = 1/3.5;
// define a vertex array to hold our vertices

#define YELLOW 1.0f, 1.0f, 0.2f
#define GOLD 0.8f, 0.6431f, 0.0f
#define LYELLOW 1.0f, 1.0f, 0.6f
#define BLACK 0.0f, 0.0f, 0.0f

glm::vec2 rotateVertex(glm::vec2 vertex, float a) {
    glm::mat2 m = glm::mat2(cos(a), sin(a), -sin(a), cos(a));
    return m * vertex;
}

glm::vec3 rotateVertex3D(glm::vec3 vertex, glm::vec3 axis, float a) {
    glm::mat3 m = glm::mat3(0, 0, 0, 0, 0, 0, 0, 0, 0);
    if (axis == glm::vec3(0,0,1)) {
        // roll
        m = glm::mat3(cos(a), sin(a), 0, -sin(a), cos(a), 0, 0, 0, 1);
    } else if (axis == glm::vec3(1,0,0)) {
        // pitch
        m = glm::mat3(1, 0, 0, 0, cos(a), sin(a), 0, -sin(a), cos(a));
    } else {
        // yaw
        m = glm::mat3(cos(a), 0, -sin(a), 0, 1, 0, sin(a), 0, cos(a));
    }
    return m * vertex;
}

float shift(float point) {
    return point/2 + 0.5f;
}

glm::vec3 sv_zero = glm::vec3(0.0f, 0.6f, 0.30f);
glm::vec3 sv_one = glm::vec3(0.35f, 0.35f*tan(54*PI/180), 0.0f);
glm::vec3 sv_two = glm::vec3(0.33f, 0.50f, 0.0f);
glm::vec3 sv_three = glm::vec3(0.05f, 0.88f, 0.0f);
glm::vec3 sv_four = glm::vec3(0.0f, 0.9f, 0.0f);
glm::vec3 sv_five = glm::vec3(-0.05f, 0.88f, 0.0f);
glm::vec3 sv_six = glm::vec3(-0.33f, 0.50f, 0.0f);

glm::vec3 z_axis = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 y_axis = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 sv_mid_zero = glm::vec3(0.0f, 0.3f, 0.4f);


#define ORIGIN(back) rotateVertex3D(glm::vec3(0.00f, 0.00f, 0.4f), y_axis, back)
#define ORIGIN_SIDE(back) ORIGIN(back)[0], ORIGIN(back)[1], ORIGIN(back)[2], LYELLOW, shift(ORIGIN(back)[0]), shift(ORIGIN(back)[1]), 0, 1, 0

#define STAR_VERT(start, a, back) rotateVertex3D(rotateVertex3D(start, z_axis, a*72*PI/180), y_axis, back)
#define STAR_VERTS(start, a, back, color) STAR_VERT(start, a, back)[0], STAR_VERT(start, a, back)[1], STAR_VERT(start, a, back)[2], color, shift(STAR_VERT(start, a, back)[0]), shift(STAR_VERT(start, a, back)[1]), 0, 1, 0

#define STAR_POINTS_ONE(b, back) STAR_VERTS(sv_zero, b, back, YELLOW), STAR_VERTS(sv_one, b, back, YELLOW), STAR_VERTS(sv_two, b, back, YELLOW), STAR_VERTS(sv_zero, b, back, YELLOW), STAR_VERTS(sv_two, b, back, YELLOW), STAR_VERTS(sv_three, b, back, GOLD)
#define STAR_POINTS_TWO(b, back) STAR_VERTS(sv_zero, b, back, YELLOW), STAR_VERTS(sv_three, b, back, GOLD), STAR_VERTS(sv_four, b, back, GOLD), STAR_VERTS(sv_zero, b, back, YELLOW), STAR_VERTS(sv_four, b, back, GOLD), STAR_VERTS(sv_five, b, back, GOLD)
#define STAR_POINTS_THREE(b, back) STAR_VERTS(sv_zero, b, back, YELLOW), STAR_VERTS(sv_five, b, back, GOLD), STAR_VERTS(sv_six, b, back, YELLOW), STAR_VERTS(sv_zero, b, back, YELLOW), STAR_VERTS(sv_six, b, back, YELLOW), STAR_VERTS(sv_one, (b+1), back, YELLOW)

#define STAR_POINT(b, back) STAR_POINTS_ONE(b, back), STAR_POINTS_TWO(b, back), STAR_POINTS_THREE(b, back)

#define INNER_STAR_POINTS(c, back) STAR_VERTS(sv_zero, c, back, YELLOW), STAR_VERTS(sv_one, (c+1), back, YELLOW), STAR_VERTS(sv_zero, (c+1), back, YELLOW)
#define INNER_PENTAGON(c, back) ORIGIN_SIDE(back), STAR_VERTS(sv_mid_zero, c, back, YELLOW), STAR_VERTS(sv_mid_zero, (c+1), back, YELLOW)

#define FILLING(c, back) STAR_VERTS(sv_mid_zero, c, back, YELLOW), STAR_VERTS(sv_zero, c, back, YELLOW), STAR_VERTS(sv_mid_zero, (c+1), back, YELLOW), STAR_VERTS(sv_mid_zero, (c+1), back, YELLOW), STAR_VERTS(sv_zero, c, back, YELLOW), STAR_VERTS(sv_zero, (c+1), back, YELLOW)


// model 1
float vertices[] =
{
    // 30 triangles
    STAR_POINT(0, 0), STAR_POINT(1, 0), STAR_POINT(2, 0), STAR_POINT(3, 0), STAR_POINT(4, 0),
    // 5 triangles
    INNER_STAR_POINTS(0, 0), INNER_STAR_POINTS(1, 0), INNER_STAR_POINTS(2, 0), INNER_STAR_POINTS(3, 0), INNER_STAR_POINTS(4, 0),
    // 5 triangles
    INNER_PENTAGON(0, 0), INNER_PENTAGON(1, 0), INNER_PENTAGON(2, 0), INNER_PENTAGON(3, 0), INNER_PENTAGON(4, 0),
    // 10 triangles
    FILLING(0, 0), FILLING(1, 0), FILLING(2, 0), FILLING(3, 0), FILLING(4, 0),

    // 30 triangles
    STAR_POINT(0, PI), STAR_POINT(1, PI), STAR_POINT(2, PI), STAR_POINT(3, PI), STAR_POINT(4, PI),
    // 5 triangels
    INNER_STAR_POINTS(0, PI), INNER_STAR_POINTS(1, PI), INNER_STAR_POINTS(2, PI), INNER_STAR_POINTS(3, PI), INNER_STAR_POINTS(4, PI),
    // 5 triangles
    INNER_PENTAGON(0, PI), INNER_PENTAGON(1, PI), INNER_PENTAGON(2, PI), INNER_PENTAGON(3, PI), INNER_PENTAGON(4, PI),
    // 10 triangles
    FILLING(0, PI), FILLING(1, PI), FILLING(2, PI), FILLING(3, PI), FILLING(4, PI),

};

float bottomBox[] = {
    // x y z r g b s t nx ny nz
    // bottom pyramid front facing OUT
    -1.5f, -1.5f, 1.5f,     1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(1.0f),   0.0f, -1.0f, 0.0f,
    -1.5f, -1.5f, -1.5f,    1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(-1.0f),  0.0f, -1.0f, 0.0f,
    1.5f, -1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(1.0f), shift(1.0f),    0.0f, -1.0f, 0.0f,

    -1.5f, -1.5f, 1.5f,     1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(-1.0f),  0.0f, 0.0f, 1.0f,
    1.5f, -1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(1.0f), shift(-1.0f),   0.0f, 0.0f, 1.0f,
    -1.5f, 1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(1.0f),   0.0f, 0.0f, 1.0f,

    -1.5f, -1.5f, 1.5f,     1.0f, 1.0f, 1.0f,   shift(1.0f), shift(-1.0f),  -1.0f, 0.0f, 0.0f,
    -1.5f, 1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(1.0f), shift(1.0f),   -1.0f, 0.0f, 0.0f,
    -1.5f, -1.5f, -1.5f,    1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(-1.0f), -1.0f, 0.0f, 0.0f,

    // bottom pyramid front facing IN
    -1.5f, -1.5f, 1.5f,     1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(1.0f),   0.0f, 1.0f, 0.0f,
    1.5f, -1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(1.0f), shift(1.0f),    0.0f, 1.0f, 0.0f,
    -1.5f, -1.5f, -1.5f,    1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(-1.0f),  0.0f, 1.0f, 0.0f,

    -1.5f, -1.5f, 1.5f,     1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(-1.0f),  0.0f, 0.0f, -1.0f,
    -1.5f, 1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(1.0f),   0.0f, 0.0f, -1.0f,
    1.5f, -1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(1.0f), shift(-1.0f),   0.0f, 0.0f, -1.0f,

    -1.5f, -1.5f, 1.5f,     1.0f, 1.0f, 1.0f,   shift(1.0f), shift(-1.0f),  1.0f, 0.0f, 0.0f,
    -1.5f, -1.5f, -1.5f,    1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(-1.0f), 1.0f, 0.0f, 0.0f,
    -1.5f, 1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(1.0f), shift(1.0f),   1.0f, 0.0f, 0.0f,

    // bottom pyramid back facing OUT
    1.5f, -1.5f, -1.5f,     1.0f, 1.0f, 1.0f,   shift(1.0f), shift(-1.0f),   0.0f, -1.0f, 0.0f,
    1.5f, -1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(1.0f), shift(1.0f),    0.0f, -1.0f, 0.0f,
    -1.5f, -1.5f, -1.5f,    1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(-1.0f),  0.0f, -1.0f, 0.0f,

    1.5f, -1.5f, -1.5f,     1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(-1.0f),  0.0f, 0.0f, -1.0f,
    -1.5f, -1.5f, -1.5f,    1.0f, 1.0f, 1.0f,   shift(1.0f), shift(-1.0f),   0.0f, 0.0f, -1.0f,
    1.5f, 1.5f, -1.5f,      1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(1.0f),   0.0f, 0.0f, -1.0f,

    1.5f, -1.5f, -1.5f,     1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(-1.0f),  1.0f, 0.0f, 0.0f,
    1.5f, 1.5f, -1.5f,      1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(1.0f),   1.0f, 0.0f, 0.0f,
    1.5f, -1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(1.0f), shift(-1.0f),   1.0f, 0.0f, 0.0f,

    // bottom pyramid back facing IN
    1.5f, -1.5f, -1.5f,     1.0f, 1.0f, 1.0f,   shift(1.0f), shift(-1.0f),   0.0f, 1.0f, 0.0f,
    -1.5f, -1.5f, -1.5f,    1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(-1.0f),  0.0f, 1.0f, 0.0f,
    1.5f, -1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(1.0f), shift(1.0f),    0.0f, 1.0f, 0.0f,

    1.5f, -1.5f, -1.5f,     1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(-1.0f),  0.0f, 0.0f, 1.0f,
    1.5f, 1.5f, -1.5f,      1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(1.0f),   0.0f, 0.0f, 1.0f,
    -1.5f, -1.5f, -1.5f,    1.0f, 1.0f, 1.0f,   shift(1.0f), shift(-1.0f),   0.0f, 0.0f, 1.0f,

    1.5f, -1.5f, -1.5f,     1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(-1.0f),  -1.0f, 0.0f, 0.0f,
    1.5f, -1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(1.0f), shift(-1.0f),   -1.0f, 0.0f, 0.0f,
    1.5f, 1.5f, -1.5f,      1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(1.0f),   -1.0f, 0.0f, 0.0f,

};

float topBox[] = {
        // upper pyramid front facing OUT
    1.5f, 1.5f, 1.5f,       1.0f, 1.0f, 1.0f,   shift(1.0f), shift(-1.0f),   0.0f, 1.0f, 0.0f,
    1.5f, 1.5f, -1.5f,      1.0f, 1.0f, 1.0f,   shift(1.0f), shift(1.0f),    0.0f, 1.0f, 0.0f,
    -1.5f, 1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(-1.0f),  0.0f, 1.0f, 0.0f,

    1.5f, 1.5f, 1.5f,       1.0f, 1.0f, 1.0f,   shift(1.0f), shift(1.0f),    0.0f, 0.0f, 1.0f,
    -1.5f, 1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(1.0f),   0.0f, 0.0f, 1.0f,
    1.5f, -1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(1.0f), shift(-1.0f),   0.0f, 0.0f, 1.0f,

    1.5f, 1.5f, 1.5f,       1.0f, 1.0f, 1.0f,   shift(1.0f), shift(1.0f),    1.0f, 0.0f, 0.0f,
    1.5f, -1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(1.0f), shift(-1.0f),   1.0f, 0.0f, 0.0f,
    1.5f, 1.5f, -1.5f,      1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(1.0f),   1.0f, 0.0f, 0.0f,

    // upper pyramid front facing IN
    1.5f, 1.5f, 1.5f,       1.0f, 1.0f, 1.0f,   shift(1.0f), shift(-1.0f),   0.0f, -1.0f, 0.0f,
    -1.5f, 1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(-1.0f),  0.0f, -1.0f, 0.0f,
    1.5f, 1.5f, -1.5f,      1.0f, 1.0f, 1.0f,   shift(1.0f), shift(1.0f),    0.0f, -1.0f, 0.0f,

    1.5f, 1.5f, 1.5f,       1.0f, 1.0f, 1.0f,   shift(1.0f), shift(1.0f),    0.0f, 0.0f, -1.0f,
    1.5f, -1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(1.0f), shift(-1.0f),   0.0f, 0.0f, -1.0f,
    -1.5f, 1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(1.0f),   0.0f, 0.0f, -1.0f,

    1.5f, 1.5f, 1.5f,       1.0f, 1.0f, 1.0f,   shift(1.0f), shift(1.0f),    -1.0f, 0.0f, 0.0f,
    1.5f, 1.5f, -1.5f,      1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(1.0f),   -1.0f, 0.0f, 0.0f,
    1.5f, -1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(1.0f), shift(-1.0f),   -1.0f, 0.0f, 0.0f,

    // upper pyramid back facing OUT
    -1.5f, 1.5f, -1.5f,     1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(1.0f),   0.0f, 1.0f, 0.0f,
    -1.5f, 1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(-1.0f),  0.0f, 1.0f, 0.0f,
    1.5f, 1.5f, -1.5f,      1.0f, 1.0f, 1.0f,   shift(1.0f), shift(1.0f),    0.0f, 1.0f, 0.0f,

    -1.5f, 1.5f, -1.5f,     1.0f, 1.0f, 1.0f,   shift(1.0f), shift(1.0f),    0.0f, 0.0f, -1.0f,
    1.5f, 1.5f, -1.5f,      1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(1.0f),   0.0f, 0.0f, -1.0f,
    -1.5f, -1.5f, -1.5f,    1.0f, 1.0f, 1.0f,   shift(1.0f), shift(-1.0f),   0.0f, 0.0f, -1.0f,

    -1.5f, 1.5f, -1.5f,     1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(1.0f),  -1.0f, 0.0f, 0.0f,
    -1.5f, -1.5f, -1.5f,    1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(-1.0f), -1.0f, 0.0f, 0.0f,
    -1.5f, 1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(1.0f), shift(1.0f),   -1.0f, 0.0f, 0.0f,

    // upper pyramid back facing IN
    -1.5f, 1.5f, -1.5f,     1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(1.0f),   0.0f, -1.0f, 0.0f,
    1.5f, 1.5f, -1.5f,      1.0f, 1.0f, 1.0f,   shift(1.0f), shift(1.0f),    0.0f, -1.0f, 0.0f,
    -1.5f, 1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(-1.0f),  0.0f, -1.0f, 0.0f,

    -1.5f, 1.5f, -1.5f,     1.0f, 1.0f, 1.0f,   shift(1.0f), shift(1.0f),    0.0f, 0.0f, 1.0f,
    -1.5f, -1.5f, -1.5f,    1.0f, 1.0f, 1.0f,   shift(1.0f), shift(-1.0f),   0.0f, 0.0f, 1.0f,
    1.5f, 1.5f, -1.5f,      1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(1.0f),   0.0f, 0.0f, 1.0f,

    -1.5f, 1.5f, -1.5f,     1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(1.0f),  1.0f, 0.0f, 0.0f,
    -1.5f, 1.5f, 1.5f,      1.0f, 1.0f, 1.0f,   shift(1.0f), shift(1.0f),   1.0f, 0.0f, 0.0f,
    -1.5f, -1.5f, -1.5f,    1.0f, 1.0f, 1.0f,   shift(-1.0f), shift(-1.0f), 1.0f, 0.0f, 0.0f,
};
// define OpenGL object IDs to represent the vertex array and the shader program in the GPU
GLuint vao, bottomBoxVAO, topBoxVAO;         // vertex array object (stores the render state for our vertex array)
GLuint vbo, bottomBoxVBO, topBoxVBO;         // vertex buffer object (reserves GPU memory for our vertex array)
GLuint shader;      // combined vertex and fragment shader
GLuint texture_eyes, texture_rainbow, texture_fog, texture_sparkle, texture_lucky_block;


glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

int step = 11; // position, color, texCoord, normal
int vertexCount = sizeof(vertices)/ (step * sizeof(float));
int bottomBoxVertexCount = sizeof(bottomBox)/ (step * sizeof(float));
int topBoxVertexCount = sizeof(topBox)/ (step * sizeof(float));

// Specularity
float specularity = 4096.0f;

// light color
int lightIndex = 3;
glm::vec3 whiteLight[] = {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.25f, 0.25f, 0.25f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.75f, 0.75f, 0.75f), glm::vec3(1.0f, 1.0f, 1.0f)};
glm::vec3 redLight[] = {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.25f, 0.0f, 0.0f), glm::vec3(0.5f, 0.0f, 0.0f), glm::vec3(0.75f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)};
glm::vec3 currentLight[] = {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.25f, 0.25f, 0.25f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.75f, 0.75f, 0.75f), glm::vec3(1.0f, 1.0f, 1.0f)};
glm::vec3 lightColor = currentLight[lightIndex];

bool firstMouse = true;
bool leftKeyPressed = false;
bool rightKeyPressed = false;
bool leftBracketPressed = false;
bool rightBracketPressed = false;
bool startAnim = false;
float yaw   = -90.0f;
float pitch =  0.0f;
float lastX =  640 / 2.0;
float lastY =  360 / 2.0;
float fov   =  45.0f;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

GLuint particleVAO, particleVBO;
GLuint particleSparkleVAO, particleSparkleVBO;
GLuint particleShader;
const float PARTICLE_SIZE = 0.1f;

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 color;
    float life;
    float opacity;
    float size;
    float rotation;
    float rotationSpeed;
};

std::vector<Particle> fogParticles;
std::vector<Particle> sparkleParticles;
std::vector<glm::vec3> sparkleSpawn(5, glm::vec3(0.0f, 0.0f, 0.0f));;
float lastFogParticleTime = 0.0f;
float lastSparkleParticleTime = 0.0f;
float fogScale = 2.0f;

// using c++ random:
std::default_random_engine generator;
std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
std::uniform_int_distribution<int> spawnDist(0, sparkleSpawn.size() - 1);


// function for creating particles
void createParticles(std::vector<Particle>& particles, float& lastParticleTime, float currentTime, int max_particles, float multiplier, float opacity, glm::vec3 color, glm::vec3 spawn) {
    if (currentTime - lastParticleTime > 0.05f) {    // spawn particles every indicated difference
        lastParticleTime = currentTime;             // update the time when the last particle was spawned
        
        // create n new particles
        for (int i = 0; i < 5; i++) {
            
            // if current vector size is less than indicated max size, spawn particle
            if ((int) particles.size() < max_particles) {
                Particle p;


                p.position = spawn;
                p.velocity = glm::vec3(
                    distribution(generator),
                    distribution(generator),
                    distribution(generator)
                ) * multiplier;
                // float gray = 0.9f + distribution(generator) * 0.1f;
                p.opacity = opacity;
                p.color = color;
                p.life = PARTICLE_LIFETIME;
                p.size = PARTICLE_SIZE * (2.0f + distribution(generator) * 0.4f);
                p.rotation = distribution(generator) * 2.0f * PI;
                p.rotationSpeed = distribution(generator) * 2.0f;

                // place at the end of the vector
                particles.push_back(p);
            }
        }
    }
}

void updateParticles(std::vector<Particle>& particles, float deltaTime, bool enableY) {
    for (size_t i = 0; i < particles.size(); ) {

        // if the particle is not dead, update the life, position and velocity
        particles[i].life -= deltaTime;
        particles[i].position += particles[i].velocity * deltaTime;
        if (!enableY) {
            particles[i].velocity.y = 0;
        }
        particles[i].rotation += particles[i].rotationSpeed * deltaTime;
        
        // if the particle is dead, place it at the back and remove it from the vector particles
        // since we moved it to the back, we don't need to i++
        // pop_back updates the particle.size
        if (particles[i].life <= 0.0f) {
            particles[i] = particles.back();
            particles.pop_back();
        } else {
            // if it is alive, move to the next index
            i++;
        }
    }
}

void getNormal(float* verts, int count, int step){
    for(int i = 0; i < count; i += 3){ //3 since 3 vertices
        glm::vec3 A = glm::vec3(verts[i*step], verts[i*step + 1], verts[i*step +2]);
        glm::vec3 B = glm::vec3(verts[(i + 1)*step], verts[(i + 1)*step + 1], verts[(i + 1)*step +2]);
        glm::vec3 C = glm::vec3(verts[(i + 2)*step], verts[(i + 2)*step + 1], verts[(i + 2)*step +2]);

        glm::vec3 U = B - A;
        glm::vec3 V = C - A;
        glm::vec3 normal = glm::normalize(glm::cross(U,V));

        for(int j = 0; j < 3; ++j){
            verts[(i + j) * step + (step-3)] = normal.x ;
            verts[(i + j) * step + (step-2)] = normal.y ;
            verts[(i + j) * step + (step-1)] = normal.z ;
        }
         //Testing
         // std::cout << normal.x << ", ";
         // std::cout << normal.y << ", ";
         // std::cout << normal.z << std::endl;
    }

    for (int i = 0; i < count; i++) {
        // vertex's position to compare
        glm::vec3 currentVertex(verts[i * step], verts[i * step + 1], verts[i * step + 2]);
        glm::vec3 totalNormal(0, 0, 0);
        glm::vec3 finalNormal(0, 0, 0);
        // vertices that share a position
        int totalVertices = 0;
        
        for (int j = 0; j < count; j++) {
            // vertex compared to current vertex
            glm::vec3 compVertex(verts[j * step], verts[j * step + 1], verts[j * step + 2]);
            // threshhold of considering "the same position"
            if (glm::distance(currentVertex, compVertex) <= 0.001f) {
                totalNormal += glm::vec3(verts[j * step + (step-3)], verts[j * step + (step-2)], verts[j * step + (step-1)]);
                totalVertices++;
            }
        }
        
        // average, normalize and set the normal to the vertex position compared
        if (totalVertices > 0) {
            finalNormal = glm::normalize(totalNormal / (float)totalVertices);
            verts[i*step + (step-3)] = finalNormal.x;
            verts[i*step + (step-2)] = finalNormal.y;
            verts[i*step + (step-1)] = finalNormal.z;
        }
    }
}

// 3d particle
float particleShape[] = {

    // Front face (facing +Z)
    0.0f,  0.0f,  2.0f,  shift(0.0f), shift(0.0f), 0, 1, 0,
    1.0f,  1.0f,  1.0f,  shift(1.0f), shift(1.0f), 0, 1, 0,
    -1.0f,  1.0f,  1.0f,  shift(-1.0f), shift(1.0f), 0, 1, 0,

    0.0f,  0.0f,  2.0f,  shift(0.0f), shift(0.0f), 0, 1, 0,
    -1.0f,  1.0f,  1.0f,  shift(-1.0f), shift(1.0f), 0, 1, 0,
    -1.0f, -1.0f,  1.0f,  shift(-1.0f), shift(-1.0f), 0, 1, 0,

    0.0f,  0.0f,  2.0f,  shift(0.0f), shift(0.0f), 0, 1, 0,
    -1.0f, -1.0f,  1.0f,  shift(-1.0f), shift(-1.0f), 0, 1, 0,
    1.0f, -1.0f,  1.0f,  shift(1.0f), shift(-1.0f), 0, 1, 0,

    0.0f,  0.0f,  2.0f,  shift(0.0f), shift(0.0f), 0, 1, 0,
    1.0f, -1.0f,  1.0f,  shift(1.0f), shift(-1.0f), 0, 1, 0,
    1.0f,  1.0f,  1.0f,  shift(1.0f), shift(1.0f), 0, 1, 0,


    // Back face (facing -Z)
    0.0f, 0.0f, -2.0f,  shift(0.0f), shift(0.0f), 0, 1, 0,
    -1.0f,  1.0f, -1.0f,  shift(-1.0f), shift(1.0f), 0, 1, 0,
    1.0f,  1.0f, -1.0f,  shift(1.0f), shift(1.0f), 0, 1, 0,

    0.0f, 0.0f, -2.0f,  shift(0.0f), shift(0.0f), 0, 1, 0,
    1.0f,  1.0f, -1.0f,  shift(1.0f), shift(1.0f), 0, 1, 0,
    1.0f, -1.0f, -1.0f,  shift(1.0f), shift(-1.0f), 0, 1, 0,

    0.0f, 0.0f, -2.0f,  shift(0.0f), shift(0.0f), 0, 1, 0,
    1.0f, -1.0f, -1.0f,  shift(1.0f), shift(-1.0f), 0, 1, 0,
    -1.0f, -1.0f, -1.0f,  shift(-1.0f), shift(-1.0f), 0, 1, 0,

    0.0f, 0.0f, -2.0f,  shift(0.0f), shift(0.0f), 0, 1, 0,
    -1.0f, -1.0f, -1.0f,  shift(-1.0f), shift(-1.0f), 0, 1, 0,
    -1.0f,  1.0f, -1.0f,  shift(-1.0f), shift(1.0f), 0, 1, 0,


    // Left face (facing -X)
    -2.0f,  0.0f,  0.0f,  shift(-2.0f), shift(0.0f), 0, 1, 0,
    -1.0f, -1.0f,  1.0f,  shift(1.0f), shift(-1.0f), 0, 1, 0,
    -1.0f,  1.0f,  1.0f,  shift(1.0f), shift(1.0f), 0, 1, 0,

    -2.0f,  0.0f,  0.0f,  shift(-2.0f), shift(0.0f), 0, 1, 0,
    -1.0f, -1.0f, -1.0f,  shift(-1.0f), shift(-1.0f), 0, 1, 0,
    -1.0f, -1.0f,  1.0f,  shift(1.0f), shift(-1.0f), 0, 1, 0,

    -2.0f,  0.0f,  0.0f,  shift(-2.0f), shift(0.0f), 0, 1, 0,
    -1.0f,  1.0f, -1.0f,  shift(-1.0f), shift(1.0f), 0, 1, 0,
    -1.0f, -1.0f, -1.0f,  shift(-1.0f), shift(-1.0f), 0, 1, 0,

    -2.0f,  0.0f,  0.0f,  shift(-2.0f), shift(0.0f), 0, 1, 0,
    -1.0f,  1.0f,  1.0f,  shift(1.0f), shift(1.0f), 0, 1, 0,
    -1.0f,  1.0f, -1.0f,  shift(-1.0f), shift(1.0f), 0, 1, 0,


    // Right face (facing +X)
    2.0f,  0.0f,  0.0f,  shift(2.0f), shift(0.0f), 0, 1, 0,
    1.0f,  1.0f,  1.0f,  shift(1.0f), shift(1.0f), 0, 1, 0,
    1.0f, -1.0f,  1.0f,  shift(1.0f), shift(-1.0f), 0, 1, 0,

    2.0f,  0.0f,  0.0f,  shift(2.0f), shift(0.0f), 0, 1, 0,
    1.0f,  1.0f, -1.0f,  shift(-1.0f), shift(1.0f), 0, 1, 0,
    1.0f,  1.0f,  1.0f,  shift(1.0f), shift(1.0f), 0, 1, 0,

    2.0f,  0.0f,  0.0f,  shift(2.0f), shift(0.0f), 0, 1, 0,
    1.0f, -1.0f, -1.0f,  shift(-1.0f), shift(-1.0f), 0, 1, 0,
    1.0f,  1.0f, -1.0f,  shift(-1.0f), shift(1.0f), 0, 1, 0,

    2.0f,  0.0f,  0.0f,  shift(2.0f), shift(0.0f), 0, 1, 0,
    1.0f, -1.0f,  1.0f,  shift(1.0f), shift(-1.0f), 0, 1, 0,
    1.0f, -1.0f, -1.0f,  shift(-1.0f), shift(-1.0f), 0, 1, 0,


    // // Top face (facing +Y)
    0.0f,  2.0f,  0.0f,  shift(0.0f), shift(2.0f), 0, 1, 0,
    -1.0f,  1.0f,  1.0f,  shift(-1.0f), shift(1.0f), 0, 1, 0,
    1.0f,  1.0f,  1.0f,  shift(1.0f), shift(1.0f), 0, 1, 0,

    0.0f,  2.0f,  0.0f,  shift(0.0f), shift(2.0f), 0, 1, 0,
    -1.0f,  1.0f, -1.0f,  shift(-1.0f), shift(-1.0f), 0, 1, 0,
    -1.0f,  1.0f,  1.0f,  shift(-1.0f), shift(1.0f), 0, 1, 0,

    0.0f,  2.0f,  0.0f,  shift(0.0f), shift(2.0f), 0, 1, 0,
    1.0f,  1.0f, -1.0f,  shift(1.0f), shift(-1.0f), 0, 1, 0,
    -1.0f,  1.0f, -1.0f,  shift(-1.0f), shift(-1.0f), 0, 1, 0,

    0.0f,  2.0f,  0.0f,  shift(0.0f), shift(2.0f), 0, 1, 0,
    1.0f,  1.0f,  1.0f,  shift(1.0f), shift(1.0f), 0, 1, 0,
    1.0f,  1.0f, -1.0f,  shift(1.0f), shift(-1.0f), 0, 1, 0,


    // Bottom face (facing -Y)
    0.0f,  -2.0f,  0.0f,  shift(0.0f), shift(-2.0f), 0, 1, 0,
    1.0f, -1.0f, -1.0f,  shift(1.0f), shift(-1.0f), 0, 1, 0,
    1.0f, -1.0f,  1.0f,  shift(1.0f), shift(1.0f), 0, 1, 0,

    0.0f,  -2.0f,  0.0f,  shift(0.0f), shift(-2.0f), 0, 1, 0,
    -1.0f, -1.0f, -1.0f,  shift(-1.0f), shift(-1.0f), 0, 1, 0,
    1.0f, -1.0f, -1.0f,  shift(1.0f), shift(-1.0f), 0, 1, 0,

    0.0f,  -2.0f,  0.0f,  shift(0.0f), shift(-2.0f), 0, 1, 0,
    -1.0f, -1.0f,  1.0f,  shift(-1.0f), shift(1.0f), 0, 1, 0,
    -1.0f, -1.0f, -1.0f,  shift(-1.0f), shift(-1.0f), 0, 1, 0,

    0.0f,  -2.0f,  0.0f,  shift(0.0f), shift(-2.0f), 0, 1, 0,
    1.0f, -1.0f,  1.0f,  shift(1.0f), shift(1.0f), 0, 1, 0,
    -1.0f, -1.0f,  1.0f,  shift(-1.0f), shift(1.0f), 0, 1, 0,
};

// sparkle particle
float particleSparkleShape[] = {

    // Front face (facing +Z)
    0.0f,  0.0f,  2.0f,  shift(0.0f), shift(0.0f), 0, 1, 0,
    0.5f,  0.5f,  0.5f,  shift(0.5f), shift(0.5f), 0, 1, 0,
    -0.5f,  0.5f,  0.5f,  shift(-0.5f), shift(0.5f), 0, 1, 0,

    0.0f,  0.0f,  2.0f,  shift(0.0f), shift(0.0f), 0, 1, 0,
    -0.5f,  0.5f,  0.5f,  shift(-0.5f), shift(0.5f), 0, 1, 0,
    -0.5f, -0.5f,  0.5f,  shift(-0.5f), shift(-0.5f), 0, 1, 0,

    0.0f,  0.0f,  2.0f,  shift(0.0f), shift(0.0f), 0, 1, 0,
    -0.5f, -0.5f,  0.5f,  shift(-0.5f), shift(-0.5f), 0, 1, 0,
    0.5f, -0.5f,  0.5f,  shift(0.5f), shift(-0.5f), 0, 1, 0,

    0.0f,  0.0f,  2.0f,  shift(0.0f), shift(0.0f), 0, 1, 0,
    0.5f, -0.5f,  0.5f,  shift(0.5f), shift(-0.5f), 0, 1, 0,
    0.5f,  0.5f,  0.5f,  shift(0.5f), shift(0.5f), 0, 1, 0,


    // Back face (facing -Z)
    0.0f, 0.0f, -2.0f,  shift(0.0f), shift(0.0f), 0, 1, 0,
    -0.5f,  0.5f, -0.5f,  shift(-0.5f), shift(0.5f), 0, 1, 0,
    0.5f,  0.5f, -0.5f,  shift(0.5f), shift(0.5f), 0, 1, 0,

    0.0f, 0.0f, -2.0f,  shift(0.0f), shift(0.0f), 0, 1, 0,
    0.5f,  0.5f, -0.5f,  shift(0.5f), shift(0.5f), 0, 1, 0,
    0.5f, -0.5f, -0.5f,  shift(0.5f), shift(-0.5f), 0, 1, 0,

    0.0f, 0.0f, -2.0f,  shift(0.0f), shift(0.0f), 0, 1, 0,
    0.5f, -0.5f, -0.5f,  shift(0.5f), shift(-0.5f), 0, 1, 0,
    -0.5f, -0.5f, -0.5f,  shift(-0.5f), shift(-0.5f), 0, 1, 0,

    0.0f, 0.0f, -2.0f,  shift(0.0f), shift(0.0f), 0, 1, 0,
    -0.5f, -0.5f, -0.5f,  shift(-0.5f), shift(-0.5f), 0, 1, 0,
    -0.5f,  0.5f, -0.5f,  shift(-0.5f), shift(0.5f), 0, 1, 0,


    // Left face (facing -X)
    -2.0f,  0.0f,  0.0f,  shift(-2.0f), shift(0.0f), 0, 1, 0,
    -0.5f, -0.5f,  0.5f,  shift(0.5f), shift(-0.5f), 0, 1, 0,
    -0.5f,  0.5f,  0.5f,  shift(0.5f), shift(0.5f), 0, 1, 0,

    -2.0f,  0.0f,  0.0f,  shift(-2.0f), shift(0.0f), 0, 1, 0,
    -0.5f, -0.5f, -0.5f,  shift(-0.5f), shift(-0.5f), 0, 1, 0,
    -0.5f, -0.5f,  0.5f,  shift(0.5f), shift(-0.5f), 0, 1, 0,

    -2.0f,  0.0f,  0.0f,  shift(-2.0f), shift(0.0f), 0, 1, 0,
    -0.5f,  0.5f, -0.5f,  shift(-0.5f), shift(0.5f), 0, 1, 0,
    -0.5f, -0.5f, -0.5f,  shift(-0.5f), shift(-0.5f), 0, 1, 0,

    -2.0f,  0.0f,  0.0f,  shift(-2.0f), shift(0.0f), 0, 1, 0,
    -0.5f,  0.5f,  0.5f,  shift(0.5f), shift(0.5f), 0, 1, 0,
    -0.5f,  0.5f, -0.5f,  shift(-0.5f), shift(0.5f), 0, 1, 0,


    // Right face (facing +X)
    2.0f,  0.0f,  0.0f,  shift(2.0f), shift(0.0f), 0, 1, 0,
    0.5f,  0.5f,  0.5f,  shift(0.5f), shift(0.5f), 0, 1, 0,
    0.5f, -0.5f,  0.5f,  shift(0.5f), shift(-0.5f), 0, 1, 0,

    2.0f,  0.0f,  0.0f,  shift(2.0f), shift(0.0f), 0, 1, 0,
    0.5f,  0.5f, -0.5f,  shift(-0.5f), shift(0.5f), 0, 1, 0,
    0.5f,  0.5f,  0.5f,  shift(0.5f), shift(0.5f), 0, 1, 0,

    2.0f,  0.0f,  0.0f,  shift(2.0f), shift(0.0f), 0, 1, 0,
    0.5f, -0.5f, -0.5f,  shift(-0.5f), shift(-0.5f), 0, 1, 0,
    0.5f,  0.5f, -0.5f,  shift(-0.5f), shift(0.5f), 0, 1, 0,

    2.0f,  0.0f,  0.0f,  shift(2.0f), shift(0.0f), 0, 1, 0,
    0.5f, -0.5f,  0.5f,  shift(0.5f), shift(-0.5f), 0, 1, 0,
    0.5f, -0.5f, -0.5f,  shift(-0.5f), shift(-0.5f), 0, 1, 0,


    // // Top face (facing +Y)
    0.0f,  2.0f,  0.0f,  shift(0.0f), shift(2.0f), 0, 1, 0,
    -0.5f,  0.5f,  0.5f,  shift(-0.5f), shift(0.5f), 0, 1, 0,
    0.5f,  0.5f,  0.5f,  shift(0.5f), shift(0.5f), 0, 1, 0,

    0.0f,  2.0f,  0.0f,  shift(0.0f), shift(2.0f), 0, 1, 0,
    -0.5f,  0.5f, -0.5f,  shift(-0.5f), shift(-0.5f), 0, 1, 0,
    -0.5f,  0.5f,  0.5f,  shift(-0.5f), shift(0.5f), 0, 1, 0,

    0.0f,  2.0f,  0.0f,  shift(0.0f), shift(2.0f), 0, 1, 0,
    0.5f,  0.5f, -0.5f,  shift(0.5f), shift(-0.5f), 0, 1, 0,
    -0.5f,  0.5f, -0.5f,  shift(-0.5f), shift(-0.5f), 0, 1, 0,

    0.0f,  2.0f,  0.0f,  shift(0.0f), shift(2.0f), 0, 1, 0,
    0.5f,  0.5f,  0.5f,  shift(0.5f), shift(0.5f), 0, 1, 0,
    0.5f,  0.5f, -0.5f,  shift(0.5f), shift(-0.5f), 0, 1, 0,


    // Bottom face (facing -Y)
    0.0f,  -2.0f,  0.0f,  shift(0.0f), shift(-2.0f), 0, 1, 0,
    0.5f, -0.5f, -0.5f,  shift(0.5f), shift(-0.5f), 0, 1, 0,
    0.5f, -0.5f,  0.5f,  shift(0.5f), shift(0.5f), 0, 1, 0,

    0.0f,  -2.0f,  0.0f,  shift(0.0f), shift(-2.0f), 0, 1, 0,
    -0.5f, -0.5f, -0.5f,  shift(-0.5f), shift(-0.5f), 0, 1, 0,
    0.5f, -0.5f, -0.5f,  shift(0.5f), shift(-0.5f), 0, 1, 0,

    0.0f,  -2.0f,  0.0f,  shift(0.0f), shift(-2.0f), 0, 1, 0,
    -0.5f, -0.5f,  0.5f,  shift(-0.5f), shift(0.5f), 0, 1, 0,
    -0.5f, -0.5f, -0.5f,  shift(-0.5f), shift(-0.5f), 0, 1, 0,

    0.0f,  -2.0f,  0.0f,  shift(0.0f), shift(-2.0f), 0, 1, 0,
    0.5f, -0.5f,  0.5f,  shift(0.5f), shift(0.5f), 0, 1, 0,
    -0.5f, -0.5f,  0.5f,  shift(-0.5f), shift(0.5f), 0, 1, 0,
};
// called by the main function to do initial setup, such as uploading vertex
// arrays, shader programs, etc.; returns true if successful, false otherwise
bool setup()
{

    // generate the VAO and VBO objects and store their IDs in vao and vbo, respectively
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    // bind the newly-created VAO to make it the current one that OpenGL will apply state changes to
    glBindVertexArray(vao);

    // upload our vertex array data to the newly-created VBO
    getNormal(vertices, vertexCount, step);
    getNormal(particleShape, sizeof(particleShape)/ (8 * sizeof(float)), 8);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // on the VAO, register the current VBO with the following vertex attribute layout:
    // - layout location 0...
    // - ... shall consist of 3 GL_FLOATs (corresponding to x, y, and z coordinates)
    // - ... its values will NOT be normalized (GL_FALSE)
    // - ... the stride length is the number of bytes of all 3 floats of each vertex (hence, 3 * sizeof(float))
    // - ... and we start at the beginning of the array (hence, (void*) 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*) (3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*) (6 * sizeof(float)));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*) (8 * sizeof(float)));

    // enable the newly-created layout location 0;
    // this shall be used by our vertex shader to read the vertex's x, y, and z
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    // bottom box vao and vbo
    glGenVertexArrays(1, &bottomBoxVAO);
    glGenBuffers(1, &bottomBoxVBO);

    glBindVertexArray(bottomBoxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, bottomBoxVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(bottomBox), bottomBox, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*) (3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*) (6 * sizeof(float)));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*) (8 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    // top box vao and vbo
    glGenVertexArrays(1, &topBoxVAO);
    glGenBuffers(1, &topBoxVBO);

    glBindVertexArray(topBoxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, topBoxVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(topBox), topBox, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*) (3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*) (6 * sizeof(float)));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*) (8 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    // important: if you have more vertex arrays to draw, make sure you separately define them
    // with unique VAO and VBO IDs, and follow the same process above to upload them to the GPU

    // load our shader program
    shader = gdevLoadShader("exer4.vs", "exer4.fs");
    if (! shader)
        return false;

    texture_eyes = gdevLoadTexture("texture_eyes.png", GL_REPEAT, true, true);
    if (! texture_eyes) return false;

    texture_rainbow = gdevLoadTexture("texture_rainbow.png", GL_REPEAT, true, true);
    if (! texture_rainbow) return false;

    texture_fog = gdevLoadTexture("texture_fog.png", GL_REPEAT, true, true);
    if (! texture_fog) return false;

    texture_sparkle = gdevLoadTexture("texture_sparkle.png", GL_REPEAT, true, true);
    if (! texture_sparkle) return false;

    texture_lucky_block = gdevLoadTexture("texture_lucky_block.png", GL_REPEAT, true, true);
    if (! texture_lucky_block) return false;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // different set of shaders for particles
    particleShader = gdevLoadShader("particle.vs", "particle.fs");
    if (!particleShader) return false;

    // vao and vbo of the particles
    glGenVertexArrays(1, &particleVAO);
    glGenBuffers(1, &particleVBO);

    glBindVertexArray(particleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particleShape), particleShape, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // sparkles
    glGenVertexArrays(1, &particleSparkleVAO);
    glGenBuffers(1, &particleSparkleVBO);

    glBindVertexArray(particleSparkleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, particleSparkleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particleSparkleShape), particleSparkleShape, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    return true;
}

// called by the main function to do rendering per frame
void render()
{
    // clear the whole frame
    glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // using our shader program...
    glUseProgram(shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_eyes);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_rainbow);

    glEnable(GL_DEPTH_TEST);

    float currentFrame = glfwGetTime();
    float time = currentFrame;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // ------------------------ stars ------------------------
    glm::mat4 projectionViewMatrix;
    projectionViewMatrix = glm::perspective(glm::radians(60.0f), 
                            (float) WINDOW_WIDTH / WINDOW_HEIGHT,
                            0.1f,
                            100.0f);
    projectionViewMatrix *= glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projectionViewMatrix"), 1, GL_FALSE, glm::value_ptr(projectionViewMatrix));

    glBindVertexArray(vao);
    // modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, -1.0f));

    glUniform1i(glGetUniformLocation(shader, "objectType"), 0);
    glUniform1i(glGetUniformLocation(shader, "mainTexture"), 0);
    glUniform1i(glGetUniformLocation(shader, "shaderRainbow"), 1);

    glUniform1f(glGetUniformLocation(shader, "time"), currentFrame);
    glUniform1f(glGetUniformLocation(shader, "specularity"), specularity);
    glUniform3f(glGetUniformLocation(shader, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
    glUniform3f(glGetUniformLocation(shader, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

    if (startAnim) {
        
        static float startTime = glfwGetTime();
        float currentTime = glfwGetTime() - startTime;
        
        // float moveX = glm::min(currentTime * 3.0f, 5.0f);
        // float moveY = -pow((0.4 * moveX - 1), 2) + 1;
        // modelMatrix = glm::translate(modelMatrix, glm::vec3(moveX, moveY, 0));

        fogScale = glm::min(2.0f + currentTime, 4.0f);

        for (int i = 0; i < 5; i ++) {
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            glm::mat4 testMat = glm::mat4(1.0f);
            float moveX = glm::min(currentTime * 3.0f, 5.0f);
            float moveY = -pow((0.4 * moveX - 1), 2) + 1;
            
            modelMatrix = glm::scale(modelMatrix, glm::vec3(glm::min(currentTime * 2.0f, 1.0f)));

            glm::vec4 movement = glm::vec4(moveX, moveY, 0.0f, 1.0f);
            testMat = glm::rotate(modelMatrix, i * glm::radians(-72.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            movement = movement * testMat;

            sparkleSpawn[i] = glm::vec3(movement.x, movement.y, movement.z);

            modelMatrix = glm::rotate(modelMatrix, sin(currentTime)/2, glm::vec3(movement.x, movement.y, movement.z));
            modelMatrix = glm::rotate(modelMatrix, sin(-currentTime)/4, glm::vec3(0, 1, 0));

            modelMatrix = glm::translate(modelMatrix, glm::vec3(movement.x, movement.y, movement.z));
            modelMatrix = glm::rotate(modelMatrix, (i + 5) * glm::radians(72.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));


            glm::mat4 normalMatrix;
            normalMatrix = glm::transpose(glm::inverse(modelMatrix));
            glUniformMatrix4fv(glGetUniformLocation(shader, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
            glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
            glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / (11 * sizeof(float)));
        }
    }

    // ------------------------ bottom box ------------------------
    glBindVertexArray(bottomBoxVAO);
    glm::mat4 bottomBoxModelMatrix = glm::mat4(1.0f);
    bottomBoxModelMatrix = glm::scale(bottomBoxModelMatrix, glm::vec3(0.5f)); // Scale down the box

    glm::mat4 bottomBoxNormalMatrix = glm::transpose(glm::inverse(bottomBoxModelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(bottomBoxNormalMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(bottomBoxModelMatrix));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_lucky_block);

    glUniform1i(glGetUniformLocation(shader, "mainTexture"), 0);
    glUniform1i(glGetUniformLocation(shader, "shaderRainbow"), 1);
    glUniform1i(glGetUniformLocation(shader, "objectType"), 1);
    glUniform1f(glGetUniformLocation(shader, "time"), currentFrame);
    glUniform1f(glGetUniformLocation(shader, "specularity"), specularity);
    glUniform3f(glGetUniformLocation(shader, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
    glUniform3f(glGetUniformLocation(shader, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

    glDrawArrays(GL_TRIANGLES, 0, bottomBoxVertexCount);

    // ------------------------ top box ------------------------
    glBindVertexArray(topBoxVAO);
    glm::mat4 topBoxModelMatrix = glm::mat4(1.0f);
    topBoxModelMatrix = glm::scale(topBoxModelMatrix, glm::vec3(0.5f)); // Scale down the box

    glm::mat4 topBoxNormalMatrix = glm::transpose(glm::inverse(topBoxModelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(topBoxNormalMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(topBoxModelMatrix));

    glDrawArrays(GL_TRIANGLES, 0, topBoxVertexCount);


    // ------------------------ particles ------------------------

    // create and update particles:
    // time for difference
    float theta = distribution(generator) * 2.0f * PI;
    float opacity = 0.7f + distribution(generator) * 0.3f;
    glm::vec3 color = glm::vec3(0.9f + distribution(generator) * 0.1f);
    glm::vec3 spawn = glm::vec3(fogScale * cos(theta), -1.0f, fogScale * sin(theta));

    createParticles(fogParticles, lastFogParticleTime, time, 500, 0.3f, opacity, color, spawn);
    createParticles(sparkleParticles, lastSparkleParticleTime, time, 50, 1.0f, 1.0f, color, sparkleSpawn[spawnDist(generator)]);

    // deltaTime so the update is consistent in all devices
    updateParticles(fogParticles, deltaTime, false);
    updateParticles(sparkleParticles, deltaTime, true);

    glUseProgram(particleShader);
    glUniformMatrix4fv(glGetUniformLocation(particleShader, "projectionViewMatrix"), 1, GL_FALSE, glm::value_ptr(projectionViewMatrix));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_fog); // particle texture

    glUniform1i(glGetUniformLocation(particleShader, "particleTexture"), 0);
    glUniform1f(glGetUniformLocation(particleShader, "time"), currentFrame);
    glUniform3f(glGetUniformLocation(particleShader, "lightColor"), lightColor.x, lightColor.y, lightColor.z);

    glm::mat4 normalMatrix;

    // render particles
    for (const auto& p : fogParticles) {

        // for fading
        float lifeRatio = p.life / PARTICLE_LIFETIME;
        float currentOpacity = p.opacity * lifeRatio;  // Fade from initial alpha to 0

        glUniform1f(glGetUniformLocation(particleShader, "opacity"), currentOpacity);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, p.position);
        model = glm::rotate(model, p.rotation, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(p.size * lifeRatio));
        
        glUniformMatrix4fv(glGetUniformLocation(particleShader, "modelMatrix"), 
                        1, GL_FALSE, glm::value_ptr(model));
        normalMatrix = glm::transpose(glm::inverse(model));
        glUniformMatrix4fv(glGetUniformLocation(particleShader, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

        glUniform3f(glGetUniformLocation(particleShader, "acolor"), 
                p.color.r, 
                p.color.g, 
                p.color.b);
        glBindVertexArray(particleVAO);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(particleShape) / (8 * sizeof(float)));
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_sparkle); // sparkle particle texture
    glUniform1i(glGetUniformLocation(particleShader, "particleTexture"), 0);

    for (const auto& p : sparkleParticles) {

        // for fading
        float lifeRatio = p.life / PARTICLE_LIFETIME;
        float currentOpacity = p.opacity * lifeRatio;  // Fade from initial alpha to 0

        glUniform1f(glGetUniformLocation(particleShader, "opacity"), currentOpacity);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, p.position);
        model = glm::rotate(model, p.rotation, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(p.size * lifeRatio));
        
        glUniformMatrix4fv(glGetUniformLocation(particleShader, "modelMatrix"), 
                        1, GL_FALSE, glm::value_ptr(model));
        normalMatrix = glm::transpose(glm::inverse(model));
        glUniformMatrix4fv(glGetUniformLocation(particleShader, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

        glUniform3f(glGetUniformLocation(particleShader, "acolor"), 
                p.color.r, 
                p.color.g, 
                p.color.b);
        glBindVertexArray(particleSparkleVAO);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(particleSparkleShape) / (8 * sizeof(float)));
    }

    glEnable(GL_CULL_FACE);
}

void processInput(GLFWwindow *window)
{

    float cameraSpeed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        startAnim = true;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && !leftKeyPressed) {
        specularity = glm::max(specularity / 2.0f, 1.0f);
        leftKeyPressed = true;
    } else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE)
        leftKeyPressed = false;

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && !rightKeyPressed) {
        specularity = glm::min(specularity * 2.0f, 8192.0f);
        rightKeyPressed = true;
    } else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE)
        rightKeyPressed = false;

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        for (int i = 0; i < 5; i++) currentLight[i] = whiteLight[i];
        lightColor = currentLight[lightIndex];
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        for (int i = 0; i < 5; i++) currentLight[i] = redLight[i];
        lightColor = currentLight[lightIndex];
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS && !leftBracketPressed) {
        lightIndex = glm::max(lightIndex - 1, 0);
        lightColor = currentLight[lightIndex];
        leftBracketPressed = true;
    } else if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_RELEASE)
        leftBracketPressed = false;

    if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS && !rightBracketPressed) {
        lightIndex = glm::min(lightIndex + 1, 4);
        lightColor = currentLight[lightIndex];
        rightBracketPressed = true;
    } else if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_RELEASE)
        rightBracketPressed = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}  

/*****************************************************************************/

// handler called by GLFW when there is a keyboard event
void handleKeys(GLFWwindow* pWindow, int key, int scancode, int action, int mode)
{
    // pressing Esc closes the window
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(pWindow, GL_TRUE);

}

// handler called by GLFW when the window is resized
void handleResize(GLFWwindow* pWindow, int width, int height)
{
    // tell OpenGL to do its drawing within the entire "client area" (area within the borders) of the window
    glViewport(0, 0, width, height);
}

// main function
int main(int argc, char** argv)
{
    // initialize GLFW and ask for OpenGL 3.3 core
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);



    // create a GLFW window with the specified width, height, and title
    pWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);

    if (! pWindow)
    {
        // gracefully terminate if we cannot create the window
        std::cout << "Cannot create the GLFW window.\n";
        glfwTerminate();
        return -1;
    }
    glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  


    // make the window the current context of subsequent OpenGL commands,
    // and enable vertical sync and aspect-ratio correction on the GLFW window
    glfwMakeContextCurrent(pWindow);
    glfwSwapInterval(1);
    glfwSetWindowAspectRatio(pWindow, WINDOW_WIDTH, WINDOW_HEIGHT);

    // set up callback functions to handle window system events
    glfwSetKeyCallback(pWindow, handleKeys);
    glfwSetFramebufferSizeCallback(pWindow, handleResize);

    // don't miss any momentary keypresses
    glfwSetInputMode(pWindow, GLFW_STICKY_KEYS, GLFW_TRUE);

    // initialize GLAD, which acts as a library loader for the current OS's native OpenGL library
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // if our initial setup is successful...
    if (setup())
    {
        // do rendering in a loop until the user closes the window
        while (! glfwWindowShouldClose(pWindow))
        {
            // render our next frame
            // (by default, GLFW uses double-buffering with a front and back buffer;
            // all drawing goes to the back buffer, so the frame does not get shown yet)
            processInput(pWindow);
            glfwSetCursorPosCallback(pWindow, mouse_callback);
            render();

            // swap the GLFW front and back buffers to show the next frame
            glfwSwapBuffers(pWindow);

            // process any window events (such as moving, resizing, keyboard presses, etc.)
            glfwPollEvents();
        }
    }

    // gracefully terminate the program
    glfwTerminate();
    return 0;
}
