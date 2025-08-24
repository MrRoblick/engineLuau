#pragma once

#include <Instance/BasePart.h>
#include <glm/glm.hpp>

class Part: public BasePart {
public:
	Part() { name = "Part"; };
	~Part() = default;
};