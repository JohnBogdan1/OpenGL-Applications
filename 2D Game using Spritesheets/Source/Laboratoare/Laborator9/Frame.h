#pragma once
#include <include/glm.h>
#include <string>
#include <vector>

class Frame {
public:
	Frame();

	~Frame();

public:
	std::string duration;
	std::vector<std::string> rectangle;
};