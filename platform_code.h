#ifndef PLATFORM_CODE_H

#include "common_code.h"

#include "Rendering/lights_code.h"
#include "Rendering/materials_code.h"

#include "class_input.h"
#include "class_fonts.h"
#include "mesh_builder.h"
#include "cube_mesh_builder.h"

#include "Debugging/debugging_system.h"

//TODO(Dima): Chineese glyphs support
//TODO(Dima): Text bounding box for rendering text
//TODO(Dima): Text groups to output

//TODO(Dima): Framebuffers support
//TODO(Dima): Shadows rendering
//TODO(Dima): Fog rendering
//TODO(Dima): Bloom
//TODO(Dima): Normal mapping
//TODO(Dima): Per-primitive anti-aliasing

//TODO(Dima): Skeletal animation
//TODO(Dima): PhysX integration
//TODO(Dima): Debugging system

struct game_settings{
	bool AntialiasingOn;
	int Antialiasing;

	int windowWidth;
	int windowHeight;
	
	bool Fullscreen;
};

enum asset_type{
	ASSET_SHADER,
	ASSET_TEXTURE,
	ASSET_FONT,
	ASSET_SOUND
};

enum game_shader_type{
	SIMPLE_SHADER,
	CUBE_SHADER,
	SKYBOX_SHADER,
	TEXT_SHADER,
	SCREEN_SHADER,
	DEPTH_SHADER,
};

struct game_shader{
	game_shader_type type;
	u32 program;
};

struct game_screen_buffer{
	int Width;
	int Height;

	int currentWidth;
	int currentHeight;

	void* Memory;
};

struct game_time{
	float time;
	float deltaTime;
	float fps;
	s64 freq;
	s64 count;
};

struct game_framebuffer{
	u32 VAO;
	u32 VBO;
	u32 FramebufferId;
	u32 AttachmentTextureId;

	u32 Width;
	u32 Height;
};

struct game_camera{
	v3 position;

	float pitch;
	float yaw;
	float roll;

	v3 front;
	v3 up;
	v3 right;

	game_camera(){
		this->position = v3(0.0f);

		this->pitch = 0.0f;
		this->yaw = 0.0f;
		this->roll = 0.0f;

		this->up = v3(0.0f, 1.0f, 0.0f);
		this->front = v3(0.0f, 0.0f, 1.0f);
	}
};

struct game_engine_state{
	game_screen_buffer* ScreenBuffer;
	game_input* Input;
	game_camera* Camera;
	game_time* Time;

	std::vector<voxel_mesh_chunk> MeshThreads;
	std::vector<Model> Models;

	game_shader MainShader;
	game_shader TextShader;
	game_shader CubeShader;
	game_shader ScreenShader;
	game_shader DepthShader;
	game_shader CubemapShader;

	character_info* Characters;
	character_atlas Atlas;

	u32 textVAO;
	u32 textVBO;

	u32 cubemapVAO;
	u32 cubemapTexture;
	
	game_framebuffer mainFramebuffer;
};

#define GAME_UPDATE_AND_RENDER(name) void name(game_engine_state* State)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);
GAME_UPDATE_AND_RENDER(GameUpdateAndRender);

void UniformMaterial(game_shader shader, phong_material pmat, std::string unVarName);
void UniformPointLight(game_shader shader, point_light lit, std::string unVarName);
void UniformDirectionalLight(game_shader shader, directional_light lit, std::string unVarName);

struct debug_read_file_result{
	void* contents;
	u32 size;
};

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) debug_read_file_result name(const char* fileName)
typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);

#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void name(void* Memory)
typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(debug_platform_free_file_memory);

struct game_memory{
	debug_platform_read_entire_file* DEBUGPlatformReadEntireFile;
	debug_platform_free_file_memory* DEBUGPlatformFreeFileMemory;
};

#define PLATFORM_CODE_H
#endif