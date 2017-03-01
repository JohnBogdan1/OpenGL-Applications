#pragma once
#include <include/glm.h>
#include <string>
#include <vector>
#include "Frame.h"

class Animation {
public:
	Animation();

	~Animation();

public:
	std::string name;
	std::vector<Frame*> frames;
};