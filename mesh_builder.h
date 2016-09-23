#ifndef MESH_BUILDER

#include "common_code.h"
#include <GL/glew.h>

#include "Animation\animation_system.h"


enum texture_type{
	DIFFUSE_TEXTURE = 0,
	SPECULAR_TEXTURE,
	NORMAL_MAP_TEXTURE,
	EMISSION_TEXTURE
};

struct Vertex{
	v3 Position;
	v3 Normal;
	v2 TexCoords;
	v3 Tangent;
};

struct SkinnedVertex{
	v3 Position;
	v3 Normal;
	v2 TexCoords;
	v3 Tangent;
	v3 Bitangent;
	float Weights[INFLUENCE_BONE_COUNT];
	int BoneIDs[INFLUENCE_BONE_COUNT];
};

struct MeshTexture{
	u32 TextureID;
	texture_type type;
	std::string path;
};

enum mesh_type{
	SIMPLE_TRIANGLE_MESH,
	SKINNED_MESH
};

struct Mesh{

	mesh_type meshType;

	std::vector<Vertex> vertices;
	std::vector<SkinnedVertex> skinnedVertices;
	std::vector<u32> indices;
	std::vector<MeshTexture> textures;

	bool withNormalMap;

	u32 VAO;
	u32 VBO;
	u32 EBO;

	Mesh(
		std::vector<Vertex> vertices,
		std::vector<u32> indices,
		std::vector<MeshTexture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		this->withNormalMap = false;
		for (std::vector<MeshTexture>::const_iterator it = this->textures.cbegin();
			it != this->textures.end();
			it++)
		{
			if ((*it).type == NORMAL_MAP_TEXTURE){
				this->withNormalMap = true;
				break;
			}
		}

		SetupMesh();
	}

	Mesh(
		std::vector<SkinnedVertex> skinnedVertices,
		std::vector<u32> indices,
		std::vector<MeshTexture> textures)
	{
		this->skinnedVertices = skinnedVertices;
		this->indices = indices;
		this->textures = textures;

		this->withNormalMap = false;
		for (std::vector<MeshTexture>::const_iterator it = this->textures.cbegin();
			it != this->textures.end();
			it++)
		{
			if ((*it).type == NORMAL_MAP_TEXTURE){
				this->withNormalMap = true;
			}
		}

		SetupSkinnedMesh();
	}

	/*
		layout(location = 0) in vec3 position;
		layout(location = 1) in vec3 normal;
		layout(location = 2) in vec2 texCoord;
		layout(location = 3) in vec3 tangent;
		layout(location = 4) in vec3 bitangent;
		layout(location = 5) in vec4 Weights;
		layout(location = 6) in vec4 BoneIDs;
	*/

	void SetupMesh(){
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);

		glBindVertexArray(this->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(u32), &this->indices[0], GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Position));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}

	void SetupSkinnedMesh(){
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);

		glBindVertexArray(this->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, this->skinnedVertices.size() * sizeof(SkinnedVertex), &this->skinnedVertices[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(u32), &this->indices[0], GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (GLvoid*)offsetof(SkinnedVertex, Position));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (GLvoid*)offsetof(SkinnedVertex, Normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (GLvoid*)offsetof(SkinnedVertex, TexCoords));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (GLvoid*)offsetof(SkinnedVertex, Tangent));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4 * INFLUENCE_BONE_COUNT, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (GLvoid*)offsetof(SkinnedVertex, Weights));
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4 * INFLUENCE_BONE_COUNT, GL_INT, sizeof(SkinnedVertex), (GLvoid*)offsetof(SkinnedVertex, BoneIDs));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};

struct Model{
	std::vector<Mesh> meshes;
	std::vector<MeshTexture> textures_loaded;
	std::string directory;

	transform localTransformation;
	
	Skeleton* skeleton;
};

#define MESH_BUILDER
#endif