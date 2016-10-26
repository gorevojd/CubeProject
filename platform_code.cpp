#include <GL/glew.h>

#include "platform_code.h"
#include "hardware_accelerated.h"

#include "FileSystem\\asset_system.h"
#include "FileSystem\\SOIL_texture_loader.h"
#include "FileSystem\\FBX_SDK_model_loader.h"
#include "FileSystem\\ASSIMP_model_loader.h"

void UniformMaterial(loaded_shader shader, phong_material pmat, std::string unVarName){
	glActiveTexture(GL_TEXTURE0);
	if (pmat.diffuseTexture){
		glBindTexture(GL_TEXTURE_2D, pmat.diffuseTexture);
		glUniform1i(glGetUniformLocation(shader.program, (unVarName + ".diffuse").c_str()), 0);
	}
	else{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glActiveTexture(GL_TEXTURE1);
	if (pmat.specularTexture){
		glBindTexture(GL_TEXTURE_2D, pmat.specularTexture);
		glUniform1i(glGetUniformLocation(shader.program, (unVarName + ".specular").c_str()), 1);
	}
	else{

	}

	glActiveTexture(GL_TEXTURE2);
	if (pmat.normalMapTexture){
		glBindTexture(GL_TEXTURE_2D, pmat.normalMapTexture);
		glUniform1i(glGetUniformLocation(shader.program, (unVarName + ".normal").c_str()), 2);
	}
	else{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glActiveTexture(GL_TEXTURE3);
	if (pmat.emissionTexture){
		glBindTexture(GL_TEXTURE_2D, pmat.emissionTexture);
		glUniform1i(glGetUniformLocation(shader.program, (unVarName + ".emission").c_str()), 3);
	}
	else{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glUniform1f(glGetUniformLocation(shader.program, (unVarName + ".shininess").c_str()), 32.0f);
}

//NOTE(Dima): Need to glUseProgram before call
void UniformPointLight(loaded_shader shader, point_light lit, std::string unVarName){
	GLint shaderLitAmbientLoc = glGetUniformLocation(shader.program, (unVarName + ".ambient").c_str());
	GLint shaderLitDiffuseLoc = glGetUniformLocation(shader.program, (unVarName + ".diffuse").c_str());
	GLint shaderLitSpecularLoc = glGetUniformLocation(shader.program, (unVarName + ".specular").c_str());
	GLint shaderLitPositionLoc = glGetUniformLocation(shader.program, (unVarName + ".position").c_str());
	GLint shaderLitConstLoc = glGetUniformLocation(shader.program, (unVarName + ".constant").c_str());
	GLint shaderLitLinearLoc = glGetUniformLocation(shader.program, (unVarName + ".linear").c_str());
	GLint shaderLitQuadraticLoc = glGetUniformLocation(shader.program, (unVarName + ".quadratic").c_str());

	std::string tempst = (unVarName + ".quadratic");

	glUniform3f(shaderLitAmbientLoc, lit.ambient, lit.ambient, lit.ambient);
	glUniform3f(shaderLitDiffuseLoc,  lit.diffuse.r, lit.diffuse.g, lit.diffuse.b);
	glUniform3f(shaderLitSpecularLoc, lit.specular.r, lit.specular.g, lit.specular.b);
	glUniform3f(shaderLitPositionLoc, lit.position.x, lit.position.y, lit.position.z);
	glUniform1f(shaderLitConstLoc, lit.constant);
	glUniform1f(shaderLitLinearLoc, lit.linear);
	glUniform1f(shaderLitQuadraticLoc, lit.quadratic);
}

//NOTE(Dima): Need to glUseProgram before call
void UniformDirectionalLight(loaded_shader shader, directional_light lit, std::string unVarName){
	GLint dirAmbientLoc = glGetUniformLocation(shader.program, (unVarName + ".ambient").c_str());
	GLint dirDiffuseLoc = glGetUniformLocation(shader.program, (unVarName + ".diffuse").c_str());
	GLint dirSpecularLoc = glGetUniformLocation(shader.program, (unVarName + ".specular").c_str());
	GLint dirDirectionLoc = glGetUniformLocation(shader.program, (unVarName + ".direction").c_str());

	glUniform3f(dirAmbientLoc, lit.ambient, lit.ambient, lit.ambient);
	glUniform3f(dirDiffuseLoc, lit.diffuse.r, lit.diffuse.g, lit.diffuse.b);
	glUniform3f(dirSpecularLoc, lit.specular.r, lit.specular.g, lit.specular.b);
	glUniform3f(dirDirectionLoc, lit.direction.x, lit.direction.y, lit.direction.z);
}

void UpdateCameraVectors(game_camera* camera){
	glm::vec3 front;
	front.x = cos(glm::radians(camera->pitch)) * cos(glm::radians(camera->yaw));
	front.y = sin(glm::radians(camera->pitch));							
	front.z = cos(glm::radians(camera->pitch)) * sin(glm::radians(camera->yaw));
	camera->front = glm::normalize(front);

	camera->right = glm::normalize(glm::cross(camera->front, glm::vec3(0.0f, 1.0f, 0.0f)));
	camera->up = glm::normalize(glm::cross(camera->right, camera->front));
}

glm::mat4 CalcDirLightSpaceMatrix(const directional_light& lit){
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	GLfloat nearPlane = 1.0f, farPlane = 7.5f;
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
	lightView = glm::lookAt(-lit.direction, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	lightSpaceMatrix = lightProjection * lightView;
	return lightSpaceMatrix;
}

loaded_shader LoadGameShader(
	debug_platform_read_entire_file* DEBUGReadFile,
	debug_platform_free_file_memory* DEBUGFreeMemory,
	const char* vertexPath,
	const char* fragmentPath)
{
	debug_read_file_result vertShaderFile = DEBUGReadFile(vertexPath);
	debug_read_file_result fragmShaderFile = DEBUGReadFile(fragmentPath);

	const GLchar* vertShaderSrc = (char*)vertShaderFile.contents;
	const GLchar* fragmShaderSrc = (char*)fragmShaderFile.contents;

	loaded_shader result;

	GLuint vertex, fragment;
	GLint success;
	GLchar infoLog[512];
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertShaderSrc, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		//OutputDebugStringA("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n");
		//OutputDebugStringA(infoLog);
		Assert(0);
		//TODO(Dima): Logging
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmShaderSrc, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		//OutputDebugStringA("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n");
		//OutputDebugStringA(infoLog);
		Assert(0);
		//TODO(Dima): Logging
	}

	result.program = glCreateProgram();
	glAttachShader(result.program, vertex);
	glAttachShader(result.program, fragment);
	glLinkProgram(result.program);
	glGetProgramiv(result.program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(result.program, 512, NULL, infoLog);
		//OutputDebugStringA("ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n");
		//OutputDebugStringA(infoLog);
		Assert(0);
		//TODO(Dima): Logging
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	DEBUGFreeMemory(vertShaderFile.contents);
	DEBUGFreeMemory(fragmShaderFile.contents);

	return result;
}

static game_framebuffer SetupFramebuffer(int w, int h, u32 VAO, int samples = 4){
	game_framebuffer res;

#if 1
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	GLuint texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		//TODO(Dima): Logging
		Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

#else
	//NOTE(Dima): Not working
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	GLuint texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, w, h, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture, 0);

	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		//TODO(Dima): Logging
		Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif


	res.FramebufferId = fbo;
	res.AttachmentTextureId = texture;
	res.VAO = VAO;
	res.Width = w;
	res.Height = h;

	return res;
}

static game_framebuffer SetupDepthMapFramebuffer(u32 VAO){
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	const u32 SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0,
		GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	game_framebuffer res;
	res.AttachmentTextureId = depthMap;
	res.FramebufferId = depthMapFBO;
	res.Width = SHADOW_WIDTH;
	res.Height = SHADOW_HEIGHT;
	res.VAO = VAO;

	return res;
}

#if ENGINE_DEBUG_MODE
game_memory* DebugGlobalMemory;
#endif

//extern "C" __declspec(dllexport) GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
#if ENGINE_DEBUG_STATE
	DebugGlobalMemory = Memory;
#endif
	static game_engine_state* State = new game_engine_state;
	if (!Memory->IsInitialized){


		GLfloat cubemapVertices[] = {
			-1.0f, 1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,

			-1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,

			-1.0f, 1.0f, -1.0f,
			1.0f, 1.0f, -1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, 1.0f
		};

		GLfloat CubeVertices[] = {
			//NOTE(Dima): Front side
			-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			//NOTE(Dima): Top side
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			//NOTE(Dima): Right side
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			//NOTE(Dima): Left side
			-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			//NOTE(Dima): Back side
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
			-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
			0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
			//NOTE(Dima): Down side
			-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f
		};

		GLuint CubeIndices[] = {
			0, 1, 2,
			0, 2, 3,

			4, 5, 6,
			4, 6, 7,

			8, 9, 10,
			8, 10, 11,

			12, 13, 14,
			12, 14, 15,

			16, 17, 18,
			16, 18, 19,

			20, 21, 22,
			20, 22, 23
		};

		GLfloat ScreenVertices[] = {
			-1.0f, 1.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f,
			1.0f, -1.0f, 1.0f, 0.0f,

			-1.0f, 1.0f, 0.0f, 1.0f,
			1.0f, -1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 1.0f, 1.0f
		};

		GLuint QuadIndices[] = {
			0, 1, 2,
			0, 2, 3
		};

		GLuint VAO;
		GLuint VBO;
		GLuint EBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), CubeVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CubeIndices), CubeIndices, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		GLuint textVBO;
		GLuint textVAO;
		glGenVertexArrays(1, &textVAO);
		glGenBuffers(1, &textVBO);
		glBindVertexArray(textVAO);
		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		GLuint screenVAO, screenVBO, screenEBO;
		glGenVertexArrays(1, &screenVAO);
		glGenBuffers(1, &screenVBO);
		glGenBuffers(1, &screenEBO);
		glBindVertexArray(screenVAO);
		glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ScreenVertices), ScreenVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		GLuint cubemapVAO, cubemapVBO;
		glGenVertexArrays(1, &cubemapVAO);
		glGenBuffers(1, &cubemapVBO);
		glBindVertexArray(cubemapVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubemapVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapVertices), cubemapVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		State->MainFramebuffer = SetupFramebuffer(ScreenBuffer->currentWidth, ScreenBuffer->currentHeight, screenVAO);

		std::vector<const char*> cubemapFaces;
		cubemapFaces.push_back("../Data/Cubemaps/mp_dejavu/dejavu_ft.jpg");
		cubemapFaces.push_back("../Data/Cubemaps/mp_dejavu/dejavu_bk.jpg");
		cubemapFaces.push_back("../Data/Cubemaps/mp_dejavu/dejavu_up.jpg");
		cubemapFaces.push_back("../Data/Cubemaps/mp_dejavu/dejavu_dn.jpg");
		cubemapFaces.push_back("../Data/Cubemaps/mp_dejavu/dejavu_rt.jpg");
		cubemapFaces.push_back("../Data/Cubemaps/mp_dejavu/dejavu_lf.jpg");

		//State->CubemapTexture = LoadCubemapTexture(cubemapFaces);

		State->MainShader = LoadGameShader(
			Memory->DEBUGPlatformReadEntireFile,
			Memory->DEBUGPlatformFreeFileMemory,
			"../Code/Shaders/main.vs",
			"../Code/Shaders/main.fs");
		State->TextShader = LoadGameShader(
			Memory->DEBUGPlatformReadEntireFile,
			Memory->DEBUGPlatformFreeFileMemory,
			"../Code/Shaders/text.vs",
			"../Code/Shaders/text.fs");
		State->ScreenShader = LoadGameShader(
			Memory->DEBUGPlatformReadEntireFile,
			Memory->DEBUGPlatformFreeFileMemory,
			"../Code/Shaders/screen.vs",
			"../Code/Shaders/screen.fs");
		State->CubemapShader = LoadGameShader(
			Memory->DEBUGPlatformReadEntireFile,
			Memory->DEBUGPlatformFreeFileMemory,
			"../Code/Shaders/cubemap.vs", 
			"../Code/Shaders/cubemap.fs");

		glm::vec3 pointLightPositions[] = {
			glm::vec3(40.0f, 15.0f, 0.0f),
			glm::vec3(-40.0f, 15.0f, 0.0f),
			glm::vec3(0.0f, 15.0f, -40.0f),
			glm::vec3(0.0f, 15.0f, 40.0f)
		};

		glm::vec3 pointLightDiffuseColors[] = {
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		};

		directional_light dirLit(
			v3(0.5f, -0.5f, 0.5f),
			v3(1.0f, 1.0f, 1.0f),
			v3(1.0f, 1.0f, 1.0f),
			0.1f);
		point_light lits[4];
		for (int i = 0; i < 4; i++){
			lits[i] = point_light(pointLightPositions[i], pointLightDiffuseColors[i], pointLightDiffuseColors[i], 0.08f, 500.0f);
		}

		glUseProgram(State->MainShader.program);
		for (int i = 0; i < 4; i++){
			std::stringstream ss;
			ss << i;
			std::string uniformStr = "pointLights[" + ss.str() + "]";
			UniformPointLight(State->MainShader, lits[i], uniformStr);
		}
		UniformDirectionalLight(State->MainShader, dirLit, "dirLight");
		GLint matShineLoc = glGetUniformLocation(State->MainShader.program, "material.shininess");
		glUniform1f(matShineLoc, 16.0f);
		glUseProgram(0);

		FbxManager* GlobalFbxManager = FbxManager::Create();
		State->Models.push_back(FBX_SDK_LoadModel(GlobalFbxManager, "../Data/Models/Scenes/MainScene.fbx", 1.0f));
		//State->Models.push_back(FBX_SDK_LoadModel(GlobalFbxManager, "../Data/Models/DeadAnarchist/Anarchist.fbx", 0.12f));
		GlobalFbxManager->Destroy();

		State->TextVAO = textVAO;
		State->TextVBO = textVBO;
		State->CubemapVAO = VAO;

		Memory->IsInitialized = true;
	}

	//glViewport(0, 0, State->ScreenBuffer->Width, State->ScreenBuffer->Height);

	State->Atlas = DepContext->Atlas;
	State->Characters = DepContext->Characters;
	State->Camera = DepContext->Camera;
	State->Time = DepContext->Time;

	int contrID = 0;
	float leftValue = Input->controllers[contrID].LeftKey.IsDown == true ? 1.0f : 0.0f;
	float rightValue = Input->controllers[contrID].RightKey.IsDown == true ? 1.0f : 0.0f;
	float upValue = Input->controllers[contrID].UpKey.IsDown == true ? 1.0f : 0.0f;
	float downValue = Input->controllers[contrID].DownKey.IsDown == true ? 1.0f : 0.0f;

	float hValue = leftValue - rightValue;
	float vValue = upValue - downValue;

	v3 moveVec(-hValue, 0.0f, vValue);
	if (hValue != 0.0f || vValue != 0.0f){
		moveVec = glm::normalize(moveVec);
	}

	float cameraSpeed = Input->controllers[0].ShiftKey.IsDown == true ? 40.0f : 10.0f;
	cameraSpeed = Input->controllers[0].SpaceKey.IsDown == true ? cameraSpeed * 6.0f : cameraSpeed;
	State->Camera->position += State->Camera->front * moveVec.z * State->Time->deltaTime * cameraSpeed;
	State->Camera->position += State->Camera->right * moveVec.x * State->Time->deltaTime * cameraSpeed;

	float xoffset =  Input->controllers[contrID].DeltaMouseX;
	float yoffset = -Input->controllers[contrID].DeltaMouseY;


	if ((xoffset != 0.0f || yoffset != 0.0f) && Input->controllers[0].CapturingMouse == true){

		float mouseSens = 0.1f;
		xoffset *= mouseSens;
		yoffset *= mouseSens;

		State->Camera->pitch += yoffset;
		State->Camera->yaw += xoffset;

		if (State->Camera->pitch > 89.0f){
			State->Camera->pitch = 89.0f;
		}
		if (State->Camera->pitch < -89.0f){
			State->Camera->pitch = -89.0f;
		}
	}

	UpdateCameraVectors(State->Camera);

	GLint ProjectionLocation = glGetUniformLocation(State->MainShader.program, "projection");
	GLint ViewLocation = glGetUniformLocation(State->MainShader.program, "view");
	GLint ModelLocation = glGetUniformLocation(State->MainShader.program, "model");
	GLint ViewPosLocation = glGetUniformLocation(State->MainShader.program, "viewPos");

	glm::mat4 ProjectionMatrix = glm::perspective(45.0f, (float)ScreenBuffer->Width / (float)ScreenBuffer->Height, 0.03f, 1000.0f);
	glm::mat4 ViewMatrix = glm::lookAt(
		State->Camera->position,
		State->Camera->position + State->Camera->front,
		State->Camera->up);
	glUseProgram(State->MainShader.program);
	glUniformMatrix4fv(ProjectionLocation, 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));
	glUniformMatrix4fv(ViewLocation, 1, GL_FALSE, glm::value_ptr(ViewMatrix));
	glUniform3f(ViewPosLocation, State->Camera->position.x, State->Camera->position.y, State->Camera->position.z);
	glUseProgram(0);

	glUseProgram(State->CubemapShader.program);
	GLint CubemapProjectionLocation = glGetUniformLocation(State->CubemapShader.program, "projection");
	GLint CubemapViewLocation = glGetUniformLocation(State->CubemapShader.program, "view");
	glUniformMatrix4fv(CubemapProjectionLocation, 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));
	glUniformMatrix4fv(CubemapViewLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(glm::mat3(ViewMatrix))));
	glUseProgram(0);


	glBindFramebuffer(GL_FRAMEBUFFER, State->MainFramebuffer.FramebufferId);
	glViewport(0, 0, State->MainFramebuffer.Width, State->MainFramebuffer.Height);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//RenderSkybox(State->cubemapVAO, State->cubemapTexture, State->CubemapShader);
	for (int i = 0; i < State->Models.size(); i++){
		RenderModel(&State->Models[i], State->MainShader);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, ScreenBuffer->currentWidth, ScreenBuffer->currentHeight);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	RenderFramebuffer(State->MainFramebuffer, State->ScreenShader);

	char fpsStr[6];
	itoa(State->Time->fps, fpsStr, 10);
	char dtStr[8];
	itoa(State->Time->deltaTime * 1000, dtStr, 10);
	char thrStr[20];
	itoa(State->MeshThreads.size(), thrStr, 10);
	

	RenderText("FPS: " + std::string(fpsStr),
		v2(10, 10),
		State->Atlas, State->TextShader,
		State->TextVAO, State->TextVBO,
		ScreenBuffer->Width,
		ScreenBuffer->Height,
		0.4f);
	RenderText("DeltaTime: " + std::string(dtStr) + "ms",
		v2(10, 30),
		State->Atlas, State->TextShader,
		State->TextVAO, State->TextVBO,
		ScreenBuffer->Width,
		ScreenBuffer->Height,
		0.4f);
}