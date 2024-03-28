#pragma once

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include <vector>

namespace curves{

    glm::vec2 get_cursor_position_NDC(GLFWwindow* window);

    std::vector<float> linspace(float a, float b, int n);

    long long binomial_coefficient(int n, int k);

    float bernstein_polynomial(int n, int i, float t);

    void draw_bezier_curve(unsigned int vao, unsigned int vbo);

    void draw_control_polygon(unsigned int vao, unsigned int vbo);
}