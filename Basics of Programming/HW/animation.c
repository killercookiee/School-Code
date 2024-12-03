#include "animation.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>


// Create an empty 3D space with the given size
Cube *create_empty_3d_space(int N) {
    Cube *space = malloc(sizeof(Cube));
    if (!space) {
        fprintf(stderr, "Failed to allocate memory for Cube.\n");
        return NULL;
    }
    space->N = N;

    space->pixels = malloc(N * sizeof(Pixel **));
    if (!space->pixels) {
        fprintf(stderr, "Failed to allocate memory for Cube pixels.\n");
        free(space);
        return NULL;
    }

    for (int i = 0; i < N; i++) {
        space->pixels[i] = malloc(N * sizeof(Pixel *));
        if (!space->pixels[i]) {
            for (int j = 0; j < i; j++) free(space->pixels[j]);
            free(space->pixels);
            free(space);
            fprintf(stderr, "Failed to allocate memory for Cube pixels row %d.\n", i);
            return NULL;
        }

        for (int j = 0; j < N; j++) {
            space->pixels[i][j] = malloc(N * sizeof(Pixel));
            if (!space->pixels[i][j]) {
                for (int k = 0; k < j; k++) free(space->pixels[i][k]);
                free(space->pixels[i]);
                for (int k = 0; k < i; k++) free(space->pixels[k]);
                free(space->pixels);
                free(space);
                fprintf(stderr, "Failed to allocate memory for Cube pixels cell %d, %d.\n", i, j);
                return NULL;
            }
        }
    }
    return space;
}

// Create a new LineSpace structure
LineSpace *create_line_space() {
    LineSpace *lines = malloc(sizeof(LineSpace));
    if (!lines) {
        fprintf(stderr, "Failed to allocate memory for LineSpace.\n");
        return NULL;
    }
    lines->line_count = 0;
    lines->capacity = 10; // Initial capacity
    lines->lines = malloc(lines->capacity * sizeof(Line));
    if (!lines->lines) {
        fprintf(stderr, "Failed to allocate memory for lines array.\n");
        free(lines);
        return NULL;
    }
    return lines;
}

void add_line(LineSpace *lines, Point start, Point end, float brightness) {
    if (!lines) return;

    if (lines->line_count >= lines->capacity) {
        int new_capacity = lines->capacity * 2;
        Line *new_lines = realloc(lines->lines, new_capacity * sizeof(Line));
        if (!new_lines) {
            fprintf(stderr, "Failed to reallocate memory for lines array.\n");
            return;
        }
        lines->lines = new_lines;
        lines->capacity = new_capacity;
    }

    lines->lines[lines->line_count].start = start;
    lines->lines[lines->line_count].end = end;
    lines->lines[lines->line_count].brightness = brightness;
    lines->line_count++;
}

