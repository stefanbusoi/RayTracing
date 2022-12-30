#pragma once

#include <glm/glm.hpp>

struct Ray
{
	glm::vec3 Origin;
	glm::vec3 Direction;
	Ray(glm::vec3 Origin, glm::vec3 Direction):Origin(Origin),Direction(Direction){};
	Ray()=default;
};