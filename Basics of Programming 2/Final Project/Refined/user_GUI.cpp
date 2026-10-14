#include "animation.hpp"
#include <chrono>
#include <thread>
#include <random>

// ─── Application ─────────────────────────────────────────────────────────────

Application::Application() : currentScene_(nullptr) {}

void Application::run() {
    while (true) {
        renderAndDisplay();

        int choice;
        if (!(std::cin >> choice)) {
            std::cerr << "Invalid input. Please enter a number.\n";
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }

        switch (choice) {
            case 1: menuCreateObject(); break;
            case 2: menuRayTracing();   break;
            case 3: menuTransform();    break;
            case 4: menuSaveLoad();     break;
            case 5:
                std::cout << ">> Exiting program. Goodbye!\n";
                delete currentScene_;
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }
}

// ─── Display helpers ─────────────────────────────────────────────────────────

void Application::displayMenu() const {
    std::cout << "\n=== 3D Space Construction Menu ===\n"
              << "1. Create Generic Object\n"
              << "2. Ray Tracing Simulation\n"
              << "3. Transform Object\n"
              << "4. Save/Load Object\n"
              << "5. Exit\n"
              << "===================================\n"
              << "Enter your choice: ";
}

void Application::renderAndDisplay() const {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    if (currentScene_) {
        std::vector<std::string> screen;
        currentScene_->render2DFace(FRONT, screen);
        std::cout << "\n=== Rendered 2D Front View ===\n";
        currentScene_->printScreen(screen);
    } else {
        std::cout << "\nNo object to display. Create or load an object first.\n";
    }

    displayMenu();
}

// ─── Projector layout helper ─────────────────────────────────────────────────

std::vector<Vector3> Application::buildProjectorDirs(int layout) {
    float inv_sqrt3 = 1.0f / std::sqrt(3.0f);
    float inv_sqrt2 = 1.0f / std::sqrt(2.0f);

    std::vector<Vector3> dirs;
    if (layout == 1) {
        dirs = {{ inv_sqrt3,  inv_sqrt3,  inv_sqrt3},
                {-inv_sqrt3, -inv_sqrt3,  inv_sqrt3},
                {-inv_sqrt3,  inv_sqrt3, -inv_sqrt3},
                { inv_sqrt3, -inv_sqrt3, -inv_sqrt3}};
    } else if (layout == 2) {
        dirs = {{0.0f, 1.0f, 0.0f},
                { inv_sqrt2, -0.5f,  inv_sqrt2},
                {-inv_sqrt2, -0.5f,  inv_sqrt2},
                {-inv_sqrt2, -0.5f, -inv_sqrt2},
                { inv_sqrt2, -0.5f, -inv_sqrt2}};
        for (auto& v : dirs) v.normalize();
    } else if (layout == 3) {
        dirs = {{ 1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
                { 0.0f, 1.0f, 0.0f}, { 0.0f,-1.0f, 0.0f},
                { 0.0f, 0.0f, 1.0f}, { 0.0f, 0.0f,-1.0f}};
    } else {
        dirs = {{ inv_sqrt3,  inv_sqrt3,  inv_sqrt3},
                {-inv_sqrt3,  inv_sqrt3,  inv_sqrt3},
                { inv_sqrt3, -inv_sqrt3,  inv_sqrt3},
                {-inv_sqrt3, -inv_sqrt3,  inv_sqrt3},
                { inv_sqrt3,  inv_sqrt3, -inv_sqrt3},
                {-inv_sqrt3,  inv_sqrt3, -inv_sqrt3},
                { inv_sqrt3, -inv_sqrt3, -inv_sqrt3},
                {-inv_sqrt3, -inv_sqrt3, -inv_sqrt3}};
    }
    return dirs;
}

// ─── Menu: Create Parametric Object ──────────────────────────────────────────

void Application::menuCreateObject() {
    std::cout << "\n>> Create Generic Object\n"
              << "1. Hollow Sphere\n"
              << "2. Filled Sphere\n"
              << "3. Hollow Cube\n"
              << "4. Filled Cube\n"
              << "5. Hollow Donut\n"
              << "6. Filled Donut\n"
              << "7. Go Back\n"
              << "Enter choice: ";

    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear(); std::cin.ignore(1000, '\n'); return;
    }
    if (choice == 7 || choice < 1 || choice > 7) return;

    MediumParameters mp;
    std::cout << "Size (N): ";                    std::cin >> mp.N;
    std::cout << "Medium transparency (0-1): ";   std::cin >> mp.medium_transparency;
    std::cout << "Medium brightness (0-1): ";     std::cin >> mp.medium_brightness;

    LineParameters lp;
    std::cout << "Line brightness (0-1): ";       std::cin >> lp.line_brightness;

    delete currentScene_; currentScene_ = nullptr;

    if (choice == 1 || choice == 2) {
        int diameter; std::cout << "Diameter: "; std::cin >> diameter;
        currentScene_ = (choice == 1)
            ? SceneBuilder::buildSphere(mp, lp, diameter)
            : SceneBuilder::buildFilledSphere(mp, lp, diameter);
    } else if (choice == 3 || choice == 4) {
        int side; std::cout << "Length: "; std::cin >> side;
        currentScene_ = (choice == 3)
            ? SceneBuilder::buildCube(mp, lp, side)
            : SceneBuilder::buildFilledCube(mp, lp, side);
    } else if (choice == 5 || choice == 6) {
        int maj, min_;
        std::cout << "Major radius: "; std::cin >> maj;
        std::cout << "Minor radius: "; std::cin >> min_;
        currentScene_ = (choice == 5)
            ? SceneBuilder::buildDonut(mp, lp, maj, min_)
            : SceneBuilder::buildFilledDonut(mp, lp, maj, min_);
    }
}

// ─── Menu: Ray Tracing ───────────────────────────────────────────────────────

void Application::menuRayTracing() {
    std::cout << "\n>> Ray Tracing Simulation Sub-Menu\n"
              << "1. Static Random Rays (Cloud/Nebula)\n"
              << "2. Animated Ray Traced Sphere\n"
              << "3. Go Back\n"
              << "Enter choice: ";

    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear(); std::cin.ignore(1000, '\n'); return;
    }
    if (choice == 1)      doStaticRayTracing();
    else if (choice == 2) doAnimatedRayTracedSphere();
}