void update_3d_space_along_line(Cube *space, Point start, Point end, float brightness) {
    int x0 = (int)roundf(start.x);
    int y0 = (int)roundf(start.y);
    int z0 = (int)roundf(start.z);
    int x1 = (int)roundf(end.x);
    int y1 = (int)roundf(end.y);
    int z1 = (int)roundf(end.z);

    int dx = abs(x1 - x0), dy = abs(y1 - y0), dz = abs(z1 - z0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int sz = (z0 < z1) ? 1 : -1;

    int err1, err2;
    if (dx >= dy && dx >= dz) {  // x-dominant line
        err1 = 2 * dy - dx;
        err2 = 2 * dz - dx;
        for (; x0 != x1; x0 += sx) {
            if (x0 >= 0 && x0 < space->N && y0 >= 0 && y0 < space->N && z0 >= 0 && z0 < space->N) {
                space->pixels[x0][y0][z0].brightness += brightness;  // Adjust brightness
            }
            if (err1 > 0) { y0 += sy; err1 -= 2 * dx; }
            if (err2 > 0) { z0 += sz; err2 -= 2 * dx; }
            err1 += 2 * dy;
            err2 += 2 * dz;
        }
    } else if (dy >= dx && dy >= dz) {  // y-dominant line
        err1 = 2 * dx - dy;
        err2 = 2 * dz - dy;
        for (; y0 != y1; y0 += sy) {
            if (x0 >= 0 && x0 < space->N && y0 >= 0 && y0 < space->N && z0 >= 0 && z0 < space->N) {
                space->pixels[x0][y0][z0].brightness += brightness;  // Adjust brightness
            }
            if (err1 > 0) { x0 += sx; err1 -= 2 * dy; }
            if (err2 > 0) { z0 += sz; err2 -= 2 * dy; }
            err1 += 2 * dx;
            err2 += 2 * dz;
        }
    } else {  // z-dominant line
        err1 = 2 * dy - dz;
        err2 = 2 * dx - dz;
        for (; z0 != z1; z0 += sz) {
            if (x0 >= 0 && x0 < space->N && y0 >= 0 && y0 < space->N && z0 >= 0 && z0 < space->N) {
                space->pixels[x0][y0][z0].brightness += brightness;  // Adjust brightness
            }
            if (err1 > 0) { y0 += sy; err1 -= 2 * dz; }
            if (err2 > 0) { x0 += sx; err2 -= 2 * dz; }
            err1 += 2 * dy;
            err2 += 2 * dx;
        }
    }
}





// Sample bject construction
ConstructedSpace* construct_cube(MediumParameters medium_params, LineParameters line_params, int object_params[1]) {
    // Create the 3D space for the cube (background)
    int cube_size = object_params[0];
    Cube *space = create_empty_3d_space(medium_params.N); // Use medium_params.N for 3D space size

    if (!space) {
        fprintf(stderr, "Failed to create 3D space.\n");
        return NULL;
    }

    // Initialize the medium with given transparency and brightness
    for (int i = 0; i < medium_params.N; i++) {
        for (int j = 0; j < medium_params.N; j++) {
            for (int k = 0; k < medium_params.N; k++) {
                space->pixels[i][j][k].transparency = medium_params.medium_transparency;
                space->pixels[i][j][k].brightness = medium_params.medium_brightness;
                space->pixels[i][j][k].r = 0;
                space->pixels[i][j][k].g = 0;
                space->pixels[i][j][k].b = 0;
            }
        }
    }

    // Create the LineSpace structure
    LineSpace *lines = create_line_space();
    if (!lines) {
        free(space); // Clean up in case of failure
        fprintf(stderr, "Failed to create LineSpace.\n");
        return NULL;
    }

    // Calculate the center offset for cube placement
    int center_offset = (medium_params.N - cube_size) / 2;

    // Generate lines for the cube's edges
    for (int i = 0; i < cube_size; i++) {
        for (int j = 0; j < cube_size; j++) {
            for (int k = 0; k < cube_size; k++) {
                if (i == 0 || i == cube_size - 1 || j == 0 || j == cube_size - 1 || k == 0 || k == cube_size - 1) {
                    Point p1 = {i + center_offset, j + center_offset, k + center_offset};

                    if (i < cube_size - 1) {
                        Point p2 = {i + 1 + center_offset, j + center_offset, k + center_offset};
                        add_line(lines, p1, p2, line_params.line_brightness);
                        update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
                    }
                    if (j < cube_size - 1) {
                        Point p2 = {i + center_offset, j + 1 + center_offset, k + center_offset};
                        add_line(lines, p1, p2, line_params.line_brightness);
                        update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
                    }
                    if (k < cube_size - 1) {
                        Point p2 = {i + center_offset, j + center_offset, k + 1 + center_offset};
                        add_line(lines, p1, p2, line_params.line_brightness);
                        update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
                    }
                }
            }
        }
    }

    // Create and return the ConstructedSpace
    ConstructedSpace *constructed = malloc(sizeof(ConstructedSpace));
    if (!constructed) {
        ConstructedSpace temp_constructed = {
            .space = space,
            .lines = lines,
            .N = medium_params.N
        };
        free_constructed_space(&temp_constructed);
        fprintf(stderr, "Failed to allocate memory for ConstructedSpace.\n");
        return NULL;
    }

    constructed->space = space;
    constructed->lines = lines;
    constructed->N = medium_params.N;
    return constructed;
}

ConstructedSpace* construct_filled_cube(MediumParameters medium_params, LineParameters line_params, int object_params[1]) {
    // Create the 3D space for the cube (background)
    int cube_size = object_params[0];
    Cube *space = create_empty_3d_space(medium_params.N); // Use medium_params.N for 3D space size

    if (!space) {
        fprintf(stderr, "Failed to create 3D space.\n");
        return NULL;
    }

    // Initialize the medium with given transparency and brightness
    for (int i = 0; i < medium_params.N; i++) {
        for (int j = 0; j < medium_params.N; j++) {
            for (int k = 0; k < medium_params.N; k++) {
                space->pixels[i][j][k].transparency = medium_params.medium_transparency;
                space->pixels[i][j][k].brightness = medium_params.medium_brightness;
                space->pixels[i][j][k].r = 0;
                space->pixels[i][j][k].g = 0;
                space->pixels[i][j][k].b = 0;
            }
        }
    }

    // Create the LineSpace structure
    LineSpace *lines = create_line_space();
    if (!lines) {
        free(space); // Clean up in case of failure
        fprintf(stderr, "Failed to create LineSpace.\n");
        return NULL;
    }

    // Calculate the center offset for cube placement
    int center_offset = (medium_params.N - cube_size) / 2;

    // Fill the interior of the cube
    for (int i = 0; i < cube_size; i++) {
        for (int j = 0; j < cube_size; j++) {
            for (int k = 0; k < cube_size; k++) {
                // Calculate the absolute voxel position
                int x = i + center_offset;
                int y = j + center_offset;
                int z = k + center_offset;

                // Fill the voxel with brightness and transparency
                space->pixels[x][y][z].brightness = line_params.line_brightness;
                space->pixels[x][y][z].transparency = medium_params.medium_transparency;

                // Optional: Set color for visualization
                space->pixels[x][y][z].r = 255; // Example: red color
                space->pixels[x][y][z].g = 0;
                space->pixels[x][y][z].b = 0;
            }
        }
    }

    // Generate lines for the cube's edges (optional for visualization)
    for (int i = 0; i < cube_size; i++) {
        for (int j = 0; j < cube_size; j++) {
            for (int k = 0; k < cube_size; k++) {
                if (i == 0 || i == cube_size - 1 || j == 0 || j == cube_size - 1 || k == 0 || k == cube_size - 1) {
                    Point p1 = {i + center_offset, j + center_offset, k + center_offset};

                    if (i < cube_size - 1) {
                        Point p2 = {i + 1 + center_offset, j + center_offset, k + center_offset};
                        add_line(lines, p1, p2, line_params.line_brightness);
                        update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
                    }
                    if (j < cube_size - 1) {
                        Point p2 = {i + center_offset, j + 1 + center_offset, k + center_offset};
                        add_line(lines, p1, p2, line_params.line_brightness);
                        update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
                    }
                    if (k < cube_size - 1) {
                        Point p2 = {i + center_offset, j + center_offset, k + 1 + center_offset};
                        add_line(lines, p1, p2, line_params.line_brightness);
                        update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
                    }
                }
            }
        }
    }

    // Create and return the ConstructedSpace
    ConstructedSpace *constructed = malloc(sizeof(ConstructedSpace));
    if (!constructed) {
        ConstructedSpace temp_constructed = {
            .space = space,
            .lines = lines,
            .N = medium_params.N
        };
        free_constructed_space(&temp_constructed);
        fprintf(stderr, "Failed to allocate memory for ConstructedSpace.\n");
        return NULL;
    }

    constructed->space = space;
    constructed->lines = lines;
    constructed->N = medium_params.N;
    return constructed;
}

ConstructedSpace* construct_sphere(MediumParameters medium_params, LineParameters line_params, int object_params[2]) {
    int N = medium_params.N;
    int segments = 10*N; // Number of segments for resolution
    int radius = (int)roundf(object_params[0] / 2.0f);  // Radius of the sphere



    Cube *space = create_empty_3d_space(N);
    LineSpace *lines = create_line_space();

    // Initialize the medium with the given transparency and brightness
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                space->pixels[i][j][k].transparency = medium_params.medium_transparency;
                space->pixels[i][j][k].brightness = medium_params.medium_brightness;
                space->pixels[i][j][k].r = 0;
                space->pixels[i][j][k].g = 0;
                space->pixels[i][j][k].b = 0;
            }
        }
    }

    // Calculate the center offset
    int center_offset = N / 2; // This shifts the sphere center to the middle of the space

    // Generate sphere points using spherical coordinates
    for (int i = 0; i <= segments; i++) {
        float theta = M_PI * i / segments;  // Latitude angle
        for (int j = 0; j < 2 * segments; j++) {
            float phi = 2 * M_PI * j / (2 * segments);  // Longitude angle

            // Cartesian coordinates of the point
            float x = radius * sin(theta) * cos(phi);
            float y = radius * sin(theta) * sin(phi);
            float z = radius * cos(theta);

            // Shift the coordinates to the center (N/2, N/2, N/2)
            Point p1 = {center_offset + x, center_offset + y, center_offset + z};

            // Add lines to adjacent points for visualization
            if (j > 0) {
                float prev_phi = 2 * M_PI * (j - 1) / (2 * segments);
                float px = radius * sin(theta) * cos(prev_phi);
                float py = radius * sin(theta) * sin(prev_phi);
                float pz = radius * cos(theta);

                // Shift the previous point coordinates to the center
                Point p2 = {center_offset + px, center_offset + py, center_offset + pz};
                add_line(lines, p1, p2, line_params.line_brightness);
                update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
            }
        }
    }

    // Create and return the constructed space
    ConstructedSpace *constructed = malloc(sizeof(ConstructedSpace));
    constructed->space = space;
    constructed->lines = lines;
    constructed->N = N;
    return constructed;
}

