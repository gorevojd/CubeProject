#ifndef OPENGL_CODE_H

#include <GL/glew.h>

#include "common_code.h"
#include "platform_code.h"

void RenderMesh(const Mesh* mesh, game_shader shader){
	//glUseProgram(shader.program);
	glUniform1i(glGetUniformLocation(shader.program, "material.withNormalMap"), mesh->withNormalMap);

	std::vector<GLint> locations;
	locations.push_back(glGetUniformLocation(shader.program, "material.diffuse"));
	locations.push_back(glGetUniformLocation(shader.program, "material.specular"));
	locations.push_back(glGetUniformLocation(shader.program, "material.normal"));
	locations.push_back(glGetUniformLocation(shader.program, "material.emission"));


	for (int i = 0; i < mesh->textures.size(); i++){
#if 0
		switch (mesh->textures[i].type){
		case DIFFUSE_TEXTURE:{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mesh->textures[i].TextureID);
			glUniform1i(glGetUniformLocation(shader.program, "material.diffuse"), 0);
		}break;
		case SPECULAR_TEXTURE:{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mesh->textures[i].TextureID);
			glUniform1i(glGetUniformLocation(shader.program, "material.specular"), 1);
		}break;
		case NORMAL_MAP_TEXTURE:{
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, mesh->textures[i].TextureID);
			glUniform1i(glGetUniformLocation(shader.program, "material.normal"), 2);
		}break;
		case EMISSION_TEXTURE:{
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, mesh->textures[i].TextureID);
			glUniform1i(glGetUniformLocation(shader.program, "material.emission"), 3);
		}break;
		}
#else
		glActiveTexture(GL_TEXTURE0 + mesh->textures[i].type);
		glBindTexture(GL_TEXTURE_2D, mesh->textures[i].TextureID);
		glUniform1i(locations[mesh->textures[i].type], mesh->textures[i].type);
#endif
	}

	glBindVertexArray(mesh->VAO);
	glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUniform1i(glGetUniformLocation(shader.program, "material.withNormalMap"), 0);
	for (GLuint i = 0; i < mesh->textures.size(); i++){
		glActiveTexture(GL_TEXTURE0 + mesh->textures[i].type);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	//glUseProgram(0);
}

void RenderModel(const Model* model, game_shader shader){
	glUseProgram(shader.program);
	GLint ModelLocation = glGetUniformLocation(shader.program, "model");
	matrix4x4 mat = GetTransformationMatrix(model->localTransformation);
	glUniformMatrix4fv(ModelLocation, 1, GL_TRUE, &GetTransformationMatrix(model->localTransformation).data[0][0]);
	//glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	for (int i = 0; i < model->meshes.size(); i++){
		RenderMesh(&model->meshes[i], shader);
	}
	glUseProgram(0);
}

void RenderFramebuffer(const game_framebuffer& framebuffer, game_shader shader){
	glUseProgram(shader.program);
	glBindVertexArray(framebuffer.VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebuffer.AttachmentTextureId);
	//glUniform1i(glGetUniformLocation(shader.program, "screenTexture"), 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glUseProgram(0);
}

void RenderVoxelMeshThreads(std::vector<voxel_mesh_chunk>& meshThreads, game_shader shader, u32 diffTex, u32 specTex, u32 emissTex){
	glUseProgram(shader.program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffTex);
	glUniform1i(glGetUniformLocation(shader.program, "material.diffuse"), 0);

#if 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specTex);
	glUniform1i(glGetUniformLocation(shader.program, "material.specular"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, emissTex);
	glUniform1i(glGetUniformLocation(shader.program, "material.emission"), 2);
#endif

	for (int i = 0; i < meshThreads.size(); i++){
		glBindVertexArray(meshThreads[i].VAO);
		glDrawElements(GL_TRIANGLES, 36 * meshThreads[i].Voxels.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
}

//NOTE(Dima): Very well optimized function
void RenderText(std::string text, v2 pos, character_atlas atlas, game_shader shader, u32 VAO, u32 VBO, int windowWidth, int windowHeight, float scale = 1.0f, v3 color = v3(1.0f, 0.0f, 1.0f)){
	glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight);

	glUseProgram(shader.program);
	GLint projLoc = glGetUniformLocation(shader.program, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glActiveTexture(GL_TEXTURE0);
	glUniform3f(glGetUniformLocation(shader.program, "glyphColor"), color.r, color.g, color.b);
	
	struct t_point {
		GLfloat x;
		GLfloat y;
		GLfloat s;
		GLfloat t;
	};

	t_point* vertices = new t_point[6 * text.length()];

	int n = 0;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, atlas.TextureID);
	glBindVertexArray(VAO);
	for (int i = 0; i < text.length(); i++){
		atlas_character_info acinfo = atlas.Characters[(int)text.c_str()[i]];

		GLfloat xpos = pos.x + acinfo.BearingX * scale;
		GLfloat ypos = pos.y - (acinfo.Height - acinfo.BearingY) * scale;

		GLfloat w = acinfo.Width * scale;
		GLfloat h = acinfo.Height * scale;

		vertices[n++] = { xpos, ypos + h, acinfo.tCoordsX1, acinfo.tCoordsY2 };
		vertices[n++] = { xpos + w, ypos, acinfo.tCoordsX2, acinfo.tCoordsY1 };
		vertices[n++] = { xpos, ypos, acinfo.tCoordsX1, acinfo.tCoordsY1 };
		vertices[n++] = { xpos, ypos + h, acinfo.tCoordsX1, acinfo.tCoordsY2 };
		vertices[n++] = { xpos + w, ypos + h, acinfo.tCoordsX2, acinfo.tCoordsY2 };
		vertices[n++] = { xpos + w, ypos, acinfo.tCoordsX2, acinfo.tCoordsY1 };

		pos.x += (acinfo.AdvanceX >> 6) * scale;
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24 * text.length(), vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_TRIANGLES, 0, n);

	glBindVertexArray(0);
	glUseProgram(0);

	delete[] vertices;
}



#define OPENGL_CODE_H
#endif