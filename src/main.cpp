#define GLFW_INCLUDE_NONE

#include "2dcurves/Shader.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>


// Global variables
std::vector<glm::vec2> control_vertices;
int num_samples = 100;
std::vector<float> t_samples;


// Helper functions
void fill_t_samples()
{
    assert(num_samples > 1);

    float t_delta = 1.0/(num_samples - 1);
    for(int i = 0; i < num_samples; ++i) {
        float t_value = i * t_delta;
        t_samples.push_back(t_value);
    }
}

long long binomial_coefficient(int n, int k)
{
    assert((k >= 0) && (n >= 0) && (n >= k));
    assert(n <= 50);

    if(k > n - k) {
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
            bezier_point += bernstein_polynomial(bezier_degree, i, t_value) * control_vertices[i];
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
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        control_vertices.size() * sizeof(glm::vec2),
        control_vertices.data(),
        GL_STATIC_DRAW
    );
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, control_vertices.size());
    glDrawArrays(GL_LINE_STRIP, 0, control_vertices.size());
    glBindVertexArray(0);
}


// Callbacks
static void error_callback(int error, const char* description)
{
    std::cerr << "Error: " << description << std::endl;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        
        // Map from screen coordinates to NDC
        float xposNDC = (xpos - (width/2.0))/(width/2.0);
        float yposNDC = ((height/2.0) - ypos)/(height/2.0);

        glm::vec2 new_vertex(xposNDC, yposNDC);
        if(control_vertices.size() <= 50) {
            control_vertices.push_back(new_vertex);
        } else {
            std::cerr << "Bézier curves with more than 51 vertices are not "
                      << "allowed to avoid overflow problems" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}


int main()
{
    fill_t_samples();

    glfwSetErrorCallback(error_callback);

    if(!glfwInit()) {
        std::cerr << "glfw failed to initialize" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
    if(!window) {
        std::cerr << "glfw failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Set callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwMakeContextCurrent(window);

    // Initialize glad
    int version = gladLoadGL(glfwGetProcAddress);
    if(version == 0) {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    // Successfully loaded OpenGL
    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "."
              << GLAD_VERSION_MINOR(version) << std::endl;

    glfwSwapInterval(1);


    // Build shader program
    const char* vertexPath = "./shaders/vertex_shader.txt";
    const char* fragmentPath = "./shaders/fragment_shader.txt";
    Shader shaderProgram(vertexPath, fragmentPath);

    // VAOs and VBOs
    unsigned int vaos[2];
    unsigned int vbos[2];

    glGenVertexArrays(2, vaos);
    glGenBuffers(2, vbos);

    // Bezier curve
    glBindVertexArray(vaos[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Control vertices
    glBindVertexArray(vaos[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    while(!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        glViewport(0, 0, width, height);
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shaderProgram.use();
        glEnable(GL_PROGRAM_POINT_SIZE);
        draw_bezier_curve(vaos[0], vbos[0]);
        draw_control_polygon(vaos[1], vbos[1]);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    
    glfwTerminate();
    return 0;
}