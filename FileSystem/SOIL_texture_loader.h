#ifndef SOIL_TEXTURE_LOADER

#include "../common_code.h"
#include <SOIL.h>
#include <GL/glew.h>

enum loaded_texture_type{
	LOADED_SIMPLE_TEXTURE,
	LOADED_CUBEMAP_TEXTURE,
};

struct loaded_texture{
	loaded_texture_type type;
	
	int width;
	int height;

	GLuint texture;
};

inline loaded_texture LoadTexture(const char* filePath){
	
	/*
	glGenTextures(1, &this->texture);
	glBindTexture(GL_TEXTURE_2D, this->texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned char* Image;
	Image = SOIL_load_image(
		FilePath,
		&this->width,
		&this->height,
		0,
		SOIL_LOAD_AUTO);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
	this->width, this->height, 0, GL_RGBA,
	GL_UNSIGNED_BYTE, Image);

	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(Image);
	glBindTexture(GL_TEXTURE_2D, 0);
	*/


	loaded_texture result;
	result.type = LOADED_SIMPLE_TEXTURE;
	
	result.texture = SOIL_load_OGL_texture
		(
		filePath,
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		0
		);

	glBindTexture(GL_TEXTURE_2D, result.texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	return result;
}

//Right -> Left -> Top -> Bottom -> Back -> Front
inline loaded_texture LoadCubemapTexture(std::vector<const char*> paths){
	loaded_texture result;
	result.type = LOADED_CUBEMAP_TEXTURE;

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

#if 1
	textureID = SOIL_load_OGL_cubemap(
		paths[0],
		paths[1],
		paths[2],
		paths[3],
		paths[4],
		paths[5],
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS);
#else
	int width;
	int height;
	unsigned char* imageTemp;
	for(int i = 0; i < 6; i++){
		imageTemp = SOIL_load_image(paths[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGB, width, height, 0, GL_RGB,
			GL_UNSIGNED_BYTE, imageTemp);
		SOIL_free_image_data(imageTemp);
	}
#endif

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	result.width = 0;
	result.height = 0;
	result.texture = textureID;
	return result;
}

#define SOIL_TEXTURE_LOADER
#endif