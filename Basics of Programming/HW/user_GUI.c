#include "animation.h"

// Debug
void debug_print_constructed_space(ConstructedSpace *constructed_space, char **screen) {
    if (!constructed_space) {
        printf("constructed_space: NULL\n");
    } else {
        printf("constructed_space: valid (N = %d)\n", constructed_space->N);
        printf("Line count: %d\n", constructed_space->lines->line_count);
    }

    if (!screen) {
        printf("screen: NULL\n");
    } else {
        printf("screen: valid\n");
    }
}

void debug_print_input_space(ConstructedSpace *input_space) {
    if (!input_space) {
        printf("input_space: NULL\n");
    } else {
        printf("input_space: valid (N = %d)\n", input_space->N);
        printf("Line count: %d\n", input_space->lines->line_count);
    }
}

float calculate_average_brightness(Cube* space) {
    if (!space || space->N <= 0) {
        fprintf(stderr, "Invalid space provided.\n");
        return 0.0f;
    }

    int N = space->N;
    long long total_brightness = 0;
    long long voxel_count = 0;

    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            for (int z = 0; z < N; z++) {
                total_brightness += space->pixels[x][y][z].brightness;
                voxel_count++;
            }
        }
    }

    if (voxel_count == 0) {
        fprintf(stderr, "No voxels found in the space.\n");
        return 0.0f;
    }

    return (float)total_brightness / voxel_count;
}



// Utility functions
// Display the main menu
void display_menu() {
    // Display the menu
    printf("\n=== 3D Space Construction Menu ===\n");
    printf("1. Choose Line Construction\n");
    printf("2. Create Object\n");
    printf("3. Transform Object\n");
    printf("4. Save/Load Object\n");
    printf("5. Exit\n");
    printf("===================================\n");
    printf("Enter your choice: ");
}

// Render the menu and display the constructed space
void render_menu_and_display(ConstructedSpace *constructed_space, char **screen) {
    // Clear the screen
#ifdef _WIN32
    system("cls"); // Clear screen for Windows
#else
    system("clear"); // Clear screen for Linux/macOS
#endif

    // Check if a constructed space exists
    if (constructed_space && constructed_space->space) {
        int N = constructed_space->N;

        if (N <= 0) {
            fprintf(stderr, "Error: Invalid constructed_space with N = %d.\n", N);
            return;
        }

        // Allocate screen for rendering
        screen = malloc(N * sizeof(char *));
        if (!screen) {
            fprintf(stderr, "Error: Failed to allocate memory for screen.\n");
            return;
        }

        // Allocate memory for each row
        for (int i = 0; i < N; i++) {
            screen[i] = malloc(N * sizeof(char));

            if (!screen[i]) {
                for (int j = 0; j < i; j++) {
                    free(screen[j]);
                }
                free(screen);
                screen = NULL;
                fprintf(stderr, "Error: Failed to allocate memory for screen row %d.\n", i);
                return;
            }
        }

        // Render the FRONT face
        render_2d_face(constructed_space->space, FRONT, screen);
        printf("\n=== Rendered 2D Front View ===\n");
        print_2d_screen(screen, N);

        // Free the screen after use
        free_screen(screen, N);
        screen = NULL;
    } else {
        printf("\nNo object to display. Create or load an object first.\n");
    }

    // Display the menu
    display_menu();
}

// Render the menu and display the input space
void render_menu_and_display_input_space(ConstructedSpace *input_space) {
    // Clear the screen
#ifdef _WIN32
    system("cls"); // Clear screen for Windows
#else
    system("clear"); // Clear screen for Linux/macOS
#endif
    
    // Check if an input space exists
    if (input_space && input_space->space) {
        int N = input_space->N;

        if (N <= 0) {
            fprintf(stderr, "Error: Invalid input_space with N = %d.\n", N);
            return;
        }

        // Allocate screen for rendering
        char **screen = malloc(N * sizeof(char *));
        if (!screen) {
            fprintf(stderr, "Error: Failed to allocate memory for screen.\n");
            return;
        }

        // Allocate memory for each row
        for (int i = 0; i < N; i++) {
            screen[i] = malloc(N * sizeof(char));
            if (!screen[i]) {
                for (int j = 0; j < i; j++) {
                    free(screen[j]);
                }
                free(screen);
                fprintf(stderr, "Error: Failed to allocate memory for screen row %d.\n", i);
                return;
            }
        }

        // Render the FRONT face
        render_2d_face(input_space->space, FRONT, screen);
        printf("\n=== Rendered 2D Front View ===\n");
        print_2d_screen(screen, N);

        // Free the screen after use
        for (int i = 0; i < N; i++) {
            free(screen[i]);
        }
        free(screen);
        screen = NULL;
    } else {
        printf("\nNo object to display. Create or load an object first.\n");
    }
    display_menu();
}

