#include "animation.hpp"
#include <chrono>
#include <thread>
#include <random>

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
    if (!space || space->N <= 0) return 0.0f;
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
              << "1. Create Generic Object\n"
              << "2. Ray Tracing Simulation\n"
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
        std::vector<std::string> screen;
        render_2d_face(cs->space, FRONT, screen);
        std::cout << "\n=== Rendered 2D Front View ===\n";
        print_2d_screen(screen);
    } else {
        std::cout << "\nNo object to display. Create or load an object first.\n";
    }

    display_menu();
}

// ─── Menu: Ray Tracing Simulations ───────────────────────────────────────────

void static_ray_tracing(ConstructedSpace*& cs) {
    std::cout << "\n--- Static Random Rays (Cloud/Nebula) ---\n";
    
    MediumParameters medium_params;
    std::cout << "Enter size of the 3D space (e.g. 30): ";
    std::cin >> medium_params.N;

    std::cout << "Enter mist transparency (0.1 to 0.99, e.g. 0.95): ";
    std::cin >> medium_params.medium_transparency;
    medium_params.medium_brightness = 0.0f;

    std::cout << "\nSelect Projector Layout:\n"
              << "1. Tetrahedron (4 sources)\n"
              << "2. Square Pyramid (5 sources)\n"
              << "3. Octahedron (6 sources)\n"
              << "4. Cube (8 sources)\n"
              << "Enter choice: ";
    int layout_choice;
    std::cin >> layout_choice;

    std::cout << "Enter rays per projector (64, 128, 256, 512): ";
    int ray_count;
    std::cin >> ray_count;

    std::cout << "Enable secondary lighting? (1 for Yes [auto-calculates until decay], 0 for No): ";
    int enable_secondary;
    std::cin >> enable_secondary;

    Cube* space = create_empty_3d_space(medium_params.N);
    LineSpace* lines = new LineSpace();
    
    for (int i = 0; i < medium_params.N; i++)
        for (int j = 0; j < medium_params.N; j++)
            for (int k = 0; k < medium_params.N; k++)
                space->pixels[i][j][k] = { 0, 0, 0, medium_params.medium_transparency, 0.0f };

    float center = medium_params.N / 2.0f;
    float R_proj = 1.5f * (medium_params.N * std::sqrt(3.0f) / 2.0f);
    float inv_sqrt3 = 1.0f / std::sqrt(3.0f);
    float inv_sqrt2 = 1.0f / std::sqrt(2.0f);

    std::vector<Vector3> projector_dirs;
    if (layout_choice == 1) {
        projector_dirs = {{inv_sqrt3, inv_sqrt3, inv_sqrt3}, {-inv_sqrt3, -inv_sqrt3, inv_sqrt3}, {-inv_sqrt3, inv_sqrt3, -inv_sqrt3}, {inv_sqrt3, -inv_sqrt3, -inv_sqrt3}};
    } else if (layout_choice == 2) {
        projector_dirs = {{0.0f, 1.0f, 0.0f}, {inv_sqrt2, -0.5f, inv_sqrt2}, {-inv_sqrt2, -0.5f, inv_sqrt2}, {-inv_sqrt2, -0.5f, -inv_sqrt2}, {inv_sqrt2, -0.5f, -inv_sqrt2}};
        for(auto& v : projector_dirs) v.normalize();
    } else if (layout_choice == 3) {
        projector_dirs = {{1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}};
    } else {
        projector_dirs = {{inv_sqrt3, inv_sqrt3, inv_sqrt3}, {-inv_sqrt3, inv_sqrt3, inv_sqrt3}, {inv_sqrt3, -inv_sqrt3, inv_sqrt3}, {-inv_sqrt3, -inv_sqrt3, inv_sqrt3}, {inv_sqrt3, inv_sqrt3, -inv_sqrt3}, {-inv_sqrt3, inv_sqrt3, -inv_sqrt3}, {inv_sqrt3, -inv_sqrt3, -inv_sqrt3}, {-inv_sqrt3, -inv_sqrt3, -inv_sqrt3}};
    }

    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_real_distribution<float> target_dist(0.0f, static_cast<float>(medium_params.N));
    std::uniform_real_distribution<float> intensity_dist(100.0f, 500.0f);

    std::cout << "Shooting rays... ";
    for (const auto& dir : projector_dirs) {
        Point proj_pos = { center + dir.dx * R_proj, center + dir.dy * R_proj, center + dir.dz * R_proj };
        for (int i = 0; i < ray_count; i++) {
            Point target = { target_dist(rng), target_dist(rng), target_dist(rng) };
            Ray ray;
            ray.origin = proj_pos;
            ray.direction = { target.x - proj_pos.x, target.y - proj_pos.y, target.z - proj_pos.z };
            ray.intensity = intensity_dist(rng);
            shoot_ray(space, ray, medium_params.medium_transparency);
        }
    }
    std::cout << "Done!\n";

    if (enable_secondary == 1) {
        std::cout << "Calculating secondary light scattering... ";
        apply_secondary_lighting(space, -1);
        std::cout << "Done!\n";
    }

    delete cs;
    cs = new ConstructedSpace(space, lines, medium_params.N);
}

