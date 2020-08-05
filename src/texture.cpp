#include <stdlib.h>
#include <stdio.h>
#include <math.h>   // include math functions, such as sin, cos, M_PI
#include <iostream> // allow c++ style console printouts
#include "texture.h"

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <vector>

/**
 * This file contains util functions to load textures
 */
using namespace std; // allow console printouts without std::

/** Load a ppm file from disk.
 @input filename The location of the PPM file.  If the file is not found, an error message
		will be printed and this function will return 0
 @input width This will be modified to contain the width of the loaded image, or 0 if file not found
 @input height This will be modified to contain the height of the loaded image, or 0 if file not found
 @return Returns the RGB pixel data as interleaved unsigned chars (R0 G0 B0 R1 G1 B1 R2 G2 B2 .... etc) or 0 if an error ocured
**/
unsigned char* loadPPM(const char* filename, int& width, int& height)
{
	const int BUFSIZE = 128;
	FILE* fp;
	unsigned int read;
	unsigned char* rawData;
	char buf[3][BUFSIZE];
	char* retval_fgets;
	size_t retval_sscanf;

	if ((fp = fopen(filename, "rb")) == NULL)
	{
		std::cerr << "error reading ppm file, could not locate " << filename << std::endl;
		width = 0;
		height = 0;
		return NULL;
	}

	// Read magic number:
	retval_fgets = fgets(buf[0], BUFSIZE, fp);

	// Read width and height:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');
	retval_sscanf = sscanf(buf[0], "%s %s", buf[1], buf[2]);
	width = atoi(buf[1]);
	height = atoi(buf[2]);

	// Read maxval:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');

	// Read image data:
	rawData = new unsigned char[width * height * 3];
	read = fread(rawData, width * height * 3, 1, fp);
	fclose(fp);
	if (read != 1)
	{
		std::cerr << "error parsing ppm file, incomplete data" << std::endl;
		delete[] rawData;
		width = 0;
		height = 0;
		return NULL;
	}

	return rawData;
}

// load image file into texture object
unsigned int loadTexture(std::vector<char *> filenames)
{
	unsigned int textureID;     // storage for one texture
	int twidth, theight;   // texture width/height [pixels]
	unsigned char* tdata;  // texture pixel data

	// Create ID for texture
	glGenTextures(1, &textureID);
	// Set this texture to be the one we are working with
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	//glActiveTexture(GL_TEXTURE1);
	//glEnable(GL_TEXTURE_CUBE_MAP);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Load image file
	// TODO: study about the following codes. Compare with: https://learnopengl.com/Advanced-OpenGL/Cubemaps
	for (int i = 0; i < filenames.size(); i++) {
		char* filename = filenames[i];
		tdata = loadPPM(filename, twidth, theight);
		if (tdata == NULL) {
			std::cout << "Failed to load: " << filename << std::endl;
			break;
		}

		// Generate the texture
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
			0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, tdata);
		delete tdata;
	}
	// Set bi-linear filtering for both minification and magnification
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

// load 2D image texture
unsigned int load2DTexture(char* filename)
{
	unsigned int textureID;     // storage for one texture
	int twidth, theight;   // texture width/height [pixels]
	unsigned char* tdata;  // texture pixel data

	// Create ID for texture
	glGenTextures(1, &textureID);
	// Set this texture to be the one we are working with
	glBindTexture(GL_TEXTURE_2D, textureID);
	//glActiveTexture(GL_TEXTURE2);

	//glEnable(GL_TEXTURE_2D);

	// Load image file
	// TODO: study about the following codes. Compare with: https://learnopengl.com/Advanced-OpenGL/Cubemaps
	tdata = loadPPM(filename, twidth, theight);
	if (tdata == NULL) {
		std::cout << "Failed to load: " << filename << std::endl;
	}
	else {
		// Generate the texture
		std::cout << "Texture generated" << std::endl;
		glTexImage2D(GL_TEXTURE_2D,
			0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, tdata);
		delete tdata;
	}

	// Set bi-linear filtering for both minification and magnification
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

	return textureID;
}