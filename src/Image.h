#pragma once
#include <GL/glew.h>
#include <string>
#include <vector>

class Image {
private: 
	unsigned int img_ID;
	unsigned char* img_LocalBuffer;
	unsigned char* normal_Buffer;
	unsigned char* gray_Img;
	
public:
	int img_Width, img_Height, img_Channels, SCREEN_WIDTH, SCREEN_HEIGHT;
	std::string filepath, normal_Path;
	std::vector<float> alpha_Map;
	float min_Alpha, max_Alpha;
	std::vector<GLfloat> vertices;
	std::vector<GLint> indices;
	unsigned int num_Vertices = 0;
	float z_Mod;

	Image(std::string filepath, int screen_Width, int screen_Height);
	~Image();
	std::string ImgToGrayScale(std::string new_FileName);
	void GetAlphas(std::string normal_Path);
	void setMinMaxAlphas();
	void calcVerticesAndIndices();
	GLfloat* getVertices();
	GLint* getIndices();
};