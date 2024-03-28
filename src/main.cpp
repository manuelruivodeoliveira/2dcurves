#define GLFW_INCLUDE_NONE

#include "2dcurves/global_vars.h"
#include "2dcurves/Shader.h"
#include "2dcurves/utils.h"
#include "2dcurves/Vertex.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <vector>


enum class mode {drawing, editing};
enum class visibility {show, hide};

// Initialize global variables
std::vector<Vertex> control_vertices;
mode active_mode = mode::drawing;
visibility active_visibility = visibility::show;
int num_samples = 200;
std::vector<float> t_samples = curves::linspace(0.0f, 1.0f, num_samples);


// Callbacks
static void error_callback(int error, const char* description)
{
    std::cerr << "Error: " << description << std::endl;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if ((key == GLFW_KEY_0 || key == GLFW_KEY_KP_0) && action == GLFW_PRESS) {
        if (active_mode == mode::editing) {
            glm::vec2 cursor_position_NDC = curves::get_cursor_position_NDC(window);
            Vertex new_vertex(cursor_position_NDC);
            control_vertices.push_back(new_vertex);
        
            active_mode = mode::drawing;
        }
    }

    if ((key == GLFW_KEY_1 || key == GLFW_KEY_KP_1) && action == GLFW_PRESS) {
        if (active_mode == mode::drawing) {
            control_vertices.pop_back();
            active_mode = mode::editing;
        }
    }

    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        control_vertices.clear();
        active_mode = mode::drawing;
    }

    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        active_visibility = visibility::show;
    }

    if (key == GLFW_KEY_H && action == GLFW_PRESS) {
        active_visibility = visibility::hide;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (active_mode == mode::drawing) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            glm::vec2 cursor_pos_NDC = curves::get_cursor_position_NDC(window);

            if (control_vertices.empty()) {
                Vertex first_vertex(cursor_pos_NDC);
                control_vertices.push_back(first_vertex);

                Vertex second_vertex(cursor_pos_NDC);
                control_vertices.push_back(second_vertex);
            }

            Vertex new_vertex(cursor_pos_NDC);
            if (control_vertices.size() <= 50) {
                control_vertices.push_back(new_vertex);
            } else {
                std::cerr << "Bézier curves with more than 51 vertices are not "
                        << "allowed to avoid overflow problems" << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
            control_vertices.pop_back();
            active_mode = mode::editing;
        }
    } else if (active_mode == mode::editing) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            glm::vec2 cursor_pos_NDC = curves::get_cursor_position_NDC(window);

            for (Vertex& v : control_vertices) {
                if (glm::distance(v.position, cursor_pos_NDC) < 0.03) {
                    v.is_moving = true;
                }
            }
        }

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            for (Vertex& v : control_vertices) {
                v.is_moving = false;
            }
        }
    }
}


int main()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        std::cerr << "glfw failed to initialize" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "2dcurves", NULL, NULL);
    if (!window) {
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
    if (version == 0) {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    // Successfully loaded OpenGL
    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "."
              << GLAD_VERSION_MINOR(version) << std::endl;

    // Keyboard input instructions
    std::cout << "\nKEYBOARD INPUT:\n" 
              << "0: drawing mode\n" 
              << "1: editing mode\n"
              << "C: clear\n"
              << "S: show control polyline\n"
              << "H: hide control polyline" << std::endl;

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


    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        glViewport(0, 0, width, height);
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::vec2 cursor_position_NDC = curves::get_cursor_position_NDC(window);

        if (active_mode == mode::drawing && !control_vertices.empty()) {
            Vertex& last_vertex = control_vertices.back();
            last_vertex.position = cursor_position_NDC;
        }

        int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (active_mode == mode::editing && state == GLFW_PRESS) {
            for (Vertex& v : control_vertices) {
                if (v.is_moving) {
                    v.position = cursor_position_NDC;
                }
            }
        }

        shaderProgram.use();
        glEnable(GL_PROGRAM_POINT_SIZE);
        
        curves::draw_bezier_curve(vaos[0], vbos[0]);

        if (active_visibility == visibility::show) {
            curves::draw_control_polygon(vaos[1], vbos[1]);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    
    glfwTerminate();
    return 0;
}