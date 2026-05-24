#include "animation.hpp"
#include <chrono>
#include <thread>

// ─── Debug Utilities ─────────────────────────────────────────────────────────

void debug_print_constructed_space(const ConstructedSpace* cs,
                                   const std::vector<std::string>& screen) {
    if (!cs)
        std::cout << "constructed_space: NULL\n";
    else
        std::cout << "constructed_space: valid (N = " << cs->N << ")\n"
                  << "Line count: " << cs->lines->size() << "\n";

    std::cout << (screen.empty() ? "screen: NULL\n" : "screen: valid\n");
}

float calculate_average_brightness(const Cube* space) {
    if (!space || space->N <= 0) {
        std::cerr << "Invalid space provided.\n";
        return 0.0f;
    }
    int N = space->N;
    double total = 0.0;
    long long count = 0;
    for (int x = 0; x < N; x++)
        for (int y = 0; y < N; y++)
            for (int z = 0; z < N; z++) {
                total += space->pixels[x][y][z].brightness;
                count++;
            }
    return count ? static_cast<float>(total / count) : 0.0f;
}

// ─── Display Helpers ─────────────────────────────────────────────────────────

void display_menu() {
    std::cout << "\n=== 3D Space Construction Menu ===\n"
              << "1. Choose Line Construction\n"
              << "2. Create Object\n"
              << "3. Transform Object\n"
              << "4. Save/Load Object\n"
              << "5. Exit\n"
              << "===================================\n"
              << "Enter your choice: ";
}

void render_and_display(const ConstructedSpace* cs) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    if (cs && cs->space) {
        int N = cs->N;
        std::vector<std::string> screen;
        render_2d_face(cs->space, FRONT, screen);
        std::cout << "\n=== Rendered 2D Front View ===\n";
        print_2d_screen(screen);
    } else {
        std::cout << "\nNo object to display. Create or load an object first.\n";
    }

    display_menu();
}

// ─── Menu: Line Construction ─────────────────────────────────────────────────

void choose_line_construction() {
    std::cout << ">> Choose Line Construction\n"
              << "1. Limitless\n"
              << "2. Go Back\n"
              << "Enter your choice: ";
    int choice;
    std::cin >> choice;

    if (choice == 1) {
        // Limitless mode selected (future implementation)
    } else if (choice == 2) {
        return;
    } else {
        std::cout << "Invalid choice. Returning to menu.\n";
    }
}

// ─── Menu: Create Object ─────────────────────────────────────────────────────

