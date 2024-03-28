#pragma once

#include <glm/vec2.hpp>

struct Vertex
{
    glm::vec2 position;
    bool is_moving = false;
};