void animated_ray_traced_sphere(ConstructedSpace*& cs) {
    std::cout << "\n--- Animated Ray Traced Sphere ---\n";
    
    int N;
    std::cout << "Enter size of the 3D space (e.g. 30): ";
    std::cin >> N;

    float mist;
    std::cout << "Enter mist transparency (e.g. 0.95): ";
    std::cin >> mist;

    float diameter;
    std::cout << "Enter sphere diameter: ";
    std::cin >> diameter;
    float radius = diameter / 2.0f;

    std::cout << "\nSelect Projector Layout:\n"
              << "1. Tetrahedron (4 sources)\n"
              << "2. Square Pyramid (5 sources)\n"
              << "3. Octahedron (6 sources)\n"
              << "4. Cube (8 sources)\n"
              << "Enter choice: ";
    int layout_choice;
    std::cin >> layout_choice;

    int ray_count;
    std::cout << "Enter rays per projector (e.g. 256, 512): ";
    std::cin >> ray_count;

    float dx, dy, dz;
    std::cout << "Enter translation amounts for X Y Z (e.g., 10 0 -5): ";
    std::cin >> dx >> dy >> dz;

    int duration;
    std::cout << "Enter movement duration (in seconds): ";
    std::cin >> duration;

    float center_orig = N / 2.0f;
    float R_proj = 1.5f * (N * std::sqrt(3.0f) / 2.0f);
    float inv_sqrt3 = 1.0f / std::sqrt(3.0f);
    float inv_sqrt2 = 1.0f / std::sqrt(2.0f);
    
    std::vector<Vector3> projector_dirs;
    if (layout_choice == 1) {
        projector_dirs = {{inv_sqrt3, inv_sqrt3, inv_sqrt3}, {-inv_sqrt3, -inv_sqrt3, inv_sqrt3}, {-inv_sqrt3, inv_sqrt3, -inv_sqrt3}, {inv_sqrt3, -inv_sqrt3, -inv_sqrt3}};
    } else if (layout_choice == 2) {
        projector_dirs = {{0.0f, 1.0f, 0.0f}, {inv_sqrt2, -0.5f, inv_sqrt2}, {-inv_sqrt2, -0.5f, inv_sqrt2}, {-inv_sqrt2, -0.5f, -inv_sqrt2}, {inv_sqrt2, -0.5f, -inv_sqrt2}};
        for(auto& v : projector_dirs) v.normalize();
    } else if (layout_choice == 3) {
        projector_dirs = {{1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}};
    } else {
        projector_dirs = {{inv_sqrt3, inv_sqrt3, inv_sqrt3}, {-inv_sqrt3, inv_sqrt3, inv_sqrt3}, {inv_sqrt3, -inv_sqrt3, inv_sqrt3}, {-inv_sqrt3, -inv_sqrt3, inv_sqrt3}, {inv_sqrt3, inv_sqrt3, -inv_sqrt3}, {-inv_sqrt3, inv_sqrt3, -inv_sqrt3}, {inv_sqrt3, -inv_sqrt3, -inv_sqrt3}, {-inv_sqrt3, -inv_sqrt3, -inv_sqrt3}};
    }

    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_real_distribution<float> dist_u(0.0f, 1.0f);
    std::uniform_real_distribution<float> dist_v(0.0f, 1.0f);
    std::uniform_real_distribution<float> dist_w(0.0f, 1.0f);
    std::uniform_real_distribution<float> intensity_dist(100.0f, 500.0f);

    int fps = 12;
    int total_frames = duration * fps;
    float step_x = dx / static_cast<float>(total_frames);
    float step_y = dy / static_cast<float>(total_frames);
    float step_z = dz / static_cast<float>(total_frames);

    for (int frame = 0; frame <= total_frames; frame++) {
        float cur_cx = center_orig + (step_x * frame);
        float cur_cy = center_orig + (step_y * frame);
        float cur_cz = center_orig + (step_z * frame);

        Cube* current_space = create_empty_3d_space(N);
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                for (int k = 0; k < N; k++)
                    current_space->pixels[i][j][k] = { 0, 0, 0, mist, 0.0f };

        for (const auto& dir : projector_dirs) {
            Point proj_pos = { center_orig + dir.dx * R_proj, center_orig + dir.dy * R_proj, center_orig + dir.dz * R_proj };

            for (int i = 0; i < ray_count; i++) {
                float u = dist_u(rng);
                float v = dist_v(rng);
                float w = dist_w(rng);
                
                float theta = 2.0f * static_cast<float>(M_PI) * u;
                float phi = std::acos(1.0f - 2.0f * v);
                float r = radius * std::cbrt(w);

                float cur_tx = r * std::sin(phi) * std::cos(theta);
                float cur_ty = r * std::sin(phi) * std::sin(theta);
                float cur_tz = r * std::cos(phi);

                Point target = { cur_cx + cur_tx, cur_cy + cur_ty, cur_cz + cur_tz };

                Ray ray;
                ray.origin = proj_pos;
                ray.direction = { target.x - proj_pos.x, target.y - proj_pos.y, target.z - proj_pos.z };
                ray.intensity = intensity_dist(rng);

                shoot_ray(current_space, ray, mist);
            }
        }

        ConstructedSpace* frame_cs = new ConstructedSpace(current_space, new LineSpace(), N);
        render_and_display(frame_cs);
        std::cout << ">> Frame " << frame << "/" << total_frames << " | Center: (" 
                  << cur_cx << ", " << cur_cy << ", " << cur_cz << ")\n";

        if (frame < total_frames) {
            delete frame_cs; 
            std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps));
        } else {
            delete cs; 
            cs = frame_cs;
        }
    }

    std::cout << ">> Animation complete.\n";
}

