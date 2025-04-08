/*
 *   Controls:
 *       WASD: movement(W - forward, A - left, S - backward, D - right)
 *       Mouse: move camera perspective
 *       look up/down + W/S: go up or down
 */

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

// change this to your desired window attributes
#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 360
#define WINDOW_TITLE  "Hello 3D World"
GLFWwindow *pWindow;
#define PI 3.14159

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
#define ORIGIN_SIDE(back) ORIGIN(back)[0], ORIGIN(back)[1], ORIGIN(back)[2], LYELLOW, shift(ORIGIN(back)[0]), shift(ORIGIN(back)[1])

#define STAR_VERT(start, a, back) rotateVertex3D(rotateVertex3D(start, z_axis, a*72*PI/180), y_axis, back)
#define STAR_VERTS(start, a, back, color) STAR_VERT(start, a, back)[0], STAR_VERT(start, a, back)[1], STAR_VERT(start, a, back)[2], color, shift(STAR_VERT(start, a, back)[0]), shift(STAR_VERT(start, a, back)[1])

#define STAR_POINTS_ONE(b, back) STAR_VERTS(sv_zero, b, back, YELLOW), STAR_VERTS(sv_one, b, back, YELLOW), STAR_VERTS(sv_two, b, back, YELLOW), STAR_VERTS(sv_zero, b, back, YELLOW), STAR_VERTS(sv_two, b, back, YELLOW), STAR_VERTS(sv_three, b, back, GOLD)
#define STAR_POINTS_TWO(b, back) STAR_VERTS(sv_zero, b, back, YELLOW), STAR_VERTS(sv_three, b, back, GOLD), STAR_VERTS(sv_four, b, back, GOLD), STAR_VERTS(sv_zero, b, back, YELLOW), STAR_VERTS(sv_four, b, back, GOLD), STAR_VERTS(sv_five, b, back, GOLD)
#define STAR_POINTS_THREE(b, back) STAR_VERTS(sv_zero, b, back, YELLOW), STAR_VERTS(sv_five, b, back, GOLD), STAR_VERTS(sv_six, b, back, YELLOW), STAR_VERTS(sv_zero, b, back, YELLOW), STAR_VERTS(sv_six, b, back, YELLOW), STAR_VERTS(sv_one, (b+1), back, YELLOW)

#define STAR_POINT(b, back) STAR_POINTS_ONE(b, back), STAR_POINTS_TWO(b, back), STAR_POINTS_THREE(b, back)

#define INNER_STAR_POINTS(c, back) STAR_VERTS(sv_zero, c, back, YELLOW), STAR_VERTS(sv_one, (c+1), back, YELLOW), STAR_VERTS(sv_zero, (c+1), back, YELLOW)
#define INNER_PENTAGON(c, back) ORIGIN_SIDE(back), STAR_VERTS(sv_mid_zero, c, back, YELLOW), STAR_VERTS(sv_mid_zero, (c+1), back, YELLOW)

#define FILLING(c, back) STAR_VERTS(sv_mid_zero, c, back, YELLOW), STAR_VERTS(sv_zero, c, back, YELLOW), STAR_VERTS(sv_mid_zero, (c+1), back, YELLOW), STAR_VERTS(sv_mid_zero, (c+1), back, YELLOW), STAR_VERTS(sv_zero, c, back, YELLOW), STAR_VERTS(sv_zero, (c+1), back, YELLOW)

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

// define OpenGL object IDs to represent the vertex array and the shader program in the GPU
GLuint vao;         // vertex array object (stores the render state for our vertex array)
GLuint vbo;         // vertex buffer object (reserves GPU memory for our vertex array)
GLuint shader;      // combined vertex and fragment shader
GLuint texture_eyes, texture_rainbow;


glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

bool firstMouse = true;
float yaw   = -90.0f;
float pitch =  0.0f;
float lastX =  640 / 2.0;
float lastY =  360 / 2.0;
float fov   =  45.0f;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame


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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));

    // enable the newly-created layout location 0;
    // this shall be used by our vertex shader to read the vertex's x, y, and z
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);


    // important: if you have more vertex arrays to draw, make sure you separately define them
    // with unique VAO and VBO IDs, and follow the same process above to upload them to the GPU

    // load our shader program
    shader = gdevLoadShader("exer3.vs", "exer3.fs");
    if (! shader)
        return false;

    texture_eyes = gdevLoadTexture("texture_eyes.png", GL_REPEAT, true, true);
    if (! texture_eyes) return false;

    texture_rainbow = gdevLoadTexture("texture_rainbow.png", GL_REPEAT, true, true);
    if (! texture_rainbow) return false;

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
    glm::mat4 matrix;

    float currentFrame = glfwGetTime();
    float time = currentFrame;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // matrix = glm::translate(matrix, glm::vec3(0.0f, 0.0f, -5.0f));
    // matrix = glm::rotate(matrix, glm::radians(-90.0f),
    // glm::vec3(1.0f, 0.0f, 0.0f));
    // matrix = glm::scale(matrix, glm::vec3(5.0f, 5.0f, 1.0f));

    // ... draw our triangles

    matrix = glm::perspective(glm::radians(60.0f), 
                            (float) WINDOW_WIDTH / WINDOW_HEIGHT,
                            0.1f,
                            100.0f);

    // default
    matrix = matrix * glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    glBindVertexArray(vao);

    // middle star
    matrix = glm::rotate(matrix, time, glm::vec3(0.0f, 0.0f, 1.0f));
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / (8 * sizeof(float)));
    glUniformMatrix4fv(glGetUniformLocation(shader, "matrix"), 1, GL_FALSE, glm::value_ptr(matrix));

    // rightstar    
    matrix = glm::rotate(matrix, -time, glm::vec3(0.0f, 0.0f, 1.0f));

    matrix = glm::translate(matrix, glm::vec3(2.3f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, time, glm::vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::scale(matrix, glm::vec3(1.5f, 1.5f, 1.0f));

    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / (8 * sizeof(float)));
    glUniformMatrix4fv(glGetUniformLocation(shader, "matrix"), 1, GL_FALSE, glm::value_ptr(matrix));

    // leftstar
    matrix = glm::scale(matrix, glm::vec3(0.67f, 0.67f, 1.0f));
    matrix = glm::rotate(matrix, -time, glm::vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::translate(matrix, glm::vec3(-2.3f, 0.0f, 0.0f));

    matrix = glm::translate(matrix, glm::vec3(-2.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, time, glm::vec3(1.0f, 0.0f, 0.0f));
    matrix = glm::scale(matrix, glm::vec3(1.0f, 0.5f, 1.0f));

    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / (8 * sizeof(float)));
    glUniformMatrix4fv(glGetUniformLocation(shader, "matrix"), 1, GL_FALSE, glm::value_ptr(matrix));


    glUniform1i(glGetUniformLocation(shader, "shaderTextureEyes"), 0);
    glUniform1i(glGetUniformLocation(shader, "shaderRainbow"), 1);

    glUniform1f(glGetUniformLocation(shader, "time"), currentFrame);

    glEnable(GL_CULL_FACE);
}

void processInput(GLFWwindow *window)
{

    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
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

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    // int windowX = (mode->width - windowWidth) / 2;
    // int windowY = (mode->height - windowHeight) / 2;

    // create a GLFW window with the specified width, height, and title
    pWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);

    glfwSetWindowPos(pWindow, (mode->width - 640)/2, (mode->height - 360)/2);
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