void create_object(ConstructedSpace*& cs) {
    std::cout << ">> Create Object\n"
              << "1. Choose from Sample\n"
              << "2. Go Back\n"
              << "Enter your choice: ";

    int choice;
    if (!(std::cin >> choice)) {
        std::cerr << "Invalid input. Returning to menu.\n";
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        return;
    }

    if (choice == 2) return;
    if (choice != 1) { std::cout << "Invalid choice.\n"; return; }

    // ---- Sample submenu ----
    std::cout << "Choose a sample:\n"
              << "1. Hollow Sphere\n"
              << "2. Filled Sphere\n"
              << "3. Hollow Cube\n"
              << "4. Filled Cube\n"
              << "5. Hollow Donut\n"
              << "6. Filled Donut\n"
              << "7. Go Back\n"
              << "Enter your choice: ";

    int sample_choice;
    if (!(std::cin >> sample_choice)) {
        std::cerr << "Invalid input. Returning to menu.\n";
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        return;
    }
    if (sample_choice == 7) return;

    // ---- Collect MediumParameters ----
    MediumParameters medium_params;

    std::cout << "Enter size of the 3D space: ";
    if (!(std::cin >> medium_params.N) || medium_params.N <= 0) {
        std::cerr << "Invalid size. Returning to menu.\n"; return;
    }
    std::cout << "Enter medium transparency (0.0 - 1.0): ";
    if (!(std::cin >> medium_params.medium_transparency) ||
        medium_params.medium_transparency < 0.0f || medium_params.medium_transparency > 1.0f) {
        std::cerr << "Invalid transparency. Returning to menu.\n"; return;
    }
    std::cout << "Enter medium brightness (0.0 - 1.0): ";
    if (!(std::cin >> medium_params.medium_brightness) ||
        medium_params.medium_brightness < 0.0f || medium_params.medium_brightness > 1.0f) {
        std::cerr << "Invalid brightness. Returning to menu.\n"; return;
    }

    // ---- Collect LineParameters ----
    LineParameters line_params;
    std::cout << "Enter line brightness (0.0 - 1.0): ";
    if (!(std::cin >> line_params.line_brightness) ||
        line_params.line_brightness < 0.0f || line_params.line_brightness > 1.0f) {
        std::cerr << "Invalid line brightness. Returning to menu.\n"; return;
    }

    // ---- Free old space ----
    delete cs;
    cs = nullptr;

    // ---- Build requested shape ----
    if (sample_choice == 1 || sample_choice == 2) {
        int object_params[2];
        std::cout << "Enter sphere diameter (must be smaller than 3D space size): ";
        if (!(std::cin >> object_params[0]) ||
            object_params[0] >= medium_params.N || object_params[0] <= 0) {
            std::cerr << "Invalid diameter. Returning to menu.\n"; return;
        }
        cs = (sample_choice == 1)
             ? construct_sphere(medium_params, line_params, object_params)
             : construct_filled_sphere(medium_params, line_params, object_params);

    } else if (sample_choice == 3 || sample_choice == 4) {
        int object_params[1];
        std::cout << "Enter cube length (must be smaller than space size): ";
        if (!(std::cin >> object_params[0]) ||
            object_params[0] >= medium_params.N || object_params[0] <= 0) {
            std::cerr << "Invalid length. Returning to menu.\n"; return;
        }
        cs = (sample_choice == 3)
             ? construct_cube(medium_params, line_params, object_params)
             : construct_filled_cube(medium_params, line_params, object_params);

    } else if (sample_choice == 5 || sample_choice == 6) {
        int object_params[2];
        std::cout << "Enter major radius (must be < half space size): ";
        if (!(std::cin >> object_params[0]) ||
            object_params[0] >= medium_params.N / 2 || object_params[0] <= 0) {
            std::cerr << "Invalid major radius. Returning to menu.\n"; return;
        }
        std::cout << "Enter minor radius (must be < half space size): ";
        if (!(std::cin >> object_params[1]) ||
            object_params[1] >= medium_params.N / 2 || object_params[1] <= 0) {
            std::cerr << "Invalid minor radius. Returning to menu.\n"; return;
        }
        cs = (sample_choice == 5)
             ? construct_donut(medium_params, line_params, object_params)
             : construct_filled_donut(medium_params, line_params, object_params);

    } else {
        std::cout << "Invalid sample choice. Returning to menu.\n";
        return;
    }

    if (!cs)
        std::cout << "Error creating sample object.\n";
    else
        std::cout << "Object created successfully. N = " << cs->N << "\n";
}

// ─── Menu: Transform Object ──────────────────────────────────────────────────

void transform_object(ConstructedSpace* cs) {
    if (!cs) {
        std::cout << ">> No object to transform.\n";
        return;
    }

    while (true) {
        std::cout << ">> Transform Object\n"
                  << "1. Rotate object\n"
                  << "2. Go back\n"
                  << "Enter your choice: ";

        int choice;
        std::cin >> choice;

        if (choice == 2) { std::cout << ">> Returning to main menu.\n"; break; }
        if (choice != 1) { std::cout << ">> Invalid choice.\n"; continue; }

        int transformation_params[3];
        std::cout << "Enter rotation angles (degrees/second) for X Y Z axes (e.g. 30 45 60): ";
        std::cin >> transformation_params[0] >> transformation_params[1] >> transformation_params[2];

        int duration;
        std::cout << "Enter rotation duration (in seconds): ";
        std::cin >> duration;

        int fps          = 12;
        int total_frames = duration * fps;

        float fx = static_cast<float>(transformation_params[0]) / fps;
        float fy = static_cast<float>(transformation_params[1]) / fps;
        float fz = static_cast<float>(transformation_params[2]) / fps;

        auto frame_duration = std::chrono::milliseconds(1000 / fps);

        int accumulated[3] = {0, 0, 0};

        for (int frame = 0; frame < total_frames; frame++) {
            accumulated[0] += static_cast<int>(fx);
            accumulated[1] += static_cast<int>(fy);
            accumulated[2] += static_cast<int>(fz);

            ConstructedSpace* rotated = rotate_constructed_space(cs, accumulated);
            if (!rotated) { std::cout << "Error: Failed to rotate the object.\n"; return; }

            render_and_display(rotated);

            float avg = calculate_average_brightness(cs->space);
            std::cout << ">> Frame " << frame + 1 << "/" << total_frames
                      << " (Avg. Brightness: " << avg << ")\n";

            delete rotated;
            std::this_thread::sleep_for(frame_duration);
        }

        std::cout << ">> Rotation complete.\n";
    }
}

