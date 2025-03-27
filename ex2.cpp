/******************************************************************************
 * This demo draws a triangle by defining its vertices in 3 dimensions
 * (the 3rd dimension is currently ignored and is just set to 0).
 *
 * The drawing is accomplished by:
 * - Uploading the vertices to the GPU using a Vertex Buffer Object (VBO).
 * - Specifying the vertices' format using a Vertex Array Object (VAO).
 * - Using a GLSL shader program (consisting of a simple vertex shader and a
 *   simple fragment shader) to actually draw the vertices as a triangle.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <gdev.h>
using namespace std;

// change this to your desired window attributes
#define WINDOW_WIDTH  1000
#define WINDOW_HEIGHT 1000
#define WINDOW_TITLE  "ROUND 1 - MY CLEMATIS"
GLFWwindow *pWindow;

float mapTexture(float coordinate) {
    return (coordinate / 2) + 0.5f;
}

// define a vertex array to hold our vertices
float vertices[] =   
{    
    // position (x, y, z)        color (r, g, b)          texture coordinates (s, t)              animation    isPetal   
    
    // middle    
    -0.030f,  0.314f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.030f), mapTexture( 0.314f),  1.0f,  0.0f,
     0.044f,  0.135f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.044f), mapTexture( 0.135f),  1.0f,  0.0f,
     0.240f,  0.131f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.240f), mapTexture( 0.131f),  1.0f,  0.0f,
     
    -0.262f,  0.155f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.262f), mapTexture( 0.155f),  1.0f,  0.0f, 
    -0.054f,  0.142f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.054f), mapTexture( 0.142f),  1.0f,  0.0f,
    -0.030f,  0.314f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.030f), mapTexture( 0.314f),  1.0f,  0.0f, 

    -0.262f,  0.155f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.262f), mapTexture( 0.155f),  1.0f,  0.0f,
    -0.174f, -0.171f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.174f), mapTexture(-0.171f),  1.0f,  0.0f,
    -0.095f, -0.002f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.095f), mapTexture(-0.002f),  1.0f,  0.0f, 

    -0.174f, -0.171f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.174f), mapTexture(-0.171f),  1.0f,  0.0f,
     0.141f, -0.143f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.141f), mapTexture(-0.143f),  1.0f,  0.0f,
     0.003f, -0.046f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.003f), mapTexture(-0.046f),  1.0f,  0.0f, 

     0.141f, -0.143f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.141f), mapTexture(-0.143f),  1.0f,  0.0f,
     0.240f,  0.131f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.240f), mapTexture( 0.131f),  1.0f,  0.0f,
     0.084f,  0.043f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.084f), mapTexture( 0.043f),  1.0f,  0.0f,

    -0.030f,  0.314f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.030f), mapTexture( 0.314f),  1.0f,  0.0f,
    -0.054f,  0.142f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.054f), mapTexture( 0.142f),  1.0f,  0.0f,
     0.044f,  0.135f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.044f), mapTexture( 0.135f),  1.0f,  0.0f,

    -0.262f,  0.155f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.262f), mapTexture( 0.155f),  1.0f,  0.0f,
    -0.095f, -0.002f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.095f), mapTexture(-0.002f),  1.0f,  0.0f,
    -0.054f,  0.142f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.054f), mapTexture( 0.142f),  1.0f,  0.0f,

    -0.095f, -0.002f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.095f), mapTexture(-0.002f),  1.0f,  0.0f,
    -0.174f, -0.171f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.174f), mapTexture(-0.171f),  1.0f,  0.0f,
     0.003f, -0.046f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.003f), mapTexture(-0.046f),  1.0f,  0.0f,

     0.003f, -0.046f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.003f), mapTexture(-0.046f),  1.0f,  0.0f,
     0.141f, -0.143f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.141f), mapTexture(-0.143f),  1.0f,  0.0f,
     0.084f,  0.043f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.084f), mapTexture( 0.043f),  1.0f,  0.0f,

     0.044f,  0.135f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.044f), mapTexture( 0.135f),  1.0f,  0.0f,
     0.084f,  0.043f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.084f), mapTexture( 0.043f),  1.0f,  0.0f,
     0.240f,  0.131f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.240f), mapTexture( 0.131f),  1.0f,  0.0f,

    -0.054f,  0.142f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.054f), mapTexture( 0.142f),  1.0f,  0.0f,
    -0.095f, -0.002f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.095f), mapTexture(-0.002f),  1.0f,  0.0f,
     0.044f,  0.135f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.044f), mapTexture( 0.135f),  1.0f,  0.0f,

     0.044f,  0.135f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.044f), mapTexture( 0.135f),  1.0f,  0.0f,
    -0.095f, -0.002f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.095f), mapTexture(-0.002f),  1.0f,  0.0f,
     0.003f, -0.046f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.003f), mapTexture(-0.046f),  1.0f,  0.0f,
 
     0.044f,  0.135f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.044f), mapTexture( 0.135f),  1.0f,  0.0f,
     0.003f, -0.046f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.003f), mapTexture(-0.046f),  1.0f,  0.0f,
     0.084f,  0.043f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.084f), mapTexture( 0.043f),  1.0f,  0.0f,
    

     // petal 1 (bottom )          
    -0.171f, -0.152f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.171f), mapTexture(-0.152f),  2.0f,  1.0f, 
    -0.406f, -0.649f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.406f), mapTexture(-0.649f),  2.0f,  1.0f, 
     0.135f, -0.141f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.135f), mapTexture(-0.141f),  2.0f,  1.0f,  

    -0.406f, -0.649f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.406f), mapTexture(-0.649f),  2.0f,  1.0f,
     0.403f, -0.471f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.403f), mapTexture(-0.471f),  2.0f,  1.0f,
     0.135f, -0.141f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.135f), mapTexture(-0.141f),  2.0f,  1.0f, 

    -0.406f, -0.649f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.406f), mapTexture(-0.649f),  2.0f,  1.0f,
     0.434f, -1.012f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.434f), mapTexture(-1.012f),  2.0f,  1.0f,
     0.403f, -0.471f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.403f), mapTexture(-0.471f),  2.0f,  1.0f,
         
    -0.406f, -0.649f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.406f), mapTexture(-0.649f),  2.0f,  1.0f,
    -1.140f, -1.030f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-1.140f), mapTexture(-1.030f),  2.0f,  1.0f,
     0.434f, -1.012f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.434f), mapTexture(-1.012f),  2.0f,  1.0f, 

    // petal 2 (bottom  left)          
    -0.737f,  0.324f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.737f), mapTexture( 0.324f),  3.0f,  2.0f, 
    -0.174f, -0.171f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.174f), mapTexture(-0.171f),  3.0f,  2.0f, 
    -0.262f,  0.155f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.262f), mapTexture( 0.155f),  3.0f,  2.0f,  

    -0.737f,  0.324f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.737f), mapTexture( 0.324f),  3.0f,  2.0f, 
    -0.406f, -0.649f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.406f), mapTexture(-0.649f),  3.0f,  2.0f, 
    -0.174f, -0.171f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.174f), mapTexture(-0.171f),  3.0f,  2.0f, 
     
    -0.737f,  0.324f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.737f), mapTexture( 0.324f),  3.0f,  2.0f, 
    -1.140f, -1.030f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-1.140f), mapTexture(-1.030f),  3.0f,  2.0f, 
    -0.406f, -0.649f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.406f), mapTexture(-0.649f),  3.0f,  2.0f, 
     
    -0.737f,  0.324f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.737f), mapTexture( 0.324f),  3.0f,  2.0f, 
    -1.000f,  0.613f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-1.000f), mapTexture( 0.613f),  3.0f,  2.0f, 
    -1.140f, -1.030f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-1.140f), mapTexture(-1.030f),  3.0f,  2.0f,  

    // petal  3 (top l eft)          
    -0.101f,  0.739f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.101f), mapTexture( 0.739f),  4.0f,  3.0f, 
    -0.262f,  0.155f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.262f), mapTexture( 0.155f),  4.0f,  3.0f, 
    -0.030f,  0.314f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.030f), mapTexture( 0.314f),  4.0f,  3.0f, 

    -0.101f,  0.739f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.101f), mapTexture( 0.739f),  4.0f,  3.0f, 
    -0.737f,  0.324f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.737f), mapTexture( 0.324f),  4.0f,  3.0f, 
    -0.262f,  0.155f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.262f), mapTexture( 0.155f),  4.0f,  3.0f, 

    -0.101f,  0.739f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.101f), mapTexture( 0.739f),  4.0f,  3.0f, 
    -1.626f,  1.157f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-1.626f), mapTexture( 1.157f),  4.0f,  3.0f, 
    -0.737f,  0.324f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.737f), mapTexture( 0.324f),  4.0f,  3.0f, 

    -0.101f,  0.739f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.101f), mapTexture( 0.739f),  4.0f,  3.0f, 
     0.033f,  1.150f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.033f), mapTexture( 1.150f),  4.0f,  3.0f, 
    -1.626f,  1.157f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-1.626f), mapTexture( 1.157f),  4.0f,  3.0f, 
         
    // petal  4 (top r ight)          
     0.692f,  0.383f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.692f), mapTexture( 0.383f),  5.0f,  4.0f,
    -0.030f,  0.314f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.030f), mapTexture( 0.314f),  5.0f,  4.0f,
     0.240f,  0.131f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.240f), mapTexture( 0.131f),  5.0f,  4.0f,   
       
     0.692f,  0.383f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.692f), mapTexture( 0.383f),  5.0f,  4.0f,
    -0.101f,  0.739f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.101f), mapTexture( 0.739f),  5.0f,  4.0f,
    -0.030f,  0.314f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.030f), mapTexture( 0.314f),  5.0f,  4.0f,

     0.692f,  0.383f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.692f), mapTexture( 0.383f),  5.0f,  4.0f,
     0.010f,  1.080f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.010f), mapTexture( 1.080f),  5.0f,  4.0f,
    -0.101f,  0.739f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.101f), mapTexture( 0.739f),  5.0f,  4.0f,

     0.692f,  0.383f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.692f), mapTexture( 0.383f),  5.0f,  4.0f,
     1.006f,  1.009f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 1.006f), mapTexture( 1.009f),  5.0f,  4.0f,
    -0.010f,  1.080f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture(-0.010f), mapTexture( 1.080f),  5.0f,  4.0f,

     0.692f,  0.383f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.692f), mapTexture( 0.383f),  5.0f,  4.0f,
     1.013f,  0.328f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 1.013f), mapTexture( 0.328f),  5.0f,  4.0f,
     1.006f,  1.009f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 1.006f), mapTexture( 1.009f),  5.0f,  4.0f,
         
    // petal 5 (bottom  right)          
     0.403f, -0.471f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.403f), mapTexture(-0.471f),  6.0f,  5.0f,
     0.240f,  0.131f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.240f), mapTexture( 0.131f),  6.0f,  5.0f,
     0.129F, -0.143f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.129F), mapTexture(-0.143f),  6.0f,  5.0f, 

     0.403f, -0.471f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.403f), mapTexture(-0.471f),  6.0f,  5.0f,
     0.692f,  0.383f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.692f), mapTexture( 0.383f),  6.0f,  5.0f,
     0.240f,  0.131f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.240f), mapTexture( 0.131f),  6.0f,  5.0f, 

     0.403f, -0.471f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.403f), mapTexture(-0.471f),  6.0f,  5.0f,
     1.018f,  0.334f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 1.018f), mapTexture( 0.334f),  6.0f,  5.0f,
     0.692f,  0.383f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.692f), mapTexture( 0.383f),  6.0f,  5.0f,   

     0.403f, -0.471f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.403f), mapTexture(-0.471f),  6.0f,  5.0f,
     1.025f, -1.030f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 1.025f), mapTexture(-1.030f),  6.0f,  5.0f,
     1.013f,  0.328f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 1.013f), mapTexture( 0.328f),  6.0f,  5.0f, 

     0.403f, -0.471f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.403f), mapTexture(-0.471f),  6.0f,  5.0f,
     1.025f, -1.030f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 1.025f), mapTexture(-1.030f),  6.0f,  5.0f,
     0.423f, -1.024f,  0.00f,    1.0f,  1.0f,  1.0f,      mapTexture( 0.423f), mapTexture(-1.024f),  6.0f,  5.0f,
};  
// define OpenGL object IDs to repres ent the vertex array and the shader program in the GPU
GLuint vao;         // vertex array o bject (stores the render state for our vertex array)
GLuint vbo;         // vertex buffer object (reserves GPU memory for our vertex array)
GLuint shader;      // combined vertex and fragment shader
GLuint texture;     // texture ID
GLuint texture2;
GLuint displacement;

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
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // on the VAO, register the current VBO with the following vertex attribute layout:
    // - layout location 0...
    // - ... shall consist of 3 GL_FLOATs (corresponding to x, y, and z coordinates)
    // - ... its values will NOT be normalized (GL_FALSE)
    // - ... the stride length is the number of bytes of all 3 floats of each vertex (hence, 3 * sizeof(float))
    // - ... and we start at the beginning of the array (hence, (void*) 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*) 0); // position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*) (3 * sizeof(float))); // color
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*) (6 * sizeof(float))); // texture
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*) (8 * sizeof(float))); // animation


    // enable the newly-created layout location 0;
    // this shall be used by our vertex shader to read the vertex's x, y, and z
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    // important: if you have more vertex arrays to draw, make sure you separately define them
    // with unique VAO and VBO IDs, and follow the same process above to upload them to the GPU

    // load our shader program
    shader = gdevLoadShader("ex2.vs", "ex2.fs");
    if (! shader)
        return false;

    texture = gdevLoadTexture("mizisua_01.png", GL_REPEAT, true, true);
    texture2 = gdevLoadTexture("mizisua_02.png", GL_REPEAT, true, true);
    displacement = gdevLoadTexture("displacement.png", GL_REPEAT, true, true);

    if (!texture) return false;
    if (!texture2) return false;
    if (!displacement) return false;

    //gdevLoadTexture is defined in gdev.h, and its parameters are:
    // - the texture filename
    // - the wrap mode (GL_REPEAT, GL_MIRRORED_REPEAT, ...)
    // - whether to enable linear filtering

    return true;
}

// called by the main function to do rendering per frame
void render()
{
    // clear the whole frame
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float t = glfwGetTime(); 
    glUniform1f(glGetUniformLocation(shader, "t"), t);

    int ii = glfwGetTime(); 
    int i = ii % 5;
    glUniform1f(glGetUniformLocation(shader, "i"), i);

    float duration = (-sin(t + 1.5) * 1/2);
    glUniform1f(glGetUniformLocation(shader, "d"), duration);
    // cout << duration << endl;
    // this duration is timing, if value is negative, the image is together
    // if positive, it's torn apart

    // using our shader program...
    glUseProgram(shader);

    // ... set the active texture...
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, displacement);

    glUniform1i(glGetUniformLocation(shader, "shaderTextureA"), 0);
    glUniform1i(glGetUniformLocation(shader, "shaderTextureB"), 1);
    glUniform1i(glGetUniformLocation(shader, "displaceTexture"), 2);

    // ... draw our triangles
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / (6 * sizeof(float)));
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
