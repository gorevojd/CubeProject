#ifndef ASSIMP_MODEL_LOADER

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../platform_code.h"

#include "../FileSystem/SOIL_texture_loader.h"

#include "../Animation/animation_system.h"

glm::mat4 AiMatToGLM(aiMatrix4x4 aiMatr){
	glm::mat4 res;
	res = glm::mat4(
		aiMatr.a1, aiMatr.a2, aiMatr.a3, aiMatr.a4,
		aiMatr.b1, aiMatr.b2, aiMatr.b3, aiMatr.b4,
		aiMatr.c1, aiMatr.c2, aiMatr.c3, aiMatr.c4,
		aiMatr.d1, aiMatr.d2, aiMatr.d3, aiMatr.d4);
	return res;
}

glm::vec3 AiVec3ToGLM(aiVector3D aiVec){
	glm::vec3 res;
	res.x = aiVec.x;
	res.y = aiVec.y;
	res.z = aiVec.z;
	return res;
}

glm::vec2 AiVec2ToGLM(aiVector2D aiVec){
	glm::vec2 res;
	res.x = aiVec.x;
	res.y = aiVec.y;
	return res;
}

glm::quat AiQuatToGLM(aiQuaternion quate){
	glm::quat res;
	res.x = quate.x;
	res.y = quate.y;
	res.z = quate.z;
	res.w = quate.w;
	return res;
}

void LoadBones(u32 meshIndex, const aiMesh* pMesh, Model* model)
{

	for (int i = 0; i < pMesh->mNumBones; i++){
		Joint joint;

		u32 boneId;

		joint.name = std::string(pMesh->mBones[i]->mName.data);
		joint.invBindPose = (glm::mat4x3)AiMatToGLM(pMesh->mBones[i]->mOffsetMatrix);

		if (model->skeleton->boneMapping.find(joint.name) == model->skeleton->boneMapping.end()){
			boneId = model->skeleton->boneMapping.size();
		}
		else{
			boneId = model->skeleton->boneMapping[joint.name];
		}

		for (int k = 0; k < pMesh->mBones[i]->mNumWeights; k++){
			float weight = pMesh->mBones[i]->mWeights[k].mWeight;
			float vertId = pMesh->mBones[i]->mWeights[k].mVertexId;

			for (int z = 0; z < 4; z++){
				if (model->meshes[meshIndex].skinnedVertices[vertId].Weights[z] == 0.0f){
					model->meshes[meshIndex].skinnedVertices[vertId].Weights[z] = weight;
					model->meshes[meshIndex].skinnedVertices[vertId].BoneIDs[z] = boneId;
					break;
				}
			}
		}

		model->skeleton->joints.push_back(joint);
	}
}

GLint TextureFromFile(const char* path, std::string directory)
{
	//Generate texture ID and load texture data 
	std::string filename = std::string(path);
	filename = directory + '/' + filename;
	GLuint textureID;
	loaded_texture tempTex = LoadTexture(filename.c_str());
	textureID = tempTex.texture;
	return textureID;
}

std::vector<MeshTexture> LoadMaterialTextures(Model* model, aiMaterial* mat, aiTextureType type){
	std::vector<MeshTexture> textures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		GLboolean skip = false;
		for (GLuint j = 0; j < model->textures_loaded.size(); j++)
		{

			if (model->textures_loaded[j].path == std::string(str.C_Str()))
			{
				textures.push_back(model->textures_loaded[j]);
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   
			// If texture hasn't been loaded already, load it
			MeshTexture texture;
			texture.TextureID = TextureFromFile(str.C_Str(), model->directory);
			switch (type){
			case aiTextureType_DIFFUSE:{
				texture.type = DIFFUSE_TEXTURE;
			}break;
			case aiTextureType_SPECULAR:{
				texture.type = SPECULAR_TEXTURE;
			}break;
			case aiTextureType_HEIGHT:{
				texture.type = NORMAL_MAP_TEXTURE;
			}break;
			case aiTextureType_NORMALS:{
				texture.type = NORMAL_MAP_TEXTURE;
			}break;
			case aiTextureType_EMISSIVE:{
				texture.type = EMISSION_TEXTURE;
			}break;
			}
			texture.path = std::string(str.C_Str());
			textures.push_back(texture);
			model->textures_loaded.push_back(texture);
		}
	}
	return textures;
}

