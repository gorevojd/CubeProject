#ifndef SOIL_TEXTURE_LOADER

#include <SOIL.h>
#include <GL/glew.h>

struct loaded_texture{
	int width;
	int height;

	GLuint texture;

	loaded_texture(){}
	loaded_texture(const char* FilePath){
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

		this->texture = SOIL_load_OGL_texture
			(
				FilePath,
				SOIL_LOAD_AUTO,
				SOIL_CREATE_NEW_ID,
				0
			);

		glBindTexture(GL_TEXTURE_2D, this->texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
};



#define SOIL_TEXTURE_LOADER
#endif