#include <GL/glew.h>

#include "platform_code.h"
#include "hardware_accelerated.h"

void UniformMaterial(game_shader shader, phong_material pmat, std::string unVarName){
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
void UniformPointLight(game_shader shader, point_light lit, std::string unVarName){
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
void UniformDirectionalLight(game_shader shader, directional_light lit, std::string unVarName){
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

#if ENGINE_DEBUG_MODE
game_memory* DebugGlobalMemory;
#endif

//extern "C" __declspec(dllexport) GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
#if ENGINE_DEBUG_STATE
	DebugGlobalMemory = Memory;
#endif

	//glViewport(0, 0, State->ScreenBuffer->Width, State->ScreenBuffer->Height);

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

	glm::mat4 ProjectionMatrix = glm::perspective(45.0f, (float)ScreenBuffer->Width / (float)ScreenBuffer->Height, 0.01f, 1000.0f);
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


	glBindFramebuffer(GL_FRAMEBUFFER, State->mainFramebuffer.FramebufferId);
	glViewport(0, 0, State->mainFramebuffer.Width, State->mainFramebuffer.Height);
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
	RenderFramebuffer(State->mainFramebuffer, State->ScreenShader);

	char fpsStr[6];
	itoa(State->Time->fps, fpsStr, 10);
	char dtStr[8];
	itoa(State->Time->deltaTime * 1000, dtStr, 10);
	char thrStr[20];
	itoa(State->MeshThreads.size(), thrStr, 10);
	

	RenderText("FPS: " + std::string(fpsStr),
		v2(10, 10),
		State->Atlas, State->TextShader,
		State->textVAO, State->textVBO,
		ScreenBuffer->Width,
		ScreenBuffer->Height,
		0.4f);
	RenderText("DeltaTime: " + std::string(dtStr) + "ms",
		v2(10, 30),
		State->Atlas, State->TextShader,
		State->textVAO, State->textVBO,
		ScreenBuffer->Width,
		ScreenBuffer->Height,
		0.4f);
}