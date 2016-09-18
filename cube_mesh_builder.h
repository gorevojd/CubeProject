#ifndef CUBE_MESH_BUILDER_HWTF

#include "common_code.h"
#include <GL/glew.h>

struct voxel_cube{
	bool IsActive;
	v3 Pos;

	voxel_cube(v3 pos){
		this->IsActive = true;
		this->Pos = pos;
	}
};

struct voxel_mesh_chunk{
	std::vector<voxel_cube> Voxels;

	u32 VAO;
	u32 VBO;
	u32 EBO;
};

inline void PushVoxelToMeshChunk(voxel_mesh_chunk& cMesh, voxel_cube Vox){
	cMesh.Voxels.push_back(Vox);
}

inline void PushVoxelToMeshThreads(std::vector<voxel_mesh_chunk>& threads, voxel_cube Vox, int threadID){
	PushVoxelToMeshChunk(threads[threadID], Vox);
}

inline void SetupVoxelMeshChunk(voxel_mesh_chunk& vMesh){
	glGenVertexArrays(1, &vMesh.VAO);
	glGenBuffers(1, &vMesh.VBO);
	glGenBuffers(1, &vMesh.EBO);
}

inline void SetupVoxelMeshThreads(std::vector<voxel_mesh_chunk>& threads){
	for (int i = 0; i < threads.size(); i++){
		SetupVoxelMeshChunk(threads[i]);
	}
}

inline void BuildVoxelMeshChunk(voxel_mesh_chunk& vMesh){
	struct temp_vertex{
		v3 Pos;
		v3 Normal;
		v2 TexCoord;
	};

	struct temp_wtf{
		u32 vert0;
		u32 vert1;
		u32 vert2;
		u32 vert3;
		u32 vert4;
		u32 vert5;
	};

	u32 n = 0;
	u32 f = 0;

	temp_vertex* vertices = new temp_vertex[24 * vMesh.Voxels.size()];
	temp_wtf* indices = new temp_wtf[6 * vMesh.Voxels.size()];

	std::vector<voxel_cube>::iterator it = vMesh.Voxels.begin();
	for (int i = 0; it != vMesh.Voxels.end(); it++, i+=6){
		v3 Pos = (*it).Pos;
		vertices[n++] = { { Pos.x - 0.5f, Pos.y - 0.5f, Pos.z + 0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } };
		vertices[n++] = { { Pos.x + 0.5f, Pos.y - 0.5f, Pos.z + 0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } };
		vertices[n++] = { { Pos.x + 0.5f, Pos.y - 0.5f, Pos.z - 0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } };
		vertices[n++] = { { Pos.x - 0.5f, Pos.y - 0.5f, Pos.z - 0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } };

		vertices[n++] = { { Pos.x - 0.5f, Pos.y + 0.5f, Pos.z - 0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } };
		vertices[n++] = { { Pos.x + 0.5f, Pos.y + 0.5f, Pos.z - 0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } };
		vertices[n++] = { { Pos.x + 0.5f, Pos.y + 0.5f, Pos.z + 0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } };
		vertices[n++] = { { Pos.x - 0.5f, Pos.y + 0.5f, Pos.z + 0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } };

		vertices[n++] = { { Pos.x - 0.5f, Pos.y + 0.5f, Pos.z - 0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
		vertices[n++] = { { Pos.x - 0.5f, Pos.y + 0.5f, Pos.z + 0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };
		vertices[n++] = { { Pos.x - 0.5f, Pos.y - 0.5f, Pos.z + 0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
		vertices[n++] = { { Pos.x - 0.5f, Pos.y - 0.5f, Pos.z - 0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };

		vertices[n++] = { { Pos.x + 0.5f, Pos.y + 0.5f, Pos.z + 0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
		vertices[n++] = { { Pos.x + 0.5f, Pos.y + 0.5f, Pos.z - 0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };
		vertices[n++] = { { Pos.x + 0.5f, Pos.y - 0.5f, Pos.z - 0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
		vertices[n++] = { { Pos.x + 0.5f, Pos.y - 0.5f, Pos.z + 0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };

		vertices[n++] = { { Pos.x + 0.5f, Pos.y + 0.5f, Pos.z - 0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } };
		vertices[n++] = { { Pos.x - 0.5f, Pos.y + 0.5f, Pos.z - 0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } };
		vertices[n++] = { { Pos.x - 0.5f, Pos.y - 0.5f, Pos.z - 0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } };
		vertices[n++] = { { Pos.x + 0.5f, Pos.y - 0.5f, Pos.z - 0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } };

		vertices[n++] = { { Pos.x - 0.5f, Pos.y + 0.5f, Pos.z + 0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } };
		vertices[n++] = { { Pos.x + 0.5f, Pos.y + 0.5f, Pos.z + 0.5f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } };
		vertices[n++] = { { Pos.x + 0.5f, Pos.y - 0.5f, Pos.z + 0.5f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } };
		vertices[n++] = { { Pos.x - 0.5f, Pos.y - 0.5f, Pos.z + 0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } };


		indices[i + 0] = { f + 0, f + 1, f + 2, f + 0, f + 2, f + 3 };
		indices[i + 1] = { f + 4, f + 5, f + 6, f + 4, f + 6, f + 7 };
		indices[i + 2] = { f + 8, f + 9, f + 10, f + 8, f + 10, f + 11 };
		indices[i + 3] = { f + 12, f + 13, f + 14, f + 12, f + 14, f + 15 };
		indices[i + 4] = { f + 16, f + 17, f + 18, f + 16, f + 18, f + 19 };
		indices[i + 5] = { f + 20, f + 21, f + 22, f + 20, f + 22, f + 23 };

		f += 24;
	}

	glBindVertexArray(vMesh.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, vMesh.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24 * 8 * vMesh.Voxels.size(), vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vMesh.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat) * 36 * vMesh.Voxels.size(), indices, GL_DYNAMIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	delete[] indices;
	delete[] vertices;
}

inline void BuildVoxelMeshThreads(std::vector<voxel_mesh_chunk>& threads){
	for (int i = 0; i < threads.size(); i++){
		BuildVoxelMeshChunk(threads[i]);
	}
}

#define CUBE_MESH_BUILDER_HWTF
#endif