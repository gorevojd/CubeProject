#version 330 core

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
    vec3 TempNormal;
} fs_in;

struct DirLight{
	vec3 direction;
	vec3 diffuse;
	vec3 specular;
	vec3 ambient;
};

struct PointLight{
	vec3 position;
	
	float constant;
	float linear;
	float quadratic;

	vec3 diffuse;
	vec3 specular;
	vec3 ambient;	
};

struct PhongMaterial{
	bool withNormalMap;

	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	sampler2D emission;

	float shininess;
};

uniform PhongMaterial material;
uniform DirLight dirLight;
#define MAX_POINT_LIGHTS 4
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform vec3 viewPos;

vec3 CalcDirLight(DirLight lit, vec3 normal, vec3 viewDir){
	
	vec3 lightDir = normalize(lit.direction);
	vec3 reflectDir = normalize(reflect(lightDir, normal));
	vec3 halfWayDir = normalize(-viewDir + reflectDir);

	vec4 sampledColor = texture(material.diffuse, fs_in.TexCoords);
	vec4 sampledSpec = texture(material.specular, fs_in.TexCoords);

	float diff = max(dot(normal, -lightDir), 0.0);
	//float spec = pow(max(dot(normal, halfWayDir), 0.0), material.shininess * 4);
	float spec = pow(max(dot(-viewDir, reflectDir), 0.0), material.shininess);

	vec3 ambient =	lit.ambient * sampledColor.xyz;
	vec3 diffuse = lit.diffuse * diff * sampledColor.xyz;
	vec3 specular = lit.specular * spec * sampledSpec.xyz;

	return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight lit, vec3 fragpos, vec3 normal, vec3 viewDir){
	
	vec3 lightDir = normalize(fragpos - lit.position);
	vec3 reflectDir = reflect(lightDir, normal);
	vec3 halfWayDir = normalize(-viewDir + reflectDir);

	vec4 sampledColor = texture(material.diffuse, fs_in.TexCoords);
	vec4 sampledSpec = texture(material.specular, fs_in.TexCoords);

	float diff = max(dot(normal, -lightDir), 0.0);
	//float spec = pow(max(dot(normal, halfWayDir), 0.0), material.shininess * 4);
	float spec = pow(max(dot(-viewDir, reflectDir), 0.0), material.shininess);

	vec3 ambient =	lit.ambient * sampledColor.xyz;
	vec3 diffuse = lit.diffuse * diff * sampledColor.xyz;
	vec3 specular = lit.specular * spec * sampledSpec.xyz;

	float dist = length(lightDir);
	float attenuation = 1.0 / (lit.constant + lit.linear * dist + lit.quadratic * dist * dist);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return ambient + diffuse + specular;
}

void main(){
	vec3 res = vec3(0.0, 0.0, 0.0);
	
	vec3 normal;
	if(material.withNormalMap){
		normal = texture(material.normal, fs_in.TexCoords).rgb;
		normal = normalize(normal * 2.0 - 1.0);
		normal = normalize(transpose(fs_in.TBN) * normal);
	}
	else{
		//normal = fs_in.TBN * vec3(0.0, 0.0, 1.0);
		normal = fs_in.TBN[2];
	}
	normal = transpose(fs_in.TBN) * vec3(0.0, 0.0, 1.0);

	vec3 viewDir = normalize(fs_in.FragPos.xyz - viewPos);
	res += CalcDirLight(dirLight, normal, viewDir);
	for(int i = 0; i < MAX_POINT_LIGHTS; i++){
		//res+=CalcPointLight(pointLights[i], fs_in.FragPos, normal, viewDir);
	}

	gl_FragColor = vec4(res, 1.0);
}