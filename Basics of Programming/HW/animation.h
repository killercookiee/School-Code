#ifndef ANIMATION_H
#define ANIMATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include <float.h>
#include <time.h>

typedef struct {
    int r, g, b;
    float transparency;
    float brightness;
} Pixel;
typedef struct {
    int N;
    Pixel ***pixels;
} Cube;
typedef enum {
    FRONT,
    LEFT,
    RIGHT,
    BACK,
    TOP,
    BOTTOM
} Face;
typedef struct {
    float x, y, z;
} Point;
typedef struct {
    Point start;
    Point end;
    float brightness;
} Line;
typedef struct {
    Line *lines;
    int line_count;
    int capacity;
} LineSpace;
typedef struct {
    Cube *space;
    LineSpace *lines;
    int N;
} ConstructedSpace;
typedef struct {
    int N;                  // Size of the 3D space
    float medium_transparency; // Medium's transparency
    float medium_brightness;  // Medium's brightness
} MediumParameters;
typedef struct {
    float line_brightness; // Brightness of the lines
} LineParameters;
typedef struct {
    int size;
} ObjectParameters;

Cube* create_empty_3d_space(int N);
Cube* create_donut_3d_space(int N, float medium_transparency, float medium_brightness);
Cube* create_sphere_3d_space(int N, float medium_transparency, float medium_brightness);
void free_constructed_space(ConstructedSpace *space);
LineSpace* create_line_space();
void add_line(LineSpace *lines, Point start, Point end, float brightness);
void update_3d_space_along_line(Cube *space, Point start, Point end, float brightness);
ConstructedSpace* construct_cube(MediumParameters medium_params, LineParameters line_params, int object_params[1]);
ConstructedSpace* construct_filled_cube(MediumParameters medium_params, LineParameters line_params, int object_params[1]);
ConstructedSpace* construct_sphere(MediumParameters medium_params, LineParameters line_params, int object_params[2]);
ConstructedSpace* construct_filled_sphere(MediumParameters medium_params, LineParameters line_params, int object_params[2]);
ConstructedSpace* construct_donut(MediumParameters medium_params, LineParameters line_params, int object_params[2]);
ConstructedSpace* construct_filled_donut(MediumParameters medium_params, LineParameters line_params, int object_params[2]);
ConstructedSpace* load_3d_space(const char *filename);
void save_3d_space(ConstructedSpace *constructed, const char *filename);
void free_cube(Cube *space);
void render_2d_face(Cube *space, Face face, char **screen);
void print_2d_screen(char **screen, int N);
void free_screen(char **screen, int N);
void free_line_space(LineSpace *lines);
void save_2d_screen(char **screen, const char *filename);
void rotate_point(Point *p, float theta_x, float theta_y, float theta_z);
ConstructedSpace* rotate_constructed_space(ConstructedSpace* input_space, int transformation_params[3]);
#endif // ANIMATION_H