void ray_tracing_menu(ConstructedSpace*& cs) {
    std::cout << "\n>> Ray Tracing Simulation Sub-Menu\n"
              << "1. Static Random Rays (Cloud/Nebula)\n"
              << "2. Animated Ray Traced Sphere\n"
              << "3. Go Back\n"
              << "Enter choice: ";
              
    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear(); std::cin.ignore(1000, '\n'); return;
    }
    
    if (choice == 1) {
        static_ray_tracing(cs);
    } else if (choice == 2) {
        animated_ray_traced_sphere(cs);
    } else {
        return;
    }
}

// ─── Menu: Create Parametric Object ──────────────────────────────────────────

void create_object(ConstructedSpace*& cs) {
    std::cout << "\n>> Create Generic Object\n"
              << "1. Hollow Sphere\n"
              << "2. Filled Sphere\n"
              << "3. Hollow Cube\n"
              << "4. Filled Cube\n"
              << "5. Hollow Donut\n"
              << "6. Filled Donut\n"
              << "7. Go Back\n"
              << "Enter choice: ";
              
    int sample_choice; 
    if (!(std::cin >> sample_choice)) {
        std::cin.clear(); std::cin.ignore(1000, '\n'); return;
    }

    if (sample_choice == 7 || sample_choice < 1 || sample_choice > 7) return;

    MediumParameters mp;
    std::cout << "Size (N): "; std::cin >> mp.N;
    std::cout << "Medium transparency (0.0 - 1.0): "; std::cin >> mp.medium_transparency;
    std::cout << "Medium brightness (0.0 - 1.0): "; std::cin >> mp.medium_brightness;

    LineParameters lp;
    std::cout << "Line brightness (0.0 - 1.0): "; std::cin >> lp.line_brightness;

    delete cs; cs = nullptr;

    if (sample_choice == 1 || sample_choice == 2) {
        int params[2]; std::cout << "Diameter: "; std::cin >> params[0];
        cs = (sample_choice == 1) ? construct_sphere(mp, lp, params) : construct_filled_sphere(mp, lp, params);
    } else if (sample_choice == 3 || sample_choice == 4) {
        int params[1]; std::cout << "Length: "; std::cin >> params[0];
        cs = (sample_choice == 3) ? construct_cube(mp, lp, params) : construct_filled_cube(mp, lp, params);
    } else if (sample_choice == 5 || sample_choice == 6) {
        int params[2]; std::cout << "Major radius: "; std::cin >> params[0];
        std::cout << "Minor radius: "; std::cin >> params[1];
        cs = (sample_choice == 5) ? construct_donut(mp, lp, params) : construct_filled_donut(mp, lp, params);
    }
}

