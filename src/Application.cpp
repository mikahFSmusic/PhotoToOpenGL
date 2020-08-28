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

#include "imgui/ImGuiFileDialog.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Image.h"
#include "Renderer.h"
#include "Shader.h"
#include "Camera.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Utilities.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

int main(void) 
{

	Utils utils;
	// constants
	// glsl version
	const char* glsl_version = "#version 330";

	// OpenGL Window Init
	GLFWwindow* window;

	// GLFW Init
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

	// checks if GLEW intializes correctly
	if (glewInit() != GLEW_OK)
		std::cout << "ERROR!" << std::endl;
	glewExperimental = GL_TRUE;

	std::cout << glGetString(GL_VERSION) << std::endl;

	// enable blending
	GLCall(glEnable(GL_BLEND));
	// get source alpha, subtract from one to blend alpha at destination  
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	// ImGUI stuff
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImGui::StyleColorsDark();

	const std::string& mvp_Name = "u_MVP";
	const std::string& color_Name = "color_Mod";

	// MODEL

	// Translation
	float mdl_XTranslate = 0.0f, mdl_YTranslate = 0.0f, mdl_ZTranslate = 0.0f;
	
	// Rotation
	float mdl_XRotate = 0.0f, mdl_YRotate = 0.0f, mdl_ZRotate = 0.0f;

	// Scale
	float g_Scale = 1.0f;

	// CAMERA 

	// Viewport Translation
	float x_Modulate = 0.0f, y_Modulate = 0.0f, z_Modulate = 0.0;

	// Color
	float red_Mod = 0.0f, green_Mod = 0.0f, blue_Mod = 0.0;
	float alpha_Mod = 1.0;

	glm::vec3 camera_Position = glm::vec3(0, 0, 1);
	glm::vec3 camera_Target = glm::vec3(0.0, 0.0, 0.0); // camera targets the origin
	glm::vec3 up_Vector = glm::vec3(0, 1, 0);
	
	// PERSPECTIVE 
	float field_Of_View = 45.0;

	// SETTINGS
	const char* gl_Draw_Mode[] = { "GL_POINTS", "GL_LINES", "GL_LINE_LOOP", "GL_TRIANGLES"};
	const int gl_Draw_Mode_Int[] = { GL_POINTS, GL_LINES, GL_LINE_LOOP, GL_TRIANGLES };
	int current_Draw_Mode = 1; 
	float gl_Point_Size = 0.2;

	std::unique_ptr<VertexArray> VAO;
	std::unique_ptr<VertexBuffer> VBO;

	Shader shader((const GLchar*)"res/shaders/Vertex.Shader", (const GLchar*)"res/shaders/Fragment.Shader");

	std::unique_ptr<Image> image;
	GLfloat* vertices;
	int num_Vertices;

	// Initialize Default Image
	std::string img_Filepath = "res/images/default.jpg";
	std::string img_Outpath = "gray_default";

	// load the default image
	image = std::make_unique<Image>(img_Filepath, SCREEN_WIDTH, SCREEN_HEIGHT);
	std::string normal_Path = image->ImgToGrayScale(img_Outpath);
	// initialize image data
	image->GetAlphas(normal_Path);
	image->setMinMaxAlphas();
	// TODO: make this modifiable in real time by ImGui interaction
	image->z_Mod = 0.5;
	float new_Z = image->z_Mod;
	// calculate vertex and index positions
	image->calcVerticesAndIndices();
	vertices = image->getVertices();
	num_Vertices = image->num_Vertices;

	//  Vertex array obect, 
	VAO = std::make_unique<VertexArray>();
	VBO = std::make_unique<VertexBuffer>(vertices, num_Vertices * 7 * sizeof(GLfloat));
	VertexBufferLayout layout;
	layout.Push<float>(3);
	layout.Push<float>(4);
	VAO->AddBuffer(*VBO, layout);

	while (!glfwWindowShouldClose(window)) {

		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		GLCall(glPointSize(gl_Point_Size));

		// Start the ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Model Matrix		
		glm::vec3 model_Position = glm::vec3(mdl_XTranslate, mdl_YTranslate, mdl_ZTranslate);
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), model_Position);
		glm::mat4 rotation_X = glm::rotate(glm::radians(mdl_XRotate), glm::vec3(1, 0, 0));
		glm::mat4 rotation_Y = glm::rotate(glm::radians(mdl_YRotate), glm::vec3(0, 1, 0));
		glm::mat4 rotation_Z = glm::rotate(glm::radians(mdl_ZRotate), glm::vec3(0, 0, 1));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(g_Scale));
		glm::mat4 model_Matrix = translation * rotation_X * rotation_Y * rotation_Z * scale;

		// View Matrix
		glm::mat4 view_Matrix = (glm::translate(glm::mat4(1.0f), glm::vec3(-x_Modulate, -y_Modulate, -z_Modulate)));

		// Camera Matrix
		glm::vec3 camera_Position = glm::vec3(0.0f, 0.0f, 3.0f);
		glm::mat4 camera_Matrix = glm::lookAt(camera_Position, camera_Target, up_Vector);

		// Projection Matrix 
		glm::mat4 projection_Matrix = glm::perspective(glm::radians(field_Of_View), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
		glm::mat4 mvp = projection_Matrix * camera_Matrix * view_Matrix * model_Matrix;

		// Draw Color
		glm::mat4 color = glm::translate(glm::mat4(alpha_Mod), glm::vec3(red_Mod, green_Mod, blue_Mod));

		shader.Bind();
		// gets location of mvp uniform
		GLCall(GLint mvp_Location = glGetUniformLocation(shader.m_RendererID, mvp_Name.c_str()));
		if (mvp_Location == -1)
			std::cout << "Warning: uniform '" << mvp_Name << "' doesn't exist." << std::endl;

		// color uniform
		GLCall(GLint color_Location = glGetUniformLocation(shader.m_RendererID, color_Name.c_str()));
		if (color_Location == -1)
			std::cout << "Warning: uniform '" << color_Name << "' doesn't exist." << std::endl;

		// instantiates uniforms
		GLCall(glUniformMatrix4fv(mvp_Location, 1, GL_FALSE, &mvp[0][0]));
		GLCall(glUniformMatrix4fv(color_Location, 1, GL_TRUE, &color[0][0]));
		// Binds Vertex Array Object
		VAO->Bind();
		
		// Draw Call 
		GLCall(glDrawArrays(gl_Draw_Mode_Int[current_Draw_Mode], 0, num_Vertices));

		// IMGUI PARAMETERS MENU

		// TODO: Load Image, calculate vertices on load before executing any OpenGL code
		ImGui::Begin("Parameters");

		ImGui::Text("Load");
		
		if (ImGui::Button("Open Image File")) 
		{
			igfd::ImGuiFileDialog::Instance()->OpenDialog("ChooseImgFileDlgKey", "Choose File", ".jpg, .png", ".");
		}
		// display
		if (igfd::ImGuiFileDialog::Instance()->FileDialog("ChooseImgFileDlgKey"))
		{
			// action if OK
			if (igfd::ImGuiFileDialog::Instance()->IsOk == true)
			{
				img_Filepath = igfd::ImGuiFileDialog::Instance()->GetFilepathName();

				image = std::make_unique<Image>(img_Filepath, SCREEN_WIDTH, SCREEN_HEIGHT);

				img_Outpath = "gray";
				normal_Path = image->ImgToGrayScale(img_Outpath);
				image->GetAlphas(normal_Path);
				image->setMinMaxAlphas();
				// TODO: make this modifiable in real time by ImGui interaction
				image->z_Mod = 0.5;
				// calculate vertex and index positions
				image->calcVerticesAndIndices();
				vertices = image->getVertices();
				num_Vertices = image->num_Vertices;

				//  Vertex array obect, 
				VAO = std::make_unique<VertexArray>();
				VBO = std::make_unique<VertexBuffer>(vertices, num_Vertices * 7 * sizeof(GLfloat));
				VertexBufferLayout layout;
				layout.Push<float>(3);
				layout.Push<float>(4);
				VAO->AddBuffer(*VBO, layout);

			}
			// close
			igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseImgFileDlgKey");
		}
		

		// Mouse + Keyboard Camera Control
		ImGuiMouseCursor mouse_Cursor = ImGui::GetMouseCursor();
		ImVec2 mousePos = ImGui::GetMousePos();
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsMousePosValid(&mousePos))
		{
			ImVec2 mouse_Drag_Delta = ImGui::GetMouseDragDelta();
			// TODO: GET SHIFT KEY PRESS WORKING
			if (ImGui::IsKeyDown(GLFW_KEY_LEFT_SHIFT)) 
			{
				mdl_ZRotate = utils.set_snorm(mouse_Drag_Delta[1], 0, SCREEN_WIDTH, 0, 1080.0f);
			}
			else {
				mdl_XRotate = utils.set_snorm(mouse_Drag_Delta[1], 0, SCREEN_WIDTH, 0, 1080.0f);
				mdl_YRotate = utils.set_snorm(mouse_Drag_Delta[0], 0, SCREEN_HEIGHT, 0, 1080.0f);
			}
			
			
		} else if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && ImGui::IsMousePosValid(&mousePos))
		{
			ImVec2 mouse_Drag_Delta = ImGui::GetMouseDragDelta(1);
			//std::cout << "Drag X: " << mouse_Drag_Delta[0] << " | " << "Drag Y: " << mouse_Drag_Delta[1] << std::endl;
			if (ImGui::IsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
				mdl_ZTranslate = utils.set_snorm(mouse_Drag_Delta[1], -SCREEN_HEIGHT, SCREEN_HEIGHT, -3.0f, 3.0f);
			}
			else
			{
				mdl_XTranslate = utils.set_snorm(mouse_Drag_Delta[0], -SCREEN_WIDTH, SCREEN_WIDTH, -3.0f, 3.0f);
				mdl_YTranslate = utils.set_snorm(mouse_Drag_Delta[1], -SCREEN_HEIGHT, SCREEN_HEIGHT, -3.0f, 3.0f);
			}
			// need handling for z translation (Scroll wheel)
		}
		

		ImGui::Separator();
		ImGui::Text("Model");
		// TODO: Need to convert these to mouse controllable rotation/translation functions, probably using camera controls.
		// TODO: add "explode", width, height 
		ImGui::Text("Use Left [Shift] for Z axis tranlation/rotation");
		ImGui::SliderFloat("Scale ", &g_Scale, 0.001f, 2.0f);
		
		ImGui::Separator();

		ImGui::Text("View");
		ImGui::SliderFloat("Cam Move X ", &x_Modulate, -3.0f, 3.0f);
		ImGui::SliderFloat("Cam Move Y ", &y_Modulate, -3.0f, 3.0f);
		ImGui::SliderFloat("Cam Move Z ", &z_Modulate, -3.0f, 3.0f);

		ImGui::Separator();

		ImGui::Text("Perspective");
		ImGui::SliderFloat("FOV ", &field_Of_View, 1.0f, 150.0f);

		ImGui::Separator();

		ImGui::Text("Color");
		ImGui::SliderFloat("Red", &red_Mod, 0.0f, 1.0f);
		ImGui::SliderFloat("Green", &green_Mod, 0.0f, 1.0f);
		ImGui::SliderFloat("Blue", &blue_Mod, 0.0f, 1.0f);
		ImGui::SliderFloat("Alpha", &alpha_Mod, 0.0f, 1.0f);

		ImGui::Separator();

		ImGui::Text("Draw Options");
		// DRAW MODE
		ImGui::ListBox("Draw Mode", &current_Draw_Mode, gl_Draw_Mode, IM_ARRAYSIZE(gl_Draw_Mode), 4);
		ImGui::SliderFloat("Point Size", &gl_Point_Size, 0.001f, 10.0f);

		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		GLCall(glBindVertexArray(0));
		// Swap frnt and back buffers
		glfwSwapBuffers(window);

		// Poll for, and process events
		glfwPollEvents();
	}

	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	glfwTerminate();
	shader.Unbind();
	VAO->Unbind();
	VBO->Unbind();
	image->~Image();
 }

// 1. Generate an array of points, evenly distributed accross x and y axis, all of which have the same z value (0) -- DONE
	//		use width and height to determine number of verteces to draw
	//		2.0/width + index, 2.0/height + index to get -1.0 -> 1.0 range of x and y vals 

// 2. Modify each z value with a corresponding alpha value -- DONE
	//		need to scale alpha value between -1.0 and 1.0 

// 3. Display the points as points in OpenGL 3D space, with the camera pointed directly at the matrix
	//		need to iterate accross "verteces" and insert them validly into OpenGL VertexArray	
