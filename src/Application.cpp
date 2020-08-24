#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// image processing libs
#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Image.h"
#include "Renderer.h"
#include "Shader.h"
#include "Camera.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

// signed normalization function for scaling input value with a known input range to an output range
float snorm(float value, float in_Min, float in_Max) 
{
	float out_Value = ( ((1.0f - -1.0f) * ((value - in_Min) / (in_Max - in_Min))) + -1.0f );
	return out_Value;
}

// unsigned normalization function
float norm(float value, float in_Min, float in_Max) 
{
	float out_Value = ( ((1.0f - 0.0f) * ((value - in_Min) / (in_Max - in_Min))) + 0.0f );
	return out_Value;
}

int main(void) 
{
	// glsl version
	const char* glsl_version = "#version 330";

	/*Image image("res/images/drama_mask_white.jpg");
	std::string normal_Path = image.ImgToGrayScale("gray_mask");*/
	/*Image image("res/images/mikahfs.jpg");
	std::string normal_Path = image.ImgToGrayScale("gray_mikah");*/
	// loads an image to greyscale (normal) and returns the path to that normal
	
	Image image("res/images/willyb.jpg");
	std::string normal_Path = image.ImgToGrayScale("gray_willyb");

	image.GetAlphas(normal_Path);
	image.setMinMaxAlphas();
	
	const int lcl_Width = image.img_Width;
	const int lcl_Height = image.img_Height;

	std::cout << "Width: " << lcl_Width << "| Height: " << lcl_Height << std::endl;
	//std::cout << "Alphas Size: " << &lcl_Alphas.size() << std::endl;
	
	std::cout << "Min Alpha: " << image.min_Alpha << std::endl; 
	std::cout << "Max Alpha: " << image.max_Alpha << std::endl;

	const double x_Increment = 1.0f / lcl_Width; 
	const double y_Increment = 2.0f / lcl_Height;

	std::cout << "X Axis Modifier: " << x_Increment << std::endl;
	std::cout << "Y Axis Modifier: " << y_Increment << std::endl;

	std::vector<GLfloat> positions;
	std::vector<GLint> indices;
	unsigned int row = 0;
	unsigned int col = 0;
	unsigned int num_Vertices = 0; 
	unsigned int pos_Count = 0; 

	unsigned int max_Cols = 0;
	unsigned int max_Rows = 0;

	std::cout << " Alpha map size (should be 1,776,889):  " << image.alpha_Map.size() << std::endl;

	// Assigns position values to vector
	for (int i = 0; (unsigned)i < image.alpha_Map.size() - 1; i++)  
	{
		// POSITION ASSIGNMENT
		// DEBUG
		/*if (i < 10000) 
		{
			std::cout << "-----------------------------------" << std::endl;
			std::cout << "i: " << i << " | C: " << col << " | R: " << row << std::endl;
		}*/

		// X
		float x_Val = -0.5f + (col * x_Increment);
		positions.push_back(x_Val);
		// increment column index 
		if (col < lcl_Width - 1) { col++; }
		// DEBUG update max col
		if (col > max_Cols) { max_Cols = col; }

		// Y
		float y_Val = 1.0f - (row * y_Increment);
		positions.push_back(y_Val);
		
		// Z
		float z_Val = snorm(image.alpha_Map[i], image.min_Alpha, image.max_Alpha);
		positions.push_back(z_Val);

		float r_Val = norm(image.alpha_Map[i], image.min_Alpha, image.max_Alpha);
		positions.push_back(r_Val);

		float g_Val = norm(image.alpha_Map[i], image.min_Alpha, image.max_Alpha);
		positions.push_back(g_Val);
		
		float b_Val = norm(image.alpha_Map[i], image.min_Alpha, image.max_Alpha);
		positions.push_back(b_Val);

		// Alpha
		float a_Val = norm(image.alpha_Map[i], image.min_Alpha, image.max_Alpha);
		//if (a_Val > 0.94) { a_Val = 0.0; }
		positions.push_back(a_Val);
		pos_Count += 7;

		//std::cout << "X: " << x_Val << " Y: " << y_Val << " Z: " << z_Val << std::endl;
		
		indices.push_back(i);

		// checks for the end of the row
		if (i > 0 && ((i % (image.img_Width)) == 0))
		{
			if (row < lcl_Height - 1)
			{
				row++; // if we've reached the end of a row, increment row index
				col = 0; // reset column index at end of each row
				//std::cout << "ROW: " << row << std::endl;

				// DEBUG update max row
				if (row > max_Rows) { max_Rows = row; }

			}
			else if (row == lcl_Height - 1)
			{
				col = 0; // reset column index at end of each row
			}
		}
		num_Vertices++;
	}

	//std::cout << "Num Verteces: " << num_Vertices << std::endl;
	//std::cout << "Num Positions: " << pos_Count << std::endl;

	// OpenGL stuff

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit()) return -1;

	// create window and context with core profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World", nullptr, nullptr);
	int screen_Width, screen_Height;
	glfwGetFramebufferSize(window, &screen_Width, &screen_Height);

	if (!window) 
	{
		glfwTerminate();
		return EXIT_FAILURE;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	//glfwSwapInterval(1);

	// checks if GLEW intializes correctly
	if (glewInit() != GLEW_OK)
		std::cout << "ERROR!" << std::endl;
	glewExperimental = GL_TRUE;

	std::cout << glGetString(GL_VERSION) << std::endl;

	//GLCall(glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));

	Shader shader((const GLchar*)"res/shaders/Vertex.Shader", (const GLchar*)"res/shaders/Fragment.Shader");

	GLfloat* vertices = &positions[0];
	
	float max = *std::max_element(positions.begin(), positions.end());
	int max_Index = std::max_element(positions.begin(), positions.end()) - positions.begin();

	//std::cout << "Max value: " << max << std::endl;
	//std::cout << "Max value INDEX: " << max_Index << std::endl;


	// for testing, prints out vertices 
	//std::cout << "-------" << std::endl;
	/*for (int i = 0; (unsigned)i < pos_Count; i++) 
	{

		if (i > 2318000 && i < 2319000)
		{
			if (i % 3 == 0) {
			std::cout << "-------" << std::endl;
		}
			std::cout << "Index: " << i << " | Value: " << vertices[i] << std::endl;
		}
	}*/

	//std::cout << "FIRST INDEX OF POSITIONS ARRAY: " << vertices[0] << std::endl;

	GLuint VBO, VAO;
	// generate vertex array object
	GLCall(glGenVertexArrays(1, &VAO));

	// generate vertex buffer object
	GLCall(glGenBuffers(1, &VBO));

	GLCall(glBindVertexArray(VAO));

	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, num_Vertices * 7 * sizeof(GLfloat), vertices, GL_STATIC_DRAW));

	std::cout << "Num Indices: " << indices.size() << " | Calculated Num Vertices: " << num_Vertices << std::endl;
	std::cout << "POSITIONS: " << positions.size() << "|  Verteces: " << (positions.size() / 3) << std::endl;
	std::cout << "Sizeof Vertices: " << sizeof(vertices) << " | Sizeof Positions: " << sizeof(positions) << std::endl;
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*) 0));

	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))));

	GLCall(glBindVertexArray(0));

	//GLCall(glEnable(GL_DEPTH_TEST));
	// enable blending
	GLCall(glEnable(GL_BLEND));
	// get source alpha, subtract from one to blend alpha at destination  
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	// Camera

	//glm::vec3 camera_Position(0, 0, 3);
	
	//Camera camera(camera_Position, 70.0f, (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT, 1.0, -1.0f);
	//glm::mat4 m_Proj = glm::ortho(-1.0f, (float)SCREEN_WIDTH, -1.0f, (float)SCREEN_HEIGHT, -100.0f, 100.0f);
	//glm::mat4 m_View = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	//glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	//glm::mat4 mvp = m_Proj * m_View * model; 
	//glm::mat4 mvp = glm::mat4(12.0);


	glm::mat4 mvp = glm::translate(glm::mat4(1.0f), glm::vec3(0.3, 0.3, 0.3));
	const std::string& name = "u_MVP";
	while (!glfwWindowShouldClose(window)) {
		GLCall(glClearColor(0.0, 0.0, 0.0, 1.0));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
		
		shader.Use();
		// gets location of mvp uniform
		GLCall(GLint u_Location = glGetUniformLocation(shader.m_RendererID, name.c_str()));
		if (u_Location == -1)
			std::cout << "Warning: uniform '" << name << "' doesn't exist." << std::endl;

		//// set the mvp (model view projection) matrix by name, with it's respective glm::vec4 value (model * view * projection)
		GLCall(glUniformMatrix4fv(u_Location, 1, GL_TRUE, &mvp[0][0]));
		
		GLCall(glBindVertexArray(VAO)); 
		glPointSize(1.0);
		GLCall(glDrawArrays(GL_POINTS, 0, num_Vertices));
		glBindVertexArray(0);

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for, and process events
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO); 
	glfwTerminate();
 }

// 1. Generate an array of points, evenly distributed accross x and y axis, all of which have the same z value (0) -- DONE
	//		use width and height to determine number of verteces to draw
	//		2.0/width + index, 2.0/height + index to get -1.0 -> 1.0 range of x and y vals 

// 2. Modify each z value with a corresponding alpha value -- DONE
	//		need to scale alpha value between -1.0 and 1.0 

// 3. Display the points as points in OpenGL 3D space, with the camera pointed directly at the matrix
	//		need to iterate accross "verteces" and insert them validly into OpenGL VertexArray	
