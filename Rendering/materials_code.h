#ifndef MATERIAL_MY_IMPL_H

#include "../common_code.h"

struct phong_material{
	u32 diffuseTexture;
	u32 specularTexture;
	u32 normalMapTexture;
	u32 emissionTexture;
	float shininess;

	bool withNormalMap;

	phong_material(){
		this->diffuseTexture = 0;
		this->specularTexture = 0;
		this->normalMapTexture = 0;
		this->emissionTexture = 0;
		this->shininess = 32.0f;
	}

	phong_material(
		u32 diffuseTexture,
		u32 specularTexture,
		u32 normalMapTexture,
		u32 emissionTexture,
		float shininess)
	{
		this->diffuseTexture = diffuseTexture;
		this->specularTexture = specularTexture;
		this->normalMapTexture = normalMapTexture;
		this->emissionTexture = emissionTexture;
		this->shininess = shininess;
	}
};

/*
inline void UniformPhongMaterial(loaded_shader shader, phong_material mater){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mater.diffuseTexture);
	glUniform1i(glGetUniformLocation(shader.program, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mater.specularTexture);
	glUniform1i(glGetUniformLocation(shader.program, "material.specular"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mater.normalMapTexture);
	glUniform1i(glGetUniformLocation(shader.program, "material.normal"), 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, mater.emissionTexture);
	glUniform1i(glGetUniformLocation(shader.program, "material.emission"), 3);
}
*/

#define MATERIAL_MY_IMPL_H
#endif