void Application::doStaticRayTracing() {
    std::cout << "\n--- Static Random Rays (Cloud/Nebula) ---\n";

    MediumParameters mp;
    std::cout << "Enter size of the 3D space (e.g. 30): ";
    std::cin >> mp.N;
    std::cout << "Enter mist transparency (0.1 to 0.99): ";
    std::cin >> mp.medium_transparency;
    mp.medium_brightness = 0.0f;

    std::cout << "\nSelect Projector Layout:\n"
              << "1. Tetrahedron (4)  2. Square Pyramid (5)\n"
              << "3. Octahedron (6)   4. Cube (8)\n"
              << "Enter choice: ";
    int layout; std::cin >> layout;

    int ray_count;
    std::cout << "Enter rays per projector (64, 128, 256, 512): ";
    std::cin >> ray_count;

    int enable_secondary;
    std::cout << "Enable secondary lighting? (1=Yes, 0=No): ";
    std::cin >> enable_secondary;

    auto* vs = new VoxelSpace(mp.N);
    vs->initMedium(mp.medium_transparency, 0.0f);

    float center = mp.N / 2.0f;
    float R_proj = 1.5f * (mp.N * std::sqrt(3.0f) / 2.0f);
    auto  dirs   = buildProjectorDirs(layout);

    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_real_distribution<float> target_dist(0.0f, float(mp.N));
    std::uniform_real_distribution<float> intensity_dist(100.0f, 500.0f);

    std::cout << "Shooting rays... ";
    for (const auto& dir : dirs) {
        Point proj_pos = { center + dir.dx * R_proj,
                           center + dir.dy * R_proj,
                           center + dir.dz * R_proj };
        for (int i = 0; i < ray_count; i++) {
            Point target = { target_dist(rng), target_dist(rng), target_dist(rng) };
            Ray ray;
            ray.origin    = proj_pos;
            ray.direction = { target.x - proj_pos.x,
                              target.y - proj_pos.y,
                              target.z - proj_pos.z };
            ray.intensity = intensity_dist(rng);
            RayTracer::shootRay(*vs, ray, mp.medium_transparency);
        }
    }
    std::cout << "Done!\n";

    if (enable_secondary == 1) {
        std::cout << "Calculating secondary light scattering... ";
        RayTracer::applySecondaryLighting(*vs, -1);
        std::cout << "Done!\n";
    }

    delete currentScene_;
    currentScene_ = new Scene(vs, new LineSpace());
}