ConstructedSpace* construct_filled_sphere(MediumParameters medium_params, LineParameters line_params, int object_params[2]) {
    int N = medium_params.N;
    int segments = 10 * N; // Number of segments for resolution
    int radius = (int)roundf(object_params[0] / 2.0f);  // Radius of the sphere

    Cube *space = create_empty_3d_space(N);
    LineSpace *lines = create_line_space();

    // Initialize the medium with the given transparency and brightness
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                space->pixels[i][j][k].transparency = medium_params.medium_transparency;
                space->pixels[i][j][k].brightness = medium_params.medium_brightness;
                space->pixels[i][j][k].r = 0;
                space->pixels[i][j][k].g = 0;
                space->pixels[i][j][k].b = 0;
            }
        }
    }

    // Calculate the center offset
    int center_offset = N / 2; // This shifts the sphere center to the middle of the space

    // Fill the interior of the sphere
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            for (int z = 0; z < N; z++) {
                // Calculate distance from the center
                float dx = x - center_offset;
                float dy = y - center_offset;
                float dz = z - center_offset;

                // Check if the voxel lies within the sphere
                if (dx * dx + dy * dy + dz * dz <= radius * radius) {
                    // Fill the voxel
                    space->pixels[x][y][z].brightness = line_params.line_brightness;
                    space->pixels[x][y][z].transparency = medium_params.medium_transparency;
                    space->pixels[x][y][z].r = 255; // Example color (red)
                    space->pixels[x][y][z].g = 0;
                    space->pixels[x][y][z].b = 0;
                }
            }
        }
    }

    // Generate surface lines using spherical coordinates
    for (int i = 0; i <= segments; i++) {
        float theta = M_PI * i / segments;  // Latitude angle
        for (int j = 0; j < 2 * segments; j++) {
            float phi = 2 * M_PI * j / (2 * segments);  // Longitude angle

            // Cartesian coordinates of the point
            float x = radius * sin(theta) * cos(phi);
            float y = radius * sin(theta) * sin(phi);
            float z = radius * cos(theta);

            // Shift the coordinates to the center (N/2, N/2, N/2)
            Point p1 = {center_offset + x, center_offset + y, center_offset + z};

            // Add lines to adjacent points for visualization
            if (j > 0) {
                float prev_phi = 2 * M_PI * (j - 1) / (2 * segments);
                float px = radius * sin(theta) * cos(prev_phi);
                float py = radius * sin(theta) * sin(prev_phi);
                float pz = radius * cos(theta);

                // Shift the previous point coordinates to the center
                Point p2 = {center_offset + px, center_offset + py, center_offset + pz};
                add_line(lines, p1, p2, line_params.line_brightness);
                update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
            }
        }
    }

    // Create and return the constructed space
    ConstructedSpace *constructed = malloc(sizeof(ConstructedSpace));
    constructed->space = space;
    constructed->lines = lines;
    constructed->N = N;
    return constructed;
}

