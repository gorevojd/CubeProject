#ifndef DEBUGGING_SYSTEM_H

#include "../common_code.h"

enum rendering_state_shader_type{
	PHONG_SHADER_TYPE = 0,
	BLINN_PHONG_SHADER_TYPE,
};

struct rendering_state{
	rendering_state_shader_type ShaderType;
	
	u32 TriangleCount;
	u32 VertexCount;
};

struct debug_state{
	std::vector<std::string> Log;

	float FPS;
	float DeltaTime;

	rendering_state RenderState;
};

struct debug_state_menu_node{

};

struct debug_state_menu{

};

#define DEBUGGING_SYSTEM_H
#endif;