void Application::doAnimatedRayTracedSphere() {
    std::cout << "\n--- Animated Ray Traced Sphere ---\n";

    int N; std::cout << "Enter size of the 3D space (e.g. 30): "; std::cin >> N;
    float mist; std::cout << "Enter mist transparency (e.g. 0.95): "; std::cin >> mist;
    float diameter; std::cout << "Enter sphere diameter: "; std::cin >> diameter;
    float radius = diameter / 2.0f;

    std::cout << "\nSelect Projector Layout:\n"
              << "1. Tetrahedron (4)  2. Square Pyramid (5)\n"
              << "3. Octahedron (6)   4. Cube (8)\n"
              << "Enter choice: ";
    int layout; std::cin >> layout;

    int ray_count; std::cout << "Enter rays per projector: "; std::cin >> ray_count;

    float dx, dy, dz;
    std::cout << "Enter translation amounts for X Y Z (e.g., 10 0 -5): ";
    std::cin >> dx >> dy >> dz;

    int duration; std::cout << "Enter movement duration (in seconds): "; std::cin >> duration;

    float center_orig = N / 2.0f;
    float R_proj      = 1.5f * (N * std::sqrt(3.0f) / 2.0f);
    auto  dirs        = buildProjectorDirs(layout);

    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_real_distribution<float> dist_u(0.0f, 1.0f);
    std::uniform_real_distribution<float> dist_v(0.0f, 1.0f);
    std::uniform_real_distribution<float> dist_w(0.0f, 1.0f);
    std::uniform_real_distribution<float> intensity_dist(100.0f, 500.0f);

    int   fps          = 12;
    int   total_frames = duration * fps;
    float step_x       = dx / static_cast<float>(total_frames);
    float step_y       = dy / static_cast<float>(total_frames);
    float step_z       = dz / static_cast<float>(total_frames);

    for (int frame = 0; frame <= total_frames; frame++) {
        float cur_cx = center_orig + step_x * frame;
        float cur_cy = center_orig + step_y * frame;
        float cur_cz = center_orig + step_z * frame;

        auto* frame_vs = new VoxelSpace(N);
        frame_vs->initMedium(mist, 0.0f);

        for (const auto& dir : dirs) {
            Point proj_pos = { center_orig + dir.dx * R_proj,
                               center_orig + dir.dy * R_proj,
                               center_orig + dir.dz * R_proj };

            for (int i = 0; i < ray_count; i++) {
                float u = dist_u(rng), v = dist_v(rng), w = dist_w(rng);
                float theta = 2.0f * static_cast<float>(M_PI) * u;
                float phi   = std::acos(1.0f - 2.0f * v);
                float r     = radius * std::cbrt(w);

                Point target = { cur_cx + r*std::sin(phi)*std::cos(theta),
                                 cur_cy + r*std::sin(phi)*std::sin(theta),
                                 cur_cz + r*std::cos(phi) };

                Ray ray;
                ray.origin    = proj_pos;
                ray.direction = { target.x - proj_pos.x,
                                  target.y - proj_pos.y,
                                  target.z - proj_pos.z };
                ray.intensity = intensity_dist(rng);
                RayTracer::shootRay(*frame_vs, ray, mist);
            }
        }

        // Display this frame temporarily (don't persist until last frame).
        Scene* frame_scene = new Scene(frame_vs, new LineSpace());

        // Borrow render from the frame, display inline.
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        std::vector<std::string> screen;
        frame_scene->render2DFace(FRONT, screen);
        std::cout << "\n=== Rendered 2D Front View ===\n";
        frame_scene->printScreen(screen);
        displayMenu();
        std::cout << ">> Frame " << frame << "/" << total_frames
                  << " | Center: (" << cur_cx << ", " << cur_cy << ", " << cur_cz << ")\n";

        if (frame < total_frames) {
            delete frame_scene;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps));
        } else {
            delete currentScene_;
            currentScene_ = frame_scene;
        }
    }

    std::cout << ">> Animation complete.\n";
}

