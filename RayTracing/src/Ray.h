#pragma once

#include "glm/vec3.hpp"
class ray {

public:
    ray() {}
    ray(const glm::vec3& origin, const glm::vec3& direction)
        : orig(origin), dir(direction)
    {
        dir = glm::normalize(dir);
        invDir[0] = 1 / dir[0];
        invDir[1] = 1 / dir[1];
        invDir[2] = 1 / dir[2];
    }

    glm::vec3 origin() const { return orig; }
    glm::vec3 direction() const { return dir; }
    glm::vec3 invDirection() const { return invDir; }
    glm::vec3 at(float t) const {
        return orig + t * dir;
    }

public:
    glm::vec3 orig;
    glm::vec3 dir;
    glm::vec3 invDir;

};