// Free the existing constructed space and screen
void reset_globals(ConstructedSpace **constructed_space, char **screen) {
    // Free existing constructed space
    if (constructed_space && *constructed_space) {
        free_constructed_space(*constructed_space);
        *constructed_space = NULL;
    }

    // Free existing screen
    if (screen) {
        int N = constructed_space ? (*constructed_space)->N : 0;
        free_screen(screen, N);
        *screen = NULL;
    }
}



// Menu functions
// Choose the line construction method
void choose_line_construction() {
    // Display the line construction menu
    int choice;

    printf(">> Choose Line Construction\n");
    printf("1. Limitless\n");
    printf("2. Go Back\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    if (choice == 1) {
    }
    else if (choice == 2) {
        return;
    }
    else {
        printf("Invalid choice. Returning to menu.\n");
    }
}

// Create a 3D object based on the user's choice
void create_object(ConstructedSpace **constructed_space, char **screen) {
    // Display the object creation menu
    int choice;

    printf(">> Create Object\n");
    printf("1. Choose from Sample\n");
    printf("2. Go Back\n");
    printf("Enter your choice: ");

    // Check for valid input
    if (scanf("%d", &choice) != 1) {
        fprintf(stderr, "Invalid input. Returning to menu.\n");
        while (getchar() != '\n'); // Clear invalid input
        return;
    }

    // Free existing constructed space
    reset_globals(constructed_space, screen);

    // Handle the different choices
    if (choice == 1) {
        int sample_choice;
        printf("Choose a sample:\n");
        printf("1. Hollow Sphere\n");
        printf("2. Filled Sphere\n");
        printf("3. Hollow Cube\n");
        printf("4. Filled Cube\n");
        printf("5. Hollow Donut\n");
        printf("6. Filled Donut\n");
        printf("7. Go Back\n");
        printf("Enter your choice: ");
        if (scanf("%d", &sample_choice) != 1) {
            fprintf(stderr, "Invalid input. Returning to menu.\n");
            while (getchar() != '\n');
            return;
        }

        if (choice == 7) {
            return;
        }

        // Collect MediumParameters
        MediumParameters medium_params;

        // Collect size of the 3D space
        printf("Enter size of the 3D space: ");
        if (scanf("%d", &medium_params.N) != 1 || medium_params.N <= 0) {
            fprintf(stderr, "Invalid size. Returning to menu.\n");
            return;
        }
        // Collect transparency of the medium
        printf("Enter medium transparency (0.0 - 1.0): ");
        if (scanf("%f", &medium_params.medium_transparency) != 1 || 
            medium_params.medium_transparency < 0.0 || medium_params.medium_transparency > 1.0) {
            fprintf(stderr, "Invalid transparency. Returning to menu.\n");
            return;
        }
        // Collect brightness of the medium
        printf("Enter medium brightness (0.0 - 1.0): ");
        if (scanf("%f", &medium_params.medium_brightness) != 1 || 
            medium_params.medium_brightness < 0.0 || medium_params.medium_brightness > 1.0) {
            fprintf(stderr, "Invalid brightness. Returning to menu.\n");
            return;
        }

        // Collect LineParameters
        LineParameters line_params;
        printf("Enter line brightness (0.0 - 1.0): ");
        if (scanf("%f", &line_params.line_brightness) != 1 || 
            line_params.line_brightness < 0.0 || line_params.line_brightness > 1.0) {
            fprintf(stderr, "Invalid line brightness. Returning to menu.\n");
            return;
        }

        // Handle the different sample choices

        // Construct sphere
        if (sample_choice == 1 || sample_choice == 2) {  // Sphere
            int object_params[1];  // {Diameter}
            printf("Enter sphere diameter (must be smaller than 3D space size): ");
            if (scanf("%d", &object_params[0]) != 1 || object_params[0] >= medium_params.N || object_params[0] <= 0) {
                fprintf(stderr, "Invalid diameter. Returning to menu.\n");
                return;
            }

            *constructed_space = (sample_choice == 1)
                                ? construct_sphere(medium_params, line_params, object_params)
                                : construct_filled_sphere(medium_params, line_params, object_params);
        }

        // Construct cube
        else if (sample_choice == 3 || sample_choice == 4) {  // Cube
            int object_params[1];
            printf("Enter cube length (must be smaller than space size): ");
            if (scanf("%d", &object_params[0]) != 1 || object_params[0] >= medium_params.N || object_params[0] <= 0) {
                fprintf(stderr, "Invalid length. Returning to menu.\n");
                return;
            }

            *constructed_space = (sample_choice == 3)
                                ? construct_cube(medium_params, line_params, object_params)
                                : construct_filled_cube(medium_params, line_params, object_params);

        }
        // Construct donut
        else if (sample_choice == 5 || sample_choice == 6) {  // Hollow/Filled Donut
            int object_params[2];
            printf("Enter major radius length (must be smaller than half space size): ");
            if (scanf("%d", &object_params[0]) != 1 || object_params[0] >= medium_params.N / 2 || object_params[0] <= 0) {
                fprintf(stderr, "Invalid major radius. Returning to menu.\n");
                return;
            }

            printf("Enter minor radius length (must be smaller than half space size): ");
            if (scanf("%d", &object_params[1]) != 1 || object_params[1] >= medium_params.N / 2 || object_params[1] <= 0) {
                fprintf(stderr, "Invalid minor radius. Returning to menu.\n");
                return;
            }

            *constructed_space = (sample_choice == 5)
                                ? construct_donut(medium_params, line_params, object_params)
                                : construct_filled_donut(medium_params, line_params, object_params);
        } else {
            printf("Invalid sample choice. Returning to menu.\n");
            return;
        }

        if (!*constructed_space) {
            printf("Error creating sample object.\n");
        } else {
            printf("Object created successfully. N = %d\n", (*constructed_space)->N);
            debug_print_constructed_space(*constructed_space, screen);
        }
    } else if (choice == 2) {  // Go Back
        return;
    } else {
        printf("Invalid choice. Returning to menu.\n");
    }
}

