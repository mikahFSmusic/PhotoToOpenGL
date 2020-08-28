#include <iostream>

#include "Image.h"
#include "vendors/stb_image/stb_image.h"
#include "vendors/stb_image/stb_image_write.h"
#include "Utilities.h"


Image::Image(std::string filepath, int screen_Width, int screen_Height)
	:img_ID(0), filepath(filepath),
	img_LocalBuffer(nullptr), normal_Buffer(nullptr),
	gray_Img(nullptr), 
	max_Alpha(NULL), min_Alpha(NULL),
	SCREEN_WIDTH(screen_Width), SCREEN_HEIGHT(screen_Height),
	z_Mod(1.0)
{	
	img_LocalBuffer = stbi_load(filepath.c_str(), &img_Width, &img_Height, &img_Channels, 0);
	if (img_LocalBuffer == NULL) {
		std::cout << "LOAD FAILED" << std::endl;
		exit(1);
	}
}

Image::~Image()
{
	// free the memory
	stbi_image_free(img_LocalBuffer);
	stbi_image_free(gray_Img);
}

std::string Image::ImgToGrayScale(std::string new_FileName)
{
	std::cout << "Input Channels: " << img_Channels << std::endl;

	size_t img_size = (uint64_t)img_Width * img_Height * img_Channels;
	int gray_channels = img_Channels == 4 ? 2 : 1;
	size_t gray_img_size = img_Width * img_Height * gray_channels;
	// allocates the memomry for an unsigned char of this particular images size
	gray_Img = (unsigned char*)malloc(gray_img_size);

	// pixel processing to greyscale
	// while p (pixels) is != image size, increment pixels, and grey pixels
	unsigned char* pg = gray_Img; 
	for (unsigned char* p = img_LocalBuffer; p != img_LocalBuffer + img_size; p += img_Channels)
	{
		// cast greyscale pixel as combined rgb value of input image as unsigned 8 bit integer
		if (pg) { *pg = (((double)(*p), (double)((*p) + 1), (*p) + 2)) / 3.0f; }
		
		// if there's a 4th channel (A), copy that to channel 2 of the grey image
		if (img_Channels == 4)
		{
			if (pg + 1 && pg + 3){ *(pg + 1) = *(pg + 3); }
			
		}
		pg += gray_channels;
	}

	// write greyscale image
	// store normal_Path variable from class
	normal_Path = "res/images/" + new_FileName + ".jpg";
	stbi_write_jpg(normal_Path.c_str(), img_Width, img_Height, gray_channels, gray_Img, img_Width * gray_channels);

	// return the path
	return normal_Path;
}

void Image::GetAlphas(std::string normal_Path)
{
	int n_Width, n_Height, n_Channels;
	// load the normal as a matrix of alpha values to be converted into signed floats between -1.0 and 1.0 for use in 3D space
	normal_Buffer = stbi_load(normal_Path.c_str(), &n_Width, &n_Height, &n_Channels, 0);

	// calculates the size of the incoming normaled image
	size_t normal_Size = n_Width * n_Height * n_Channels;
	std::cout << n_Channels << std::endl;
	
	float alpha_Val;
	// loops through normal buffer and assigns alpha values to a vector called alphas
	for (unsigned char* lb = normal_Buffer; lb != normal_Buffer + normal_Size; lb += n_Channels)
	{
		alpha_Val = (*lb, (*lb + 1.0f), (*lb + 2.0f)) / 3.0f;
		alpha_Map.push_back(alpha_Val);
	} 
}

void Image::setMinMaxAlphas()
{
	min_Alpha = alpha_Map[0];
	max_Alpha = alpha_Map[0];
	// custom min/max function for vector class
	for (unsigned int i = 0; i < alpha_Map.size(); i++)
	{
		if (alpha_Map[i] < min_Alpha)
		{
			min_Alpha = alpha_Map[i];
		}
		else if (alpha_Map[i] > max_Alpha)
		{
			max_Alpha = alpha_Map[i];
		}
	}
}

void Image::calcVerticesAndIndices()
{
	const double scr_Aspect_Ratio = (double)SCREEN_WIDTH / (double)SCREEN_HEIGHT;
	const double img_Aspect_Ratio = (double)img_Width / (double)img_Height;

	// set to 3.0 here to accomodate perspective shift, should probably be 2.0 for mathematical accuracy...  
	const double width_Mod = (3.0 * img_Aspect_Ratio) / (scr_Aspect_Ratio);
	const double x_Padding = (2.0 - width_Mod) / 2.0;
	const double x_Increment =  (width_Mod / img_Width);

	const double y_Increment = (2.0 / img_Height);

	float x_Start = -1.0f + x_Padding;
	float y_Start = 1.0f;
	unsigned int row = 0;
	unsigned int col = 0;
	unsigned int pos_Count = 0;

	unsigned int max_Cols = 0;
	unsigned int max_Rows = 0;

	// Assigns position values to vector
	for (int i = 0; (unsigned)i < alpha_Map.size() - 1; i++)
	{
		Utils i_Utils;

		// X
		float x_Val = x_Start + (col * x_Increment);
		vertices.push_back(x_Val);
		// increment column index 
		if (col < img_Width - 1) { col++; }
		// DEBUG update max col
		if (col > max_Cols) { max_Cols = col; }

		// Y
		float y_Val = y_Start - (row * y_Increment);
		vertices.push_back(y_Val);

		// Z
		float z_Val = i_Utils.set_snorm(alpha_Map[i], min_Alpha, max_Alpha, -z_Mod, z_Mod);
		//float z_Val = 0.0f;
		vertices.push_back(z_Val);

		float r_Val = i_Utils.norm(alpha_Map[i], min_Alpha, max_Alpha);
		//float r_Val = 1.0f;
		vertices.push_back(r_Val);

		float g_Val = i_Utils.norm(alpha_Map[i], min_Alpha, max_Alpha);
		//float g_Val = 0.5;
		vertices.push_back(g_Val);

		float b_Val = i_Utils.norm(alpha_Map[i], min_Alpha, max_Alpha);
		//float b_Val = 1.0;
		vertices.push_back(b_Val);

		// Alpha
		float a_Val = i_Utils.norm(alpha_Map[i], min_Alpha, max_Alpha);
		if (a_Val > 0.94) { a_Val = 0.0; }
		vertices.push_back(a_Val);
		pos_Count += 7;

		indices.push_back(i);

		// checks for the end of the row
		if (i > 0 && ((i % (img_Width)) == 0))
		{
			if (row < img_Height - 1)
			{
				row++; // if we've reached the end of a row, increment row index
				col = 0; // reset column index at end of each row
				//std::cout << "ROW: " << row << std::endl;

				// DEBUG update max row
				if (row > max_Rows) { max_Rows = row; }

			}
			else if (row == img_Height - 1)
			{
				col = 0; // reset column index at end of each row
			}
		}
		num_Vertices++;
	}
}

GLfloat* Image::getVertices()
{
	return &vertices[0];
}

GLint* Image::getIndices()
{
	return &indices[0];
}
