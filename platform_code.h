#ifndef PLATFORM_CODE_H

#define ENGINE_DEBUG_MODE 1

#include "common_code.h"

#include "Rendering/lights_code.h"
#include "Rendering/materials_code.h"

#include "class_fonts.h"
#include "mesh_builder.h"
#include "cube_mesh_builder.h"

#include "FileSystem\\asset_system.h"

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

struct debug_read_file_result{
	void* contents;
	u32 size;
};

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) debug_read_file_result name(const char* fileName)
typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);

#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) bool name(const char* FileName, void* Memory, u32 MemorySize)
typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(debug_platform_write_entire_file);

#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void name(void* Memory)
typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(debug_platform_free_file_memory);

enum{
	DebugCycleCounter_GameUpdateAndRender,
	DebugCycleCounter_Count
};


struct debug_cycle_counter{
	u64 CycleCount;
};

struct game_memory{
	bool IsInitialized;

	u32 PermanentStorageSize;
	void* PermanentStorage;

	u32 TransientStorageSize;
	void* TransientStorage;

	debug_platform_read_entire_file* DEBUGPlatformReadEntireFile;
	debug_platform_write_entire_file* DEBUGPlatformWriteEntireFile;
	debug_platform_free_file_memory* DEBUGPlatformFreeFileMemory;

#if ENGINE_DEBUG_MODE
	debug_cycle_counter Counters[256];
#endif
};

struct game_engine_state{

	game_time* Time;
	game_camera* Camera;
	std::vector<voxel_mesh_chunk> MeshThreads;
	std::vector<Model> Models;

	loaded_shader MainShader;
	loaded_shader TextShader;
	loaded_shader CubeShader;
	loaded_shader ScreenShader;
	loaded_shader DepthShader;
	loaded_shader CubemapShader;

	character_info* Characters;
	character_atlas Atlas;

	u32 TextVAO;
	u32 TextVBO;

	//soil_loaded_texture CubemapTexture;
	u32 CubemapVAO;

	game_framebuffer MainFramebuffer;
};

struct platform_dependent_context{
	game_time* Time;
	game_camera* Camera;
	character_info* Characters;
	character_atlas Atlas;
};

struct game_button_state{
	bool IsDown;
	bool WasDown;
};

struct game_input_controller{

	bool CapturingMouse;

	union{
		struct{
			float MouseX;
			float MouseY;
		};
		float MouseP[2];
	};

	float DeltaMouseX;
	float DeltaMouseY;

	game_button_state LeftKey;
	game_button_state RightKey;
	game_button_state UpKey;
	game_button_state DownKey;

	game_button_state SpaceKey;
	game_button_state ShiftKey;
};

struct game_input{

	game_camera Camera;
	game_time Time;

	union{
		struct{
			game_input_controller controller0;
			game_input_controller controller1;
			game_input_controller controller2;
			game_input_controller controller3;
		};
		game_input_controller controllers[4];
	};
	game_input(){

	}
};


#define GAME_UPDATE_AND_RENDER(name) void name(game_memory* Memory, game_input* Input, game_screen_buffer* ScreenBuffer, platform_dependent_context* DepContext)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);
GAME_UPDATE_AND_RENDER(GameUpdateAndRender);

void UniformMaterial(loaded_shader shader, phong_material pmat, std::string unVarName);
void UniformPointLight(loaded_shader shader, point_light lit, std::string unVarName);
void UniformDirectionalLight(loaded_shader shader, directional_light lit, std::string unVarName);


extern game_memory* DebugGlobalMemory;
#if _MSC_VER
#define BEGIN_TIMED_BLOCK(ID) u32 StartCycleCount##ID= __rdtsc();
#define END_TIMED_BLOCK(ID) Memory->Counters[DebugCycleCounter_##ID] += __rdtsc() - StartSycleCount;
#else
#define BEGIN_TIMED_BLOCK(ID)
#define END_TIMED_BLOCK(ID)
#endif


#define PLATFORM_CODE_H
#endif