// ─── Menu: Save / Load ───────────────────────────────────────────────────────

void list_files_in_directory(const std::string& directory, std::vector<std::string>& files) {
    files.clear();
    DIR* dp = opendir(directory.c_str());
    if (!dp) { perror("opendir"); return; }

    struct dirent* entry;
    while ((entry = readdir(dp)) != nullptr) {
        if (entry->d_type == DT_REG)
            files.push_back(entry->d_name);
    }
    closedir(dp);
}

void save_load_object(ConstructedSpace*& cs) {
    std::cout << ">> Save/Load Object\n"
              << "1. Save\n2. Load\n3. Go Back\nEnter your choice: ";

    int choice;
    if (!(std::cin >> choice)) {
        std::cerr << "Invalid input.\n";
        std::cin.clear(); std::cin.ignore(1000, '\n');
        return;
    }

    if (choice == 3) return;

    if (choice == 1) {
        std::string name;
        std::cout << "Enter object name to save: ";
        std::cin >> name;

        if (!cs) { std::cout << "No object to save.\n"; return; }

        std::string obj_file    = "Object Presets/"  + name + ".txt";
        std::string render_file = "Render Presets/"  + name + ".txt";

        save_3d_space(cs, obj_file);

        std::vector<std::string> screen;
        render_2d_face(cs->space, FRONT, screen);
        save_2d_screen(screen, render_file);

        std::cout << "Object saved to " << obj_file
                  << " and render saved to " << render_file << ".\n";

    } else if (choice == 2) {
        delete cs;
        cs = nullptr;

        std::vector<std::string> files;
        list_files_in_directory("Object Presets", files);

        if (files.empty()) {
            std::cout << "No saved objects found in Object Presets.\n";
            return;
        }

        std::cout << "Available objects to load:\n";
        for (int i = 0; i < static_cast<int>(files.size()); i++)
            std::cout << i + 1 << ". " << files[i] << "\n";

        std::cout << "Enter the number of the object to load: ";
        int sel;
        if (!(std::cin >> sel) || sel < 1 || sel > static_cast<int>(files.size())) {
            std::cerr << "Invalid choice.\n"; return;
        }

        std::string obj_file = "Object Presets/" + files[sel - 1];
        cs = load_3d_space(obj_file);

        if (cs)
            std::cout << "Object loaded from " << obj_file << ".\n";
        else
            std::cout << "Failed to load object.\n";

    } else {
        std::cout << "Invalid choice.\n";
    }
}

// ─── Main ────────────────────────────────────────────────────────────────────

int main() {
    ConstructedSpace* cs = nullptr;

    while (true) {
        render_and_display(cs);

        int choice;
        if (!(std::cin >> choice)) {
            std::cerr << "Invalid input. Please enter a number.\n";
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }

        switch (choice) {
            case 1: choose_line_construction();  break;
            case 2: create_object(cs);           break;
            case 3: transform_object(cs);        break;
            case 4: save_load_object(cs);        break;
            case 5:
                std::cout << ">> Exiting program. Goodbye!\n";
                delete cs;
                return 0;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }
}