#include "2dcurves/global_vars.h"
#include "2dcurves/Vertex.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iterator>
#include <vector>

namespace curves{

    glm::vec2 get_cursor_position_NDC(GLFWwindow* window)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        // Map from screen coordinates to NDC
        float xposNDC = (xpos - (width/2.0))/(width/2.0);
        float yposNDC = ((height/2.0) - ypos)/(height/2.0);

        return glm::vec2(xposNDC, yposNDC);
    }

    std::vector<float> linspace(float a, float b, int n)
    {
        assert(n > 1);

        std::vector<float> res;
        float increment = (b - a) / (n - 1);

        for (int i = 0; i < n; ++i) {
            float value = a + i * increment;
            res.push_back(value);
        }

        return res;
    }

    long long binomial_coefficient(int n, int k)
    {
        assert((k >= 0) && (n >= 0) && (n >= k));
        assert(n <= 50);

        if (k > n - k) {
            k = n - k;
        }

        long long res = 1;
        for (int i = 0; i < k; ++i) {
            res *= n - i;
            res /= i + 1;
        }

        return res;
    }

    float bernstein_polynomial(int n, int i, float t)
    {
        assert((n >= 0) && (i >= 0) && (n >= i));

        return binomial_coefficient(n, i) * std::pow(t, i) * std::pow(1 - t, n - i);
    }

    void draw_bezier_curve(unsigned int vao, unsigned int vbo)
    {
        // Compute bezier points
        std::vector<glm::vec2> bezier_points;
        int bezier_degree = control_vertices.size() - 1;

        for (float t_value : t_samples) {
            glm::vec2 bezier_point(0.0, 0.0);
            for (int i = 0; i <= bezier_degree; i++) {
                bezier_point += bernstein_polynomial(bezier_degree, i, t_value) * control_vertices[i].position;
            }
            bezier_points.push_back(bezier_point);
        }

        assert(bezier_points.size() == num_samples);

        // Pass bezier_points to OpenGL
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(
            GL_ARRAY_BUFFER,
            bezier_points.size() * sizeof(glm::vec2),
            bezier_points.data(),
            GL_STATIC_DRAW
        );
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Draw
        glBindVertexArray(vao);
        glDrawArrays(GL_LINE_STRIP, 0, bezier_points.size());
        glBindVertexArray(0);
    }

    void draw_control_polygon(unsigned int vao, unsigned int vbo)
    {
        std::vector<glm::vec2> control_vertices_positions;
        std::transform(
            control_vertices.begin(),
            control_vertices.end(),
            std::back_inserter(control_vertices_positions),
            [](Vertex v) { return v.position; }
        );

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(
            GL_ARRAY_BUFFER,
            control_vertices_positions.size() * sizeof(glm::vec2),
            control_vertices_positions.data(),
            GL_STATIC_DRAW
        );
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS, 0, control_vertices_positions.size());
        glDrawArrays(GL_LINE_STRIP, 0, control_vertices_positions.size());
        glBindVertexArray(0);
    }
}