ConstructedSpace* construct_donut(MediumParameters medium_params, LineParameters line_params, int object_params[2]) {
    int N = medium_params.N;
    int segments = 10 * N; // Resolution for the torus
    int major_radius = object_params[0]; // Major radius of the torus
    int minor_radius = object_params[1]; // Minor radius of the torus

    Cube *space = create_empty_3d_space(N);
    LineSpace *lines = create_line_space();

    // Initialize the medium with given transparency and brightness
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                space->pixels[i][j][k].transparency = medium_params.medium_transparency;
                space->pixels[i][j][k].brightness = medium_params.medium_brightness;
                space->pixels[i][j][k].r = 0;
                space->pixels[i][j][k].g = 0;
                space->pixels[i][j][k].b = 0;
            }
        }
    }

    // Calculate the center offset
    int center_offset = N / 2; // Center the torus in the middle of the space

    // Generate torus points using parametric equations
    for (int i = 0; i <= segments; i++) {
        float theta = 2 * M_PI * i / segments; // Major circle angle
        for (int j = 0; j <= segments; j++) {
            float phi = 2 * M_PI * j / segments; // Minor circle angle

            // Toroidal coordinates to Cartesian coordinates
            float x = (major_radius + minor_radius * cos(phi)) * cos(theta);
            float y = (major_radius + minor_radius * cos(phi)) * sin(theta);
            float z = minor_radius * sin(phi);

            // Shift the coordinates to center the torus
            Point p1 = {center_offset + x, center_offset + y, center_offset + z};

            // Add lines to adjacent points for visualization
            if (j > 0) {
                float prev_phi = 2 * M_PI * (j - 1) / segments;
                float px = (major_radius + minor_radius * cos(prev_phi)) * cos(theta);
                float py = (major_radius + minor_radius * cos(prev_phi)) * sin(theta);
                float pz = minor_radius * sin(prev_phi);

                // Shift the previous point coordinates to the center
                Point p2 = {center_offset + px, center_offset + py, center_offset + pz};
                add_line(lines, p1, p2, line_params.line_brightness);
                update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
            }

            if (i > 0) {
                float prev_theta = 2 * M_PI * (i - 1) / segments;
                float px = (major_radius + minor_radius * cos(phi)) * cos(prev_theta);
                float py = (major_radius + minor_radius * cos(phi)) * sin(prev_theta);
                float pz = minor_radius * sin(phi);

                // Shift the previous point coordinates to the center
                Point p2 = {center_offset + px, center_offset + py, center_offset + pz};
                add_line(lines, p1, p2, line_params.line_brightness);
                update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
            }
        }
    }

    // Create and return the constructed space
    ConstructedSpace *constructed = malloc(sizeof(ConstructedSpace));
    constructed->space = space;
    constructed->lines = lines;
    constructed->N = N;
    return constructed;
}

