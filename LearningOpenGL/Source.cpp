#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\0";

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


	/** VERTEX SHADER **/
	// Create shader object
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Attach source code to shader object
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Check if vertex shader compilation was successful
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}	

	/** FRAGMENT SHADER **/
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Check if fragment shader compilation was successful
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}


	/** SHADER PROGRAM **/
	// Create program object and return ID reference of program object
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);    // Attach shader program with attached shaders
	
	// Check if shaders program failed
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
	}

	// Delete shader objects after linking (no longer need them)
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	float vertices[] = {
		0.5f, 0.5f, 0.0f,    // top right
		0.5f, -0.5f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,	 // bottom left
		-0.5f, 0.5f, 0.0f    // top left
		
	};
	unsigned int indices[] = {
		0, 1, 3,			 // first triangle
		1, 2, 3              // second triangle
	};


	/** VERTEX BUFFER OBJECT AND VERTEX ARRAY OBJECT **/
	unsigned int VBO, VAO, EBO;   // Vertex buffer object
	glGenVertexArrays(1, &VAO);   // Give VAO unique buffer ID
	glGenBuffers(1, &VBO);        // Give VBO unique buffer ID
	glGenBuffers(1, &EBO);        // Element Buffer Object (ID)

	// First, bind VAO, then bind and set vertex buffers, then lastly configure vertex attributes
	glBindVertexArray(VAO);

	// Bind new buffer and make all buffer calls on GL_ARRAY_BUFFER apply to VBO)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Copy prev. defined vertices data into buffer memory and chooses gpu draw method
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Do the same for EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	/** LINKING VERTEX ATTRIBUTES **/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind VAO afterwards so other VAO calls won't modify this VAO
	glBindVertexArray(0);


	/** DEBUG: WIREFRAME MODE **/
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	// RENDER LOOP (single buffer (use double buffer to avoid artifacting))
	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window);

		// rendering commands
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);  // Clear color buffer

		// Draw First Triangle
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		// Draw element instead of array for more than 1 set of vertices
		// This function takes its indices from EBO currently bound to GL_ELEMENT_ARRAY_BUFFER
		// The most recent element buffer object gets bound when VAO is bound and its stored in the VAO
		// (Mode, # of elements to draw, type of indices, offset in the EBO)
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);	// Swap color buffer to that's used to render and show it as output
		glfwPollEvents();			// Check if any events are triggered (inputs)
	}

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
