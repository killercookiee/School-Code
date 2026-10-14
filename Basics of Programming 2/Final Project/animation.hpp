#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#define _USE_MATH_DEFINES // Ensures M_PI is available on all platforms
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <cfloat>
#include <ctime>
#include <limits>
#include <dirent.h>
#include <stdexcept>

// ─── Data Structures ─────────────────────────────────────────────────────────

struct Pixel {
    int r = 0, g = 0, b = 0;
    float transparency = 1.0f;
    float brightness   = 0.0f;
};

struct Cube {
    int N;
    std::vector<std::vector<std::vector<Pixel>>> pixels;

    explicit Cube(int n) : N(n), pixels(n, std::vector<std::vector<Pixel>>(n, std::vector<Pixel>(n))) {}
};

enum Face { FRONT, LEFT, RIGHT, BACK, TOP, BOTTOM };

struct Point { float x, y, z; };

// ─── New Volumetric Physics Structures ───────────────────────────────────────

struct Vector3 {
    float dx, dy, dz;

    // Helper to normalize the vector to a length of 1
    void normalize() {
        float len = std::sqrt(dx * dx + dy * dy + dz * dz);
        if (len > 0) {
            dx /= len; dy /= len; dz /= len;
        }
    }
};

struct Ray {
    Point origin;
    Vector3 direction;
    float intensity; // The power of the light source
};

struct Projector {
    Point position;
    Vector3 base_target;
    std::vector<Ray> rays;
};

// ─────────────────────────────────────────────────────────────────────────────

struct Line {
    Point start, end;
    float brightness;
};

struct LineSpace {
    std::vector<Line> lines;

    void add(Point start, Point end, float brightness) {
        lines.push_back({start, end, brightness});
    }
    int size() const { return static_cast<int>(lines.size()); }
};

struct ConstructedSpace {
    Cube* space;
    LineSpace* lines;
    int        N;

    ConstructedSpace(Cube* s, LineSpace* l, int n) : space(s), lines(l), N(n) {}
    ~ConstructedSpace();  // defined in animation.cpp
};

struct MediumParameters {
    int   N;
    float medium_transparency;
    float medium_brightness;
};

struct LineParameters {
    float line_brightness;
};

// ─── Function Declarations ───────────────────────────────────────────────────

// Space creation & Light Transport
Cube* create_empty_3d_space(int N);
void              update_3d_space_along_line(Cube* space, Point start, Point end, float brightness);
void              shoot_ray(Cube* space, Ray ray, float medium_transparency);

// Object construction (Parametric)
ConstructedSpace* construct_cube(MediumParameters medium_params, LineParameters line_params, int object_params[1]);
ConstructedSpace* construct_filled_cube(MediumParameters medium_params, LineParameters line_params, int object_params[1]);
ConstructedSpace* construct_sphere(MediumParameters medium_params, LineParameters line_params, int object_params[2]);
ConstructedSpace* construct_filled_sphere(MediumParameters medium_params, LineParameters line_params, int object_params[2]);
ConstructedSpace* construct_donut(MediumParameters medium_params, LineParameters line_params, int object_params[2]);
ConstructedSpace* construct_filled_donut(MediumParameters medium_params, LineParameters line_params, int object_params[2]);

// Volumetric Light Test
ConstructedSpace* construct_test_ray(MediumParameters medium_params);
void apply_secondary_lighting(Cube* space, int max_depth);

// Save / Load
ConstructedSpace* load_3d_space(const std::string& filename);
void              save_3d_space(ConstructedSpace* constructed, const std::string& filename);

// Rendering
void render_2d_face(Cube* space, Face face, std::vector<std::string>& screen);
void print_2d_screen(const std::vector<std::string>& screen);
void save_2d_screen(const std::vector<std::string>& screen, const std::string& filename);

// Transformations
void              rotate_point(Point& p, float theta_x, float theta_y, float theta_z);
ConstructedSpace* rotate_constructed_space(ConstructedSpace* input_space, int transformation_params[3]);

#endif // ANIMATION_HPP