#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>


// Define the structure for a pixel with RGB, transparency, and brightness
typedef struct {
    int r, g, b;
    float transparency;
    float brightness;
} Pixel;

// Structure to represent the 3D space (cube)
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

Cube* create_empty_3d_space(int N) {
    Cube *space = malloc(sizeof(Cube));
    space->N = N;
    space->pixels = malloc(N * sizeof(Pixel**));

    for (int i = 0; i < N; i++) {
        space->pixels[i] = malloc(N * sizeof(Pixel*));
        for (int j = 0; j < N; j++) {
            space->pixels[i][j] = malloc(N * sizeof(Pixel));
            for (int k = 0; k < N; k++) {
                space->pixels[i][j][k].r = 0; 
                space->pixels[i][j][k].g = 0;
                space->pixels[i][j][k].b = 0;
                space->pixels[i][j][k].transparency = 1.0; 
                space->pixels[i][j][k].brightness = 0.0;
            }
        }
    }

    return space;
}

Cube* create_donut_3d_space(int N) {
    Cube *space = create_empty_3d_space(N);

    // Smaller constants for the torus shape
    float R = N / 5.0;  // Distance from the center of the torus to the center of the tube (smaller than before)
    float r = N / 10.0; // Radius of the tube (also smaller)

    // Set the entire space to 95% transparency initially
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                space->pixels[i][j][k].transparency = 0.95;  // 95% transparency
                space->pixels[i][j][k].brightness = 0.0;     // Background brightness set to 0
            }
        }
    }

    // Create the filled donut shape with white color and full brightness
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                // Convert (i, j, k) to normalized coordinates centered at (N/2, N/2, N/2)
                float x = i - N / 2.0;
                float y = j - N / 2.0;
                float z = k - N / 2.0;

                // Calculate distance from the center axis in the XY plane
                float distance_xy = sqrt(x * x + y * y);

                // Check if the point is inside the filled torus
                float torus_equation = (R - distance_xy) * (R - distance_xy) + z * z;
                if (torus_equation <= r * r) {  // Fill within the tube radius
                    space->pixels[i][j][k].r = 255;
                    space->pixels[i][j][k].g = 255;
                    space->pixels[i][j][k].b = 255;
                    space->pixels[i][j][k].transparency = 0.95;  // 95% transparency
                    space->pixels[i][j][k].brightness = 1.0;    // Full brightness
                }
            }
        }
    }

    return space;
}

Cube* create_sphere_3d_space(int N) {
    Cube *space = create_empty_3d_space(N);

    // Radius of the sphere
    float radius = N / 4.0;
    float center = N / 2.0;

    // Set the entire space to 95% transparency initially
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                space->pixels[i][j][k].transparency = 0.99;  // 95% transparency
                space->pixels[i][j][k].brightness = 0.0;  // Set background brightness to 0
            }
        }
    }

    // Create the sphere shape with white color and constant brightness
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                // Calculate the distance from the center of the cube
                float distance = sqrt(pow(i - center, 2) + pow(j - center, 2) + pow(k - center, 2));

                // Check if the point (i, j, k) is within the sphere
                if (distance <= radius) {
                    space->pixels[i][j][k].r = 255;
                    space->pixels[i][j][k].g = 255;
                    space->pixels[i][j][k].b = 255;
                    space->pixels[i][j][k].transparency = 0.99;
                    space->pixels[i][j][k].brightness = 1.0;  // Full brightness
                }
            }
        }
    }

    return space;
}

Cube* load_3d_space(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s for reading\n", filename);
        return NULL;
    }

    int N;
    fscanf(file, "%d", &N);

    Cube *space = create_empty_3d_space(N);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                fscanf(file, "%d %d %d %f %f",
                       &space->pixels[i][j][k].r,
                       &space->pixels[i][j][k].g,
                       &space->pixels[i][j][k].b,
                       &space->pixels[i][j][k].transparency,
                       &space->pixels[i][j][k].brightness);
            }
        }
    }

    fclose(file);
    printf("3D space loaded from %s\n", filename);
    return space;
}

void save_3d_space(Cube *space, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing\n", filename);
        return;
    }

    fprintf(file, "%d\n", space->N); // Write the size of the 3D space

    for (int i = 0; i < space->N; i++) {
        for (int j = 0; j < space->N; j++) {
            for (int k = 0; k < space->N; k++) {
                Pixel p = space->pixels[i][j][k];
                fprintf(file, "%d %d %d %.2f %.2f\n", p.r, p.g, p.b, p.transparency, p.brightness);
            }
        }
    }

    fclose(file);
    printf("3D space saved to %s\n", filename);
}



void render_2d_face(Cube *space, Face face, char **screen) {
    // Clear the screen
    for (int i = 0; i < space->N; i++) {
        for (int j = 0; j < space->N; j++) {
            screen[i][j] = ' ';
        }
    }

    // Allocate memory for the brightness map
    float **brightness_map = malloc(space->N * sizeof(float *));
    for (int i = 0; i < space->N; i++) {
        brightness_map[i] = malloc(space->N * sizeof(float));
        for (int j = 0; j < space->N; j++) {
            brightness_map[i][j] = 0.0f;  // Initialize to zero
        }
    }

    // Variables for calculating min and max brightness across the screen
    float min_brightness = FLT_MAX;
    float max_brightness = -FLT_MAX;

    // Loop through each screen pixel
    for (int i = 0; i < space->N; i++) {
        for (int j = 0; j < space->N; j++) {
            float total_brightness = 0.0;
            float opacity_accumulation = 1.0;  // Start with full opacity

            // Traverse along the depth of the cube in the direction of the face
            for (int k = 0; k < space->N; k++) {
                Pixel p;
                float distance;

                // Determine the pixel and its distance based on the selected face
                if (face == FRONT) {
                    p = space->pixels[i][j][k];
                    distance = k + 1;  // 1-based depth for distance
                } else if (face == LEFT) {
                    p = space->pixels[k][i][j];
                    distance = k + 1;
                } else if (face == RIGHT) {
                    p = space->pixels[space->N - 1 - k][i][j];
                    distance = k + 1;
                } else if (face == BACK) {
                    p = space->pixels[i][j][space->N - 1 - k];
                    distance = k + 1;
                } else if (face == TOP) {
                    p = space->pixels[i][k][j];
                    distance = k + 1;
                } else if (face == BOTTOM) {
                    p = space->pixels[i][space->N - 1 - k][j];
                    distance = k + 1;
                } else {
                    continue;
                }

                // Calculate the brightness contribution with distance attenuation
                float inverse_square = 1.0f / (distance * distance);  // Inverse square law
                float effective_brightness = p.brightness * inverse_square * opacity_accumulation;

                // Accumulate brightness and update opacity due to transparency
                total_brightness += effective_brightness;
                opacity_accumulation *= p.transparency;  // Update opacity for subsequent pixels

                // Stop if fully opaque
                if (opacity_accumulation <= 0.01f) break;
            }

            // Track min and max brightness for normalization
            if (total_brightness < min_brightness) min_brightness = total_brightness;
            if (total_brightness > max_brightness) max_brightness = total_brightness;

            // Store the brightness in the map
            brightness_map[i][j] = total_brightness;
        }
    }

    // Normalize and map the raw total brightness to ASCII characters
    const char *ascii_map = " .:-=+*%#@";
    int ascii_map_length = strlen(ascii_map);

    for (int i = 0; i < space->N; i++) {
        for (int j = 0; j < space->N; j++) {
            // Normalize the brightness to a 0-1 range
            float normalized_brightness = (max_brightness - min_brightness != 0) ?
                (brightness_map[i][j] - min_brightness) / (max_brightness - min_brightness) : 0;

            // Map the normalized brightness to an index in the ASCII map
            int index = (int)(normalized_brightness * (ascii_map_length - 1));
            index = (index > ascii_map_length - 1) ? ascii_map_length - 1 : index;

            // Assign the corresponding ASCII character
            screen[i][j] = ascii_map[index];
        }
    }

    // Free the memory used by the brightness map
    for (int i = 0; i < space->N; i++) {
        free(brightness_map[i]);
    }
    free(brightness_map);
}

void print_2d_screen(char **screen) {
    // Assuming screen is square and its size is stored in the first row's length
    int N = 0;
    while (screen[N] != NULL) {
        N++;
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%c ", screen[i][j]);
        }
        printf("\n");
    }
}

void save_2d_screen(char **screen, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing\n", filename);
        return;
    }

    int N = 0;
    while (screen[N] != NULL) {
        N++;
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(file, "%c ", screen[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    printf("2D screen saved to %s\n", filename);
}




int main() {
    int N = 50; // Size of the 3D space

    // Create a 3D space initialized with a donut shape
    Cube *space = create_donut_3d_space(N);

    // Allocate memory for the 2D screen
    char **screen = malloc(N * sizeof(char*));
    for (int i = 0; i < N; i++) {
        screen[i] = malloc(N * sizeof(char));
    }

    // Render and print the 2D face (e.g., face 0)
    render_2d_face(space, FRONT, screen);
    print_2d_screen(screen);

    // Save the 3D space and 2D screen to files
    // save_3d_space(space, "Object Presets/donut_3d_space.txt");
    // save_2d_screen(screen, "Render Presets/donut_2d_render.txt");

    // Free memory
    for (int i = 0; i < N; i++) {
        free(screen[i]);
    }
    free(screen);

    // Free 3D space memory
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            free(space->pixels[i][j]);
        }
        free(space->pixels[i]);
    }
    free(space->pixels);
    free(space);

    return 0;
}