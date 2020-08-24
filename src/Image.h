#pragma once

#include <string>
#include <vector>


class Image {
private: 
	unsigned int img_ID;
	unsigned char* img_LocalBuffer;
	unsigned char* normal_Buffer;
	unsigned char* gray_Img;
	
public:
	std::string filepath, normal_Path;
	std::vector<float> alpha_Map;
	int img_Width, img_Height, img_Channels;
	float min_Alpha, max_Alpha;
	
	Image(std::string filepath);
	~Image();
	std::string ImgToGrayScale(std::string new_FileName);
	void GetAlphas(std::string normal_Path);
	void setMinMaxAlphas();
};