Mesh ProcessMesh(Model* model, aiMesh* mesh, const aiScene* scene)
{

	if (!mesh->HasBones()){

		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<MeshTexture> textures;

		for (GLuint i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector;

			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;

			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;

			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vec;
				// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else{
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			}
			vertices.push_back(vertex);
		}

		for (GLuint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (GLuint j = face.mNumIndices; j > 0; j--){
				indices.push_back(face.mIndices[j - 1]);
			}
			if (face.mNumIndices == 3){

				v3 tangent;
				v3 bitangent;

				v3 vert1 = AiVec3ToGLM(mesh->mVertices[face.mIndices[0]]);
				v3 vert2 = AiVec3ToGLM(mesh->mVertices[face.mIndices[1]]);
				v3 vert3 = AiVec3ToGLM(mesh->mVertices[face.mIndices[2]]);

				v2 uv1 = (v2)AiVec3ToGLM(mesh->mTextureCoords[0][face.mIndices[0]]);
				v2 uv2 = (v2)AiVec3ToGLM(mesh->mTextureCoords[0][face.mIndices[1]]);
				v2 uv3 = (v2)AiVec3ToGLM(mesh->mTextureCoords[0][face.mIndices[2]]);
				
				v3 edge1 = vert2 - vert1;
				v3 edge2 = vert3 - vert1;

				v2 deltaUV1 = uv2 - uv1;
				v2 deltaUV2 = uv3 - uv1;

				float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

				tangent.x = f *(deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
				tangent.y = f *(deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
				tangent.z = f *(deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
				tangent = glm::normalize(tangent);
				
				vertices[face.mIndices[0]].Tangent = tangent;
				vertices[face.mIndices[1]].Tangent = tangent;
				vertices[face.mIndices[2]].Tangent = tangent;
			}
			
		}

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			std::vector<MeshTexture> diffuseMaps = LoadMaterialTextures(model, material, aiTextureType_DIFFUSE);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			std::vector<MeshTexture> specularMaps = LoadMaterialTextures(model, material, aiTextureType_SPECULAR);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			std::vector<MeshTexture> normalMaps = LoadMaterialTextures(model, material, aiTextureType_HEIGHT);
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		}

		return Mesh(vertices, indices, textures);
	}
	else{
		std::vector<SkinnedVertex> vertices;
		std::vector<GLuint> indices;
		std::vector<MeshTexture> textures;

		for (GLuint i = 0; i < mesh->mNumVertices; i++)
		{
			SkinnedVertex vertex;
			glm::vec3 vector;

			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;

			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;

			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = 1.0f - mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else{
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			}
			vertices.push_back(vertex);
		}

		for (GLuint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (GLuint j = face.mNumIndices; j > 0; j--){
				indices.push_back(face.mIndices[j - 1]);
			}
			if (face.mNumIndices == 3){

				v3 tangent;
				v3 bitangent;

				v3 vert1 = AiVec3ToGLM(mesh->mVertices[face.mIndices[0]]);
				v3 vert2 = AiVec3ToGLM(mesh->mVertices[face.mIndices[1]]);
				v3 vert3 = AiVec3ToGLM(mesh->mVertices[face.mIndices[2]]);

				v2 uv1 = (v2)AiVec3ToGLM(mesh->mTextureCoords[0][face.mIndices[0]]);
				v2 uv2 = (v2)AiVec3ToGLM(mesh->mTextureCoords[0][face.mIndices[1]]);
				v2 uv3 = (v2)AiVec3ToGLM(mesh->mTextureCoords[0][face.mIndices[2]]);

				v3 edge1 = vert2 - vert1;
				v3 edge2 = vert3 - vert1;

				v2 deltaUV1 = uv2 - uv1;
				v2 deltaUV2 = uv3 - uv1;

				float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

				tangent.x = f *(deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
				tangent.y = f *(deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
				tangent.z = f *(deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
				tangent = glm::normalize(tangent);

				vertices[face.mIndices[0]].Tangent = tangent;
				vertices[face.mIndices[1]].Tangent = tangent;
				vertices[face.mIndices[2]].Tangent = tangent;
			}
		}

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			std::vector<MeshTexture> diffuseMaps = LoadMaterialTextures(model, material, aiTextureType_DIFFUSE);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			std::vector<MeshTexture> specularMaps = LoadMaterialTextures(model, material, aiTextureType_SPECULAR);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			std::vector<MeshTexture> normalMaps = LoadMaterialTextures(model, material, aiTextureType_HEIGHT);
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		}

		return Mesh(vertices, indices, textures);
	}

}

void ProcessNode(Model* model, aiNode* node, const aiScene* scene)
{

	for (GLuint i = 0; i < node->mNumMeshes; i++){
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh pushMesh = ProcessMesh(model, mesh, scene);
		model->meshes.push_back(pushMesh);
		LoadBones(i, mesh, model);
	}

	for (GLuint i = 0; i < node->mNumChildren; i++){
		ProcessNode(model, node->mChildren[i], scene);
	}
}

enum load_skeleton_options{
	CREATE_SKELETON_FROM_THIS_MODEL,
	USE_EXISTING_SKELETON
};

inline Model ASSIMP_LoadModel(std::string path,
	float scale,
	load_skeleton_options skeletonOption,
	Skeleton* skeleton = 0)
{
	Model model;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		path.c_str(),
		aiProcess_Triangulate |
		aiProcess_GenNormals | 
		aiProcess_CalcTangentSpace
		);

	model.directory = path.substr(0, path.find_last_of('/'));
	model.localTransformation = DefaultTransform();

	if (skeletonOption == CREATE_SKELETON_FROM_THIS_MODEL){
		model.skeleton = new Skeleton();
	}
	else if (skeletonOption == USE_EXISTING_SKELETON){
		model.skeleton = skeleton;
	}

	TransformScale(model.localTransformation, Vector3(scale));
	ProcessNode(&model, scene->mRootNode, scene);
	return model;
}


#define ASSIMP_MODEL_LOADER
#endif