ConstructedSpace* construct_filled_donut(MediumParameters medium_params, LineParameters line_params, int object_params[2]) {
    int N = medium_params.N;
    int segments = 10 * N; // Resolution for the torus
    int major_radius = object_params[0]; // Major radius of the torus
    int minor_radius = object_params[1]; // Minor radius of the torus

    Cube *space = create_empty_3d_space(N);
    LineSpace *lines = create_line_space();

    // Initialize the medium with given transparency and brightness
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                space->pixels[i][j][k].transparency = medium_params.medium_transparency;
                space->pixels[i][j][k].brightness = medium_params.medium_brightness;
                space->pixels[i][j][k].r = 0;
                space->pixels[i][j][k].g = 0;
                space->pixels[i][j][k].b = 0;
            }
        }
    }

    // Calculate the center offset
    int center_offset = N / 2; // Center the torus in the middle of the space

    // Generate torus points and fill its interior
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            for (int z = 0; z < N; z++) {
                // Shift voxel coordinates to be centered
                float dx = x - center_offset;
                float dy = y - center_offset;
                float dz = z - center_offset;

                // Calculate distance to the torus' surface
                float distance_to_major_circle = sqrt(dx * dx + dy * dy) - major_radius;
                float distance_to_torus_surface = sqrt(distance_to_major_circle * distance_to_major_circle + dz * dz);

                // Check if the voxel lies inside the torus
                if (distance_to_torus_surface <= minor_radius) {
                    // Fill the voxel
                    space->pixels[x][y][z].brightness = line_params.line_brightness;
                    space->pixels[x][y][z].transparency = medium_params.medium_transparency;
                    space->pixels[x][y][z].r = 255; // Example color
                    space->pixels[x][y][z].g = 255;
                    space->pixels[x][y][z].b = 0;
                }
            }
        }
    }

    // Generate outer lines using the parametric equations
    for (int i = 0; i <= segments; i++) {
        float theta = 2 * M_PI * i / segments; // Major circle angle
        for (int j = 0; j <= segments; j++) {
            float phi = 2 * M_PI * j / segments; // Minor circle angle

            // Toroidal coordinates to Cartesian coordinates
            float x = (major_radius + minor_radius * cos(phi)) * cos(theta);
            float y = (major_radius + minor_radius * cos(phi)) * sin(theta);
            float z = minor_radius * sin(phi);

            // Shift the coordinates to center the torus
            Point p1 = {center_offset + x, center_offset + y, center_offset + z};

            // Add lines to adjacent points for visualization
            if (j > 0) {
                float prev_phi = 2 * M_PI * (j - 1) / segments;
                float px = (major_radius + minor_radius * cos(prev_phi)) * cos(theta);
                float py = (major_radius + minor_radius * cos(prev_phi)) * sin(theta);
                float pz = minor_radius * sin(prev_phi);

                // Shift the previous point coordinates to the center
                Point p2 = {center_offset + px, center_offset + py, center_offset + pz};
                add_line(lines, p1, p2, line_params.line_brightness);
                update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
            }

            if (i > 0) {
                float prev_theta = 2 * M_PI * (i - 1) / segments;
                float px = (major_radius + minor_radius * cos(phi)) * cos(prev_theta);
                float py = (major_radius + minor_radius * cos(phi)) * sin(prev_theta);
                float pz = minor_radius * sin(phi);

                // Shift the previous point coordinates to the center
                Point p2 = {center_offset + px, center_offset + py, center_offset + pz};
                add_line(lines, p1, p2, line_params.line_brightness);
                update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
            }
        }
    }

    // Create and return the constructed space
    ConstructedSpace *constructed = malloc(sizeof(ConstructedSpace));
    constructed->space = space;
    constructed->lines = lines;
    constructed->N = N;
    return constructed;
}




