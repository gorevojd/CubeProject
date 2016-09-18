#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 tangent;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
    vec3 TempNormal;
}vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(){

	gl_Position = projection * view * model * vec4(position.xyz, 1.0);

	vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(T, N);

	mat3 normalMatrix = mat3(transpose(inverse(model)));
	vs_out.TempNormal = normalMatrix * normal;
	
	vs_out.TBN = transpose(mat3(T, B, N));
	vs_out.FragPos = vec3(model * vec4(position.xyz, 1.0));
	vs_out.TexCoords = vec2(texCoord.x, 1.0 - texCoord.y);
	
}