// ─── Menu: Transform ─────────────────────────────────────────────────────────

void Application::menuTransform() {
    if (!currentScene_) { std::cout << ">> No object to transform.\n"; return; }
    std::cout << ">> Transform Object\n1. Rotate\n2. Go back\nEnter choice: ";
    int choice; std::cin >> choice; if (choice != 1) return;

    int degX, degY, degZ, duration;
    std::cout << "Enter X Y Z angles (deg/s): ";
    std::cin >> degX >> degY >> degZ;
    std::cout << "Duration (s): ";
    std::cin >> duration;

    int   fps          = 12;
    int   total_frames = duration * fps;
    float fx = degX / float(fps), fy = degY / float(fps), fz = degZ / float(fps);
    float accX = 0, accY = 0, accZ = 0;

    for (int frame = 0; frame < total_frames; frame++) {
        accX += fx; accY += fy; accZ += fz;
        Scene* rotated = currentScene_->rotate(
            static_cast<int>(accX),
            static_cast<int>(accY),
            static_cast<int>(accZ));

        // Borrow-display without changing currentScene_.
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        std::vector<std::string> screen;
        rotated->render2DFace(FRONT, screen);
        std::cout << "\n=== Rendered 2D Front View ===\n";
        rotated->printScreen(screen);
        displayMenu();
        std::cout << "Frame " << frame + 1 << "/" << total_frames << "\n";

        delete rotated;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps));
    }
}

// ─── Menu: Save / Load ───────────────────────────────────────────────────────

void Application::listFilesInDirectory(const std::string& directory,
                                        std::vector<std::string>& files) {
    files.clear();
    DIR* dp = opendir(directory.c_str());
    if (!dp) return;
    struct dirent* entry;
    while ((entry = readdir(dp)) != nullptr)
        if (entry->d_type == DT_REG) files.push_back(entry->d_name);
    closedir(dp);
}

void Application::menuSaveLoad() {
    std::cout << ">> 1. Save\n2. Load\n3. Go Back\nEnter choice: ";
    int choice; std::cin >> choice;

    if (choice == 1 && currentScene_) {
        std::string name; std::cout << "Name: "; std::cin >> name;
        currentScene_->save("Object Presets/" + name + ".txt");
    } else if (choice == 2) {
        std::vector<std::string> files;
        listFilesInDirectory("Object Presets", files);
        if (files.empty()) { std::cout << "No saved objects found.\n"; return; }
        for (int i = 0; i < (int)files.size(); i++)
            std::cout << i+1 << ". " << files[i] << "\n";
        int sel; std::cout << "Load #: "; std::cin >> sel;
        if (sel > 0 && sel <= (int)files.size()) {
            Scene* loaded = Scene::load("Object Presets/" + files[sel-1]);
            if (loaded) {
                delete currentScene_;
                currentScene_ = loaded;
            }
        }
    }
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main() {
    Application app;
    app.run();
    return 0;
}