// Free the memory allocated for the ConstructedSpace
void free_constructed_space(ConstructedSpace *constructed) {
    if (!constructed) return;

    if (constructed->space) {
        for (int i = 0; i < constructed->N; i++) {
            for (int j = 0; j < constructed->N; j++) {
                free(constructed->space->pixels[i][j]);
            }
            free(constructed->space->pixels[i]);
        }
        free(constructed->space->pixels);
        free(constructed->space);
    }

    if (constructed->lines) {
        free(constructed->lines->lines);
        free(constructed->lines);
    }

    free(constructed);
}




// 3D
ConstructedSpace *load_3d_space(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file for reading");
        return NULL;
    }

    int N;
    if (fscanf(file, "CUBE %d\n", &N) != 1) {
        fprintf(stderr, "Invalid file format: Missing CUBE header\n");
        fclose(file);
        return NULL;
    }

    ConstructedSpace *constructed = malloc(sizeof(ConstructedSpace));
    if (!constructed) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }
    constructed->N = N;
    constructed->space = create_empty_3d_space(N);
    if (!constructed->space) {
        free(constructed);
        fclose(file);
        return NULL;
    }

    constructed->lines = create_line_space();
    if (!constructed->lines) {
        free_constructed_space(constructed);
        fclose(file);
        return NULL;
    }

    Cube *space = constructed->space;
    while (1) {
        int i, j, k, r, g, b;
        float transparency, brightness;
        if (fscanf(file, "%d %d %d %f %f %d %d %d\n",
                   &i, &j, &k, &transparency, &brightness, &r, &g, &b) != 8) {
            break;  // End of the CUBE section
        }

        space->pixels[i][j][k].transparency = transparency;
        space->pixels[i][j][k].brightness = brightness;
        space->pixels[i][j][k].r = r;
        space->pixels[i][j][k].g = g;
        space->pixels[i][j][k].b = b;
    }

    int line_count;
    if (fscanf(file, "LINES %d\n", &line_count) != 1) {
        fprintf(stderr, "Invalid file format: Missing LINES header\n");
        free_constructed_space(constructed);
        fclose(file);
        return NULL;
    }

    for (int i = 0; i < line_count; i++) {
        Line line;
        if (fscanf(file, "%f %f %f %f %f %f %f\n",
                   &line.start.x, &line.start.y, &line.start.z,
                   &line.end.x, &line.end.y, &line.end.z,
                   &line.brightness) != 7) {
            fprintf(stderr, "Invalid file format: Line data mismatch\n");
            free_constructed_space(constructed);
            fclose(file);
            return NULL;
        }
        add_line(constructed->lines, line.start, line.end, line.brightness);
    }

    fclose(file);
    return constructed;
}

