#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader_s.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    glfwInit();

    // Set OpenGL to Version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Use core-profile -> Get access to smaller subset of OpenGL
    // features w/o backwards-compatible features we don't need
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // macOS Support
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Make context of window main context on the current thread
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLAD before calling OpenGL functions
    // (GLAD manages function pointers for OpenGL and defines correct
    // function for current OS we're compiling for)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader ourShader("./Shaders/shader.vs", "./Shaders/shader.fs");

    float triangle[] = {
        // positions         // colors
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // bottom left   
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f, // bottom right
        0.0f, 0.5f, 0.0f,    0.0f, 0.0f, 1.0f  // top
    };

    /** VERTEX BUFFER OBJECT AND VERTEX ARRAY OBJECT **/
    unsigned int VBO, VAO;   // Vertex buffer object
    glGenVertexArrays(1, &VAO);   // Give VAO unique buffer ID
    glGenBuffers(1, &VBO);        // Give VBO unique buffer ID

    // First, bind VAO, then bind and set vertex buffers, then lastly configure vertex attributes
    glBindVertexArray(VAO);

    // Bind new buffer and make all buffer calls on GL_ARRAY_BUFFER apply to VBO)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Copy prev. defined vertices data into VBO and choose gpu draw method
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

    /** LINKING VERTEX ATTRIBUTES **/
    // position attribute (The 0 accounts for the location)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute (The 1 accounts for the location)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind VAO afterwards so other VAO calls won't modify this VAO
    glBindVertexArray(0);


    /** DEBUG: WIREFRAME MODE **/
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    // RENDER LOOP (single buffer (use double buffer to avoid artifacting))
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // rendering commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);  // Clear color buffer

        // render the triangle
        ourShader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);    // Swap color buffer to that's used to render and show it as output
        glfwPollEvents();            // Check if any events are triggered (inputs)
    }
    
    // De-allocate resources (optional but good practice)
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate(); // Deletes GLFW's resources that were allocated
    return 0;
}

// Callback function that gets called each time window is resized
void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
    // Tell OpenGL size of rendering window (so it knows how we want
    // to display data/coordinates w/ respect to the window)
    glViewport(0, 0, width, height);
}

// Input Handler
void processInput(GLFWwindow* window)
{
    // Check if Escape is being pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
