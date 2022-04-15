#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shader_s.h"

#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float mixValue = 0.2f;

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

    float vertices[] = {
        // positions        // textures coords
        0.5f, 0.5f, 0.0f,   1.0f, 1.0f,    // top right
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f,    // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,    // bottom left
        -0.5f, 0.5f, 0.0f,  0.0f, 1.0f,    // top left
    };
    unsigned int indices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    /** VERTEX BUFFER OBJECT AND VERTEX ARRAY OBJECT **/
    unsigned int VBO, VAO, EBO;   // Vertex buffer object
    glGenVertexArrays(1, &VAO);   // Give VAO unique buffer ID
    glGenBuffers(1, &VBO);        // Give VBO unique buffer ID
    glGenBuffers(1, &EBO);        // Give Element buffer unique buffer ID

    // First, bind VAO, then bind and set vertex buffers, then lastly configure vertex attributes
    glBindVertexArray(VAO);

    // Bind new buffer and make all buffer calls on GL_ARRAY_BUFFER apply to VBO)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Copy prev. defined vertices data into VBO and choose gpu draw method
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Bind EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    /** LINKING VERTEX ATTRIBUTES **/
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // LOAD TEXTURES:
    // TEXTURE 1
    unsigned int texture1, texture2;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set texture wrapping/filtering options (on currently bound texture argument)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture, and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on y-axis
    unsigned char *data = stbi_load(("assets/container.jpeg"), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
        std::cout << "Failed to load texture 1" << std::endl;
    stbi_image_free(data);

    // TEXTURE 2
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load(("assets/mario.png"), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
        std::cout << "Failed to load texture 2" << std::endl;
    stbi_image_free(data);


    /** DEBUG: WIREFRAME MODE **/
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // tell OpenGL which texture unit each shader belongs to
    ourShader.use();       
    // Can use either method below to do this (going to use both just for familiarity)
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);  // 0 for first
    ourShader.setInt("texture2", 1);   // 1 for texture2

    // RENDER LOOP (single buffer (use double buffer to avoid artifacting))
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // rendering commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);  // Clear color buffer

        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // create transformations
        glm::mat4 transform = glm::mat4(1.0f); // Initialize to identity matrix first
        // first container
        transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
        transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        // get matrix's uniform location and set matrix
        unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        
        // render first container
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // mix value (opacity of image)
        ourShader.setFloat("mixValue", mixValue);

        // second container       
        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(-0.5f, 0.5f, 0.0));
        float scaleAmount = static_cast<float>(sin(glfwGetTime()));
        transform = glm::scale(transform, glm::vec3(scaleAmount, scaleAmount, scaleAmount));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &transform[0][0]); // use the matrix's first element as mem pointer value
        
        // draw again
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);    // Swap color buffer to that's used to render and show it as output
        glfwPollEvents();           // Check if any events are triggered (inputs)
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

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        mixValue += 0.01f;
        if (mixValue >= 1.0f)
            mixValue = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        mixValue -= 0.01f;
        if (mixValue <= 0.0f)
            mixValue = 0.0f;
    }
}