// ─── Menu: Transform Object ──────────────────────────────────────────────────

void transform_object(ConstructedSpace* cs) {
    if (!cs) { std::cout << ">> No object to transform.\n"; return; }
    std::cout << ">> Transform Object\n1. Rotate\n2. Go back\nEnter choice: ";
    int choice; std::cin >> choice; if (choice != 1) return;

    int t_params[3], duration;
    std::cout << "Enter X Y Z angles (deg/s): "; std::cin >> t_params[0] >> t_params[1] >> t_params[2];
    std::cout << "Duration (s): "; std::cin >> duration;

    int fps = 12, total_frames = duration * fps;
    float fx = t_params[0] / (float)fps, fy = t_params[1] / (float)fps, fz = t_params[2] / (float)fps;
    int acc[3] = {0, 0, 0};

    for (int frame = 0; frame < total_frames; frame++) {
        acc[0] += fx; acc[1] += fy; acc[2] += fz;
        ConstructedSpace* rotated = rotate_constructed_space(cs, acc);
        render_and_display(rotated);
        std::cout << "Frame " << frame + 1 << "/" << total_frames << "\n";
        delete rotated;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps));
    }
}

// ─── Menu: Save / Load ───────────────────────────────────────────────────────

void list_files_in_directory(const std::string& directory, std::vector<std::string>& files) {
    files.clear();
    DIR* dp = opendir(directory.c_str());
    if (!dp) return;
    struct dirent* entry;
    while ((entry = readdir(dp)) != nullptr) {
        if (entry->d_type == DT_REG) files.push_back(entry->d_name);
    }
    closedir(dp);
}

void save_load_object(ConstructedSpace*& cs) {
    std::cout << ">> 1. Save\n2. Load\n3. Go Back\nEnter choice: ";
    int choice; std::cin >> choice;
    if (choice == 1 && cs) {
        std::string name; std::cout << "Name: "; std::cin >> name;
        save_3d_space(cs, "Object Presets/" + name + ".txt");
    } else if (choice == 2) {
        std::vector<std::string> files; list_files_in_directory("Object Presets", files);
        if (files.empty()) return;
        for (int i = 0; i < files.size(); i++) std::cout << i + 1 << ". " << files[i] << "\n";
        int sel; std::cout << "Load #: "; std::cin >> sel;
        if (sel > 0 && sel <= files.size()) { delete cs; cs = load_3d_space("Object Presets/" + files[sel - 1]); }
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
            case 1: create_object(cs);            break;
            case 2: ray_tracing_menu(cs);         break;
            case 3: transform_object(cs);         break;
            case 4: save_load_object(cs);         break;
            case 5: 
                std::cout << ">> Exiting program. Goodbye!\n";
                delete cs;
                return 0;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }
}