void save_3d_space(ConstructedSpace *constructed, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    Cube *space = constructed->space;
    LineSpace *lines = constructed->lines;
    printf("Lines: %p\n", lines->lines);

    // Save the 3D space properties
    fprintf(file, "CUBE %d\n", space->N);
    for (int i = 0; i < space->N; i++) {
        for (int j = 0; j < space->N; j++) {
            for (int k = 0; k < space->N; k++) {
                Pixel p = space->pixels[i][j][k];
                fprintf(file, "%d %d %d %.2f %.2f %d %d %d\n", 
                        i, j, k, p.transparency, p.brightness, p.r, p.g, p.b);
            }
        }
    }

    printf("3D space saved to %s\n", filename);

    // Save the lines
    fprintf(file, "LINES %d\n", lines->line_count);
    for (int i = 0; i < lines->line_count; i++) {
        Line line = lines->lines[i];
        fprintf(file, "%.2f %.2f %.2f %.2f %.2f %.2f %.2f\n", 
                line.start.x, line.start.y, line.start.z,
                line.end.x, line.end.y, line.end.z,
                line.brightness);
    }

    fclose(file);
}

void free_cube(Cube *space) {
    if (!space) return;
    for (int i = 0; i < space->N; i++) {
        for (int j = 0; j < space->N; j++) {
            free(space->pixels[i][j]);
        }
        free(space->pixels[i]);
    }
    free(space->pixels);
    free(space);
}


// 2D
void render_2d_face(Cube *space, Face face, char **screen) {
    int N = space->N;

    // Clear the screen with spaces
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            screen[i][j] = ' ';
        }
    }

    // Allocate and initialize the brightness map
    float **brightness_map = malloc(N * sizeof(float *));
    for (int i = 0; i < N; i++) {
        brightness_map[i] = calloc(N, sizeof(float));  // Zero-initialized
    }

    float min_brightness = FLT_MAX;
    float max_brightness = -FLT_MAX;

    // Traverse the cube depth along the selected face
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            float total_brightness = 0.0;
            float opacity_accumulation = 1.0;

            for (int k = 0; k < N; k++) {
                Pixel p;
                float distance;

                // Determine the pixel and distance based on the face
                switch (face) {
                    case FRONT:  p = space->pixels[i][j][k]; distance = k + 1; break;
                    case LEFT:   p = space->pixels[k][i][j]; distance = k + 1; break;
                    case RIGHT:  p = space->pixels[N - 1 - k][i][j]; distance = k + 1; break;
                    case BACK:   p = space->pixels[i][j][N - 1 - k]; distance = k + 1; break;
                    case TOP:    p = space->pixels[i][k][j]; distance = k + 1; break;
                    case BOTTOM: p = space->pixels[i][N - 1 - k][j]; distance = k + 1; break;
                    default: continue;
                }

                float inverse_square = 1.0f / (distance * distance);
                float effective_brightness = p.brightness * inverse_square * opacity_accumulation;

                total_brightness += effective_brightness;
                opacity_accumulation *= p.transparency;

                if (opacity_accumulation <= 0.01f) break;
            }

            // Track brightness for normalization
            if (total_brightness < min_brightness) min_brightness = total_brightness;
            if (total_brightness > max_brightness) max_brightness = total_brightness;

            brightness_map[i][j] = total_brightness;
        }
    }

    // Normalize brightness and map to ASCII
    const char *ascii_map = " .:-=+*%#@";
    int ascii_map_length = strlen(ascii_map);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            float normalized = (max_brightness - min_brightness != 0)
                               ? (brightness_map[i][j] - min_brightness) / (max_brightness - min_brightness)
                               : 0;

            int index = (int)(normalized * (ascii_map_length - 1));
            screen[i][j] = ascii_map[index];
        }
    }

    // Free the brightness map
    for (int i = 0; i < N; i++) {
        free(brightness_map[i]);
    }
    free(brightness_map);
}

