#ifndef LIGHTS_CODE_H

#include "../common_code.h"

struct point_light{
	v3 position;
	v3 diffuse;
	v3 specular;
	float ambient;

	float constant;
	float linear;
	float quadratic;

	point_light(){
		this->position = v3(0.0f);
		this->diffuse = v3(0.0f);
		this->specular = v3(0.0f);
		this->ambient = 0.05f;

		this->constant = 1.0f;
		this->linear = 0.0f;
		this->quadratic = 0.0f;
	}

	point_light(
		v3 position,
		v3 diffuse,
		v3 specular,
		float ambient,
		float radius)
	{
		this->position = position;
		this->diffuse = diffuse;
		this->ambient = ambient;
		this->specular = specular;

		this->constant = 1.0f;
		this->linear = 5 / radius;
		this->quadratic = 80.0f / radius / radius;
	}
};

struct directional_light{
	v3 direction;
	v3 diffuse;
	v3 specular;
	float ambient;

	directional_light(){}
	directional_light(
		v3 direction,
		v3 diffuse,
		v3 specular,
		float ambient)
	{
		this->direction = direction;
		this->diffuse = diffuse;
		this->specular = specular;
		this->ambient = ambient;
	}
};

inline void SetPosition(point_light* lit, v3 pos){
	lit->position = pos;
}

inline void SetSpecularColor(point_light* lit, v3 col){
	lit->specular = col;
}

inline void SetDiffuseColor(point_light* lit, v3 col){
	lit->diffuse = col;
}

inline void SetAttenuation(point_light* lit, float cons, float linear, float quadratic){
	lit->constant = cons;
	lit->linear = linear;
	lit->quadratic = quadratic;
}


#define LIGHTS_CODE_H
#endif