#pragma once
#include <include/glm.h>
#include <Component/SimpleScene.h>
#include <vector>
#include <string>
#include "Animation.h"

class Enemy {
public:
	Enemy();

	~Enemy();

	void setPosition(float x, float y);
public:
	glm::vec2 position;
	std::vector<Animation*> animations;
	unsigned int sheet_width, sheet_height;
	std::unordered_map<std::string, int> animation_names;

	int walking_forward_id = 0;
	int walking_forward_frames_id = 0;
	int walking_backward_id = 0;
	int walking_backward_frames_id = 0;
	int hit_id = 0;
	int hit_frames_id = 0;
	int block_id = 0;
	int block_frames_id = 0;
	int falling_id = 0;
	int falling_frames_id = 0;

	int healthpoints = 100;
	int power = 10;
	int delay = 0;

	bool default_state = true, move_forward = false, move_backward = false, hit = false, blocking = false, falling = false;
	std::string orientation = "left", last_orientation = "left";
};