void print_2d_screen(char **screen, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%c ", screen[i][j]);
        }
        printf("\n");
    }
}

void free_screen(char **screen, int N) {
    if (!screen) return;
    for (int i = 0; i < N; i++) {
        free(screen[i]);
    }
    free(screen);
}

void free_line_space(LineSpace *lines) {
    if (!lines) return;
    free(lines->lines);
    free(lines);
}

void save_2d_screen(char **screen, const char *filename) {
    if (screen == NULL) {
        printf("Error: Screen buffer is NULL.\n");
        return;
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing\n", filename);
        return;
    }

    // Determine N dynamically by counting the number of rows (assuming screen is properly allocated)
    int N = 0;
    while (screen[N] != NULL) {
        N++;
    }

    // Write the screen content to the file
    for (int i = 0; i < N; i++) {
        if (screen[i] != NULL) {
            fprintf(file, "%s\n", screen[i]);  // Write the entire row with a newline
        }
    }

    fclose(file);
    printf("2D screen saved to %s\n", filename);
}




// Transformation
void rotate_point(Point *p, float theta_x, float theta_y, float theta_z) {
    float x = p->x;
    float y = p->y;
    float z = p->z;

    // Rotation around X-axis
    float new_y = y * cos(theta_x) - z * sin(theta_x);
    float new_z = y * sin(theta_x) + z * cos(theta_x);
    y = new_y;
    z = new_z;

    // Rotation around Y-axis
    float new_x = x * cos(theta_y) + z * sin(theta_y);
    new_z = -x * sin(theta_y) + z * cos(theta_y);
    x = new_x;
    z = new_z;

    // Rotation around Z-axis
    new_x = x * cos(theta_z) - y * sin(theta_z);
    new_y = x * sin(theta_z) + y * cos(theta_z);
    x = new_x;
    y = new_y;

    p->x = x;
    p->y = y;
    p->z = z;
}

ConstructedSpace* rotate_constructed_space(ConstructedSpace* input_space, int transformation_params[3]) {
    int N = input_space->N;
    float theta_x = transformation_params[0] * M_PI / 180.0;
    float theta_y = transformation_params[1] * M_PI / 180.0;
    float theta_z = transformation_params[2] * M_PI / 180.0;

    Cube *new_space = create_empty_3d_space(N);

    int center = N / 2;

    // Rotate each pixel and assign it to the new space
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                Point p = {i - center, j - center, k - center};
                rotate_point(&p, theta_x, theta_y, theta_z);

                int new_i = roundf(p.x) + center;
                int new_j = roundf(p.y) + center;
                int new_k = roundf(p.z) + center;

                if (new_i >= 0 && new_i < N && new_j >= 0 && new_j < N && new_k >= 0 && new_k < N) {
                    new_space->pixels[new_i][new_j][new_k] = input_space->space->pixels[i][j][k];
                }
            }
        }
    }

    ConstructedSpace *constructed = (ConstructedSpace *)malloc(sizeof(ConstructedSpace));
    if (!constructed) {
        fprintf(stderr, "Memory allocation failed for ConstructedSpace\n");
        free_constructed_space(input_space);
        free_cube(new_space);
        exit(1);
    }

    constructed->space = new_space;
    constructed->lines = NULL;
    constructed->N = N;
    return constructed;
}