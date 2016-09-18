#ifndef CLASS_INPUT_H

#include "common_code.h"

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
	~game_input(){
		delete[] this->controllers;
	}
};

#define CLASS_INPUT_H
#endif