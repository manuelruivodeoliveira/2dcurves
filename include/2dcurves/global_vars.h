#pragma once

#include "2dcurves/Vertex.h"

#include <vector>

enum class mode;
enum class visibility;

// Global variables
extern std::vector<Vertex> control_vertices;
extern mode active_mode;
extern visibility active_visibility;
extern int num_samples;
extern std::vector<float> t_samples;