// Rotate the constructed space around the X, Y, and Z axes
void transform_object(ConstructedSpace *constructed_space, char **screen) {
    // Check if a constructed space exists
    ConstructedSpace* current_space = constructed_space;
    if (!current_space) {
        printf(">> No object to transform.\n");
        return;
    }

    // Display the transform object menu
    while (1) {
        printf(">> Transform object selected.\n");
        printf("Options:\n");
        printf("1. Rotate object\n");
        printf("2. Go back\n");
        printf("Enter your choice: ");

        int choice;
        scanf("%d", &choice);

        if (choice == 1) { // Rotate option
            printf(">> Rotate object selected.\n");

            int transformation_params[3];
            printf("Enter rotation angles (degrees per second) for X, Y, Z axes (e.g., 30 45 60): ");
            scanf("%d %d %d", &transformation_params[0], &transformation_params[1], &transformation_params[2]);

            int duration;
            printf("Enter rotation duration (in seconds): ");
            scanf("%d", &duration);

            int fps = 12; // Frames per second for rendering
            int total_frames = duration * fps;

            // Calculate the frame increments for each axis
            float frame_increment_x = (float)transformation_params[0] / fps;
            float frame_increment_y = (float)transformation_params[1] / fps;
            float frame_increment_z = (float)transformation_params[2] / fps;

            struct timespec frame_delay = {
                .tv_sec = 0,
                .tv_nsec = 1000000000 / fps // Frame duration in nanoseconds
            };

            int accumulated_rotation[3] = {0, 0, 0}; // Accumulated rotation values

            for (int frame = 0; frame < total_frames; frame++) {
                // Increment the accumulated rotation
                accumulated_rotation[0] += frame_increment_x;
                accumulated_rotation[1] += frame_increment_y;
                accumulated_rotation[2] += frame_increment_z;

                // Rotate the constructed space
                ConstructedSpace* rotated_space = rotate_constructed_space(current_space, accumulated_rotation);
                if (!rotated_space) {
                    printf("Error: Failed to rotate the object.\n");
                    return;
                }

                // Clear screen and render the rotated object
                render_menu_and_display_input_space(rotated_space);

                // Indicate the current frame
                float average_brightness = calculate_average_brightness(constructed_space->space);
                printf(">> Frame %d/%d (Avg. Brightness: %.2f)\n", frame + 1, total_frames, average_brightness);

                // Free the rotated space to avoid memory leaks
                free_constructed_space(rotated_space);

                // Sleep for the frame duration
                nanosleep(&frame_delay, NULL);
            }

            printf(">> Rotation complete.\n");

        } else if (choice == 2) { // Go back option
            printf(">> Returning to main menu.\n");
            break;
        } else {
            printf(">> Invalid choice. Please try again.\n");
        }
    }
}

// Helper function to get a list of files in a directory
void list_files_in_directory(const char *directory, char files[][100], int *file_count) {
    // Open the directory
    struct dirent *entry;
    DIR *dp = opendir(directory);
    if (dp == NULL) {
        perror("opendir");
        return;
    }

    // Read each entry in the directory
    *file_count = 0;
    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_type == DT_REG) {  // Only regular files
            strcpy(files[*file_count], entry->d_name);
            (*file_count)++;
        }
    }
    closedir(dp);
}

// Save or load an object
void save_load_object(ConstructedSpace **constructed_space, char **screen) {
    int choice;
    char filename[100];

    printf(">> Save/Load Object\n");
    printf("1. Save\n2. Load\n3. Go Back\nEnter your choice: ");
    if (scanf("%d", &choice) != 1) {
        fprintf(stderr, "Invalid input.\n");
        return;
    }

    // Handle the different choices
    // Save an object
    if (choice == 1) {  // Save
        printf("Enter object name to save: ");
        if (scanf("%s", filename) != 1) {
            fprintf(stderr, "Invalid input.\n");
            return;
        }

        // Check if a constructed space exists
        if (*constructed_space) {
            char object_filename[150], render_filename[150];
            snprintf(object_filename, sizeof(object_filename), "Object Presets/%s.txt", filename);
            snprintf(render_filename, sizeof(render_filename), "Render Presets/%s.txt", filename);

            char **screen = malloc((*constructed_space)->N * sizeof(char *));
            if (!screen) {
                fprintf(stderr, "Failed to allocate memory for screen.\n");
                return;
            }
            for (int i = 0; i < (*constructed_space)->N; i++) {
                screen[i] = malloc((*constructed_space)->N * sizeof(char));
                if (!screen[i]) {
                    free_screen(screen, (*constructed_space)->N);
                    fprintf(stderr, "Failed to allocate memory for screen row %d.\n", i);
                    return;
                }
            }

            // Save the object and render
            save_3d_space(*constructed_space, object_filename);

            render_2d_face((*constructed_space)->space, FRONT, screen);
            save_2d_screen(screen, render_filename);

            printf("Object saved to %s and render saved to %s.\n", object_filename, render_filename);
            
            // Free the screen after use
            free_screen(screen, (*constructed_space)->N);
            screen = NULL;

            debug_print_constructed_space(*constructed_space, screen);
        } else {
            printf("No object to save.\n");
        }
    }

    // Load an object
    else if (choice == 2) {  // Load
        reset_globals(constructed_space, screen);

        // List files in the Object Presets directory
        char files[100][100];
        int file_count = 0;
        list_files_in_directory("Object Presets", files, &file_count);

        // Check if any files were found
        if (file_count == 0) {
            printf("No saved objects found in Object Presets.\n");
            return;
        }

        // Display the available objects to load
        printf("Available objects to load:\n");
        for (int i = 0; i < file_count; i++) {
            printf("%d. %s\n", i + 1, files[i]);
        }

        // Select a file to load
        int selected_file;
        printf("Enter the number of the object to load: ");
        if (scanf("%d", &selected_file) != 1 || selected_file < 1 || selected_file > file_count) {
            fprintf(stderr, "Invalid choice.\n");
            return;
        }

        // Load the selected object
        char object_filename[150];
        snprintf(object_filename, sizeof(object_filename), "Object Presets/%s", files[selected_file - 1]);
        *constructed_space = load_3d_space(object_filename);
        if (*constructed_space) {
            printf("Object loaded from %s.\n", object_filename);
            debug_print_constructed_space(*constructed_space, screen);
        } else {
            printf("Failed to load object.\n");
        }
    } else if (choice == 3) {
        return;
    } else {
        printf("Invalid choice.\n");
    }
}

// Exit the program
void exit_program() {
    printf(">> Exiting program. Goodbye!\n");
    exit(0);
}



// Main function
int main() {
    ConstructedSpace *constructed_space = NULL;
    char **screen = NULL;

    int choice;

    // Main loop
    while (1) {
        // Display the menu and get the user's choice
        render_menu_and_display(constructed_space, screen);
        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        // Handle the user's choice
        switch (choice) {
            case 1: choose_line_construction() ; break;
            case 2: create_object(&constructed_space, screen); break;
            case 3: transform_object(constructed_space, screen); break;
            case 4: save_load_object(&constructed_space, screen); break;
            case 5: exit_program(); break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
