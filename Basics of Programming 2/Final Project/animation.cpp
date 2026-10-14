#include "animation.hpp"

// ─── ConstructedSpace destructor ─────────────────────────────────────────────

ConstructedSpace::~ConstructedSpace() {
    delete space;
    delete lines;
}

// ─── Space Creation & Light Transport ────────────────────────────────────────

Cube* create_empty_3d_space(int N) {
    return new Cube(N);
}

// 3D DDA Ray Marching Algorithm for Volumetric Light
void shoot_ray(Cube* space, Ray ray, float medium_transparency) {
    // 1. Ensure the direction vector is normalized
    ray.direction.normalize();

    // 2. Initial Voxel Coordinates
    int x = static_cast<int>(std::floor(ray.origin.x));
    int y = static_cast<int>(std::floor(ray.origin.y));
    int z = static_cast<int>(std::floor(ray.origin.z));

    // 3. Step Direction (+1 or -1 based on ray vector)
    int stepX = (ray.direction.dx > 0) ? 1 : ((ray.direction.dx < 0) ? -1 : 0);
    int stepY = (ray.direction.dy > 0) ? 1 : ((ray.direction.dy < 0) ? -1 : 0);
    int stepZ = (ray.direction.dz > 0) ? 1 : ((ray.direction.dz < 0) ? -1 : 0);

    // 4. tDelta: Distance along ray to traverse 1 unit in a specific axis
    float tDeltaX = (stepX != 0) ? std::abs(1.0f / ray.direction.dx) : std::numeric_limits<float>::infinity();
    float tDeltaY = (stepY != 0) ? std::abs(1.0f / ray.direction.dy) : std::numeric_limits<float>::infinity();
    float tDeltaZ = (stepZ != 0) ? std::abs(1.0f / ray.direction.dz) : std::numeric_limits<float>::infinity();

    // 5. tMax: Distance from ray origin to the very NEXT voxel boundary
    float tMaxX = (stepX > 0) ? (std::floor(ray.origin.x) + 1.0f - ray.origin.x) * tDeltaX
                              : (stepX < 0) ? (ray.origin.x - std::floor(ray.origin.x)) * tDeltaX
                              : std::numeric_limits<float>::infinity();

    float tMaxY = (stepY > 0) ? (std::floor(ray.origin.y) + 1.0f - ray.origin.y) * tDeltaY
                              : (stepY < 0) ? (ray.origin.y - std::floor(ray.origin.y)) * tDeltaY
                              : std::numeric_limits<float>::infinity();

    float tMaxZ = (stepZ > 0) ? (std::floor(ray.origin.z) + 1.0f - ray.origin.z) * tDeltaZ
                              : (stepZ < 0) ? (ray.origin.z - std::floor(ray.origin.z)) * tDeltaZ
                              : std::numeric_limits<float>::infinity();

    float current_intensity = ray.intensity;
    int N = space->N;
    
    // Track if we have entered the valid grid space yet
    bool has_entered_grid = false;

    // 6. The Marching Loop
    while (current_intensity > 0.01f) { // Stop if light dies out
        
        bool in_bounds = (x >= 0 && x < N && y >= 0 && y < N && z >= 0 && z < N);
        
        if (in_bounds) {
            has_entered_grid = true;
            
            // The voxel absorbs light based on opacity (1 - transparency)
            float absorbed = current_intensity * (1.0f - medium_transparency);
            space->pixels[x][y][z].brightness += absorbed;
            
            // The ray loses that absorbed power
            current_intensity *= medium_transparency;
        } 
        else if (has_entered_grid) {
            // We stepped out of the grid, ray is gone
            break; 
        }

        // 7. Step to the next voxel boundary whichever is closest
        if (tMaxX < tMaxY) {
            if (tMaxX < tMaxZ) {
                x += stepX;
                tMaxX += tDeltaX;
            } else {
                z += stepZ;
                tMaxZ += tDeltaZ;
            }
        } else {
            if (tMaxY < tMaxZ) {
                y += stepY;
                tMaxY += tDeltaY;
            } else {
                z += stepZ;
                tMaxZ += tDeltaZ;
            }
        }
        
        // Safety Break (prevents infinite loops if starting far away)
        if (std::min({tMaxX, tMaxY, tMaxZ}) > N * 3.0f) break;
    }
}

// Bresenham's 3-D line algorithm (Kept for backwards compatibility)
void update_3d_space_along_line(Cube* space, Point start, Point end, float brightness) {
    int x0 = static_cast<int>(std::roundf(start.x));
    int y0 = static_cast<int>(std::roundf(start.y));
    int z0 = static_cast<int>(std::roundf(start.z));
    int x1 = static_cast<int>(std::roundf(end.x));
    int y1 = static_cast<int>(std::roundf(end.y));
    int z1 = static_cast<int>(std::roundf(end.z));

    int dx = std::abs(x1 - x0), dy = std::abs(y1 - y0), dz = std::abs(z1 - z0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int sz = (z0 < z1) ? 1 : -1;
    int N  = space->N;

    auto inBounds = [&](int x, int y, int z) {
        return x >= 0 && x < N && y >= 0 && y < N && z >= 0 && z < N;
    };

    int err1, err2;
    if (dx >= dy && dx >= dz) {
        err1 = 2 * dy - dx;
        err2 = 2 * dz - dx;
        for (; x0 != x1; x0 += sx) {
            if (inBounds(x0, y0, z0))
                space->pixels[x0][y0][z0].brightness += brightness;
            if (err1 > 0) { y0 += sy; err1 -= 2 * dx; }
            if (err2 > 0) { z0 += sz; err2 -= 2 * dx; }
            err1 += 2 * dy;
            err2 += 2 * dz;
        }
    } else if (dy >= dx && dy >= dz) {
        err1 = 2 * dx - dy;
        err2 = 2 * dz - dy;
        for (; y0 != y1; y0 += sy) {
            if (inBounds(x0, y0, z0))
                space->pixels[x0][y0][z0].brightness += brightness;
            if (err1 > 0) { x0 += sx; err1 -= 2 * dy; }
            if (err2 > 0) { z0 += sz; err2 -= 2 * dy; }
            err1 += 2 * dx;
            err2 += 2 * dz;
        }
    } else {
        err1 = 2 * dy - dz;
        err2 = 2 * dx - dz;
        for (; z0 != z1; z0 += sz) {
            if (inBounds(x0, y0, z0))
                space->pixels[x0][y0][z0].brightness += brightness;
            if (err1 > 0) { y0 += sy; err1 -= 2 * dz; }
            if (err2 > 0) { x0 += sx; err2 -= 2 * dz; }
            err1 += 2 * dy;
            err2 += 2 * dx;
        }
    }
}

// ─── Helper: initialise medium voxels ────────────────────────────────────────

static void init_medium(Cube* space, const MediumParameters& mp) {
    int N = space->N;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            for (int k = 0; k < N; k++) {
                space->pixels[i][j][k] = { 0, 0, 0, mp.medium_transparency, mp.medium_brightness };
            }
}

// ─── Object Construction (Parametric) ────────────────────────────────────────

ConstructedSpace* construct_cube(MediumParameters medium_params, LineParameters line_params, int object_params[1]) {
    int cube_size = object_params[0];
    Cube* space = create_empty_3d_space(medium_params.N);
    LineSpace* lines = new LineSpace();

    init_medium(space, medium_params);

    int center_offset = (medium_params.N - cube_size) / 2;

    for (int i = 0; i < cube_size; i++) {
        for (int j = 0; j < cube_size; j++) {
            for (int k = 0; k < cube_size; k++) {
                if (i == 0 || i == cube_size - 1 ||
                    j == 0 || j == cube_size - 1 ||
                    k == 0 || k == cube_size - 1) {

                    Point p1 = { static_cast<float>(i + center_offset),
                                 static_cast<float>(j + center_offset),
                                 static_cast<float>(k + center_offset) };

                    if (i < cube_size - 1) {
                        Point p2 = { static_cast<float>(i + 1 + center_offset),
                                     static_cast<float>(j + center_offset),
                                     static_cast<float>(k + center_offset) };
                        lines->add(p1, p2, line_params.line_brightness);
                        update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
                    }
                    if (j < cube_size - 1) {
                        Point p2 = { static_cast<float>(i + center_offset),
                                     static_cast<float>(j + 1 + center_offset),
                                     static_cast<float>(k + center_offset) };
                        lines->add(p1, p2, line_params.line_brightness);
                        update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
                    }
                    if (k < cube_size - 1) {
                        Point p2 = { static_cast<float>(i + center_offset),
                                     static_cast<float>(j + center_offset),
                                     static_cast<float>(k + 1 + center_offset) };
                        lines->add(p1, p2, line_params.line_brightness);
                        update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
                    }
                }
            }
        }
    }

    return new ConstructedSpace(space, lines, medium_params.N);
}

ConstructedSpace* construct_filled_cube(MediumParameters medium_params, LineParameters line_params, int object_params[1]) {
    int cube_size = object_params[0];
    Cube* space = create_empty_3d_space(medium_params.N);
    LineSpace* lines = new LineSpace();

    init_medium(space, medium_params);

    int center_offset = (medium_params.N - cube_size) / 2;

    // Fill interior
    for (int i = 0; i < cube_size; i++)
        for (int j = 0; j < cube_size; j++)
            for (int k = 0; k < cube_size; k++) {
                auto& px = space->pixels[i + center_offset][j + center_offset][k + center_offset];
                px.brightness    = line_params.line_brightness;
                px.transparency  = medium_params.medium_transparency;
                px.r = 255; px.g = 0; px.b = 0;
            }

    // Edge lines
    for (int i = 0; i < cube_size; i++) {
        for (int j = 0; j < cube_size; j++) {
            for (int k = 0; k < cube_size; k++) {
                if (i == 0 || i == cube_size - 1 ||
                    j == 0 || j == cube_size - 1 ||
                    k == 0 || k == cube_size - 1) {

                    Point p1 = { static_cast<float>(i + center_offset),
                                 static_cast<float>(j + center_offset),
                                 static_cast<float>(k + center_offset) };

                    if (i < cube_size - 1) {
                        Point p2 = { static_cast<float>(i + 1 + center_offset),
                                     static_cast<float>(j + center_offset),
                                     static_cast<float>(k + center_offset) };
                        lines->add(p1, p2, line_params.line_brightness);
                        update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
                    }
                    if (j < cube_size - 1) {
                        Point p2 = { static_cast<float>(i + center_offset),
                                     static_cast<float>(j + 1 + center_offset),
                                     static_cast<float>(k + center_offset) };
                        lines->add(p1, p2, line_params.line_brightness);
                        update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
                    }
                    if (k < cube_size - 1) {
                        Point p2 = { static_cast<float>(i + center_offset),
                                     static_cast<float>(j + center_offset),
                                     static_cast<float>(k + 1 + center_offset) };
                        lines->add(p1, p2, line_params.line_brightness);
                        update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
                    }
                }
            }
        }
    }

    return new ConstructedSpace(space, lines, medium_params.N);
}

ConstructedSpace* construct_sphere(MediumParameters medium_params, LineParameters line_params, int object_params[2]) {
    int N       = medium_params.N;
    int segments = 10 * N;
    int radius  = static_cast<int>(std::roundf(object_params[0] / 2.0f));

    Cube* space = create_empty_3d_space(N);
    LineSpace* lines = new LineSpace();

    init_medium(space, medium_params);

    int center = N / 2;

    for (int i = 0; i <= segments; i++) {
        float theta = static_cast<float>(M_PI) * i / segments;
        for (int j = 0; j < 2 * segments; j++) {
            float phi = 2.0f * static_cast<float>(M_PI) * j / (2 * segments);

            Point p1 = { center + radius * std::sin(theta) * std::cos(phi),
                         center + radius * std::sin(theta) * std::sin(phi),
                         center + radius * std::cos(theta) };

            if (j > 0) {
                float prev_phi = 2.0f * static_cast<float>(M_PI) * (j - 1) / (2 * segments);
                Point p2 = { center + radius * std::sin(theta) * std::cos(prev_phi),
                              center + radius * std::sin(theta) * std::sin(prev_phi),
                              center + radius * std::cos(theta) };
                lines->add(p1, p2, line_params.line_brightness);
                update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
            }
        }
    }

    return new ConstructedSpace(space, lines, N);
}

ConstructedSpace* construct_filled_sphere(MediumParameters medium_params, LineParameters line_params, int object_params[2]) {
    int N       = medium_params.N;
    int segments = 10 * N;
    int radius  = static_cast<int>(std::roundf(object_params[0] / 2.0f));

    Cube* space = create_empty_3d_space(N);
    LineSpace* lines = new LineSpace();

    init_medium(space, medium_params);

    int center = N / 2;

    // Fill interior
    for (int x = 0; x < N; x++)
        for (int y = 0; y < N; y++)
            for (int z = 0; z < N; z++) {
                float dx = x - center, dy = y - center, dz = z - center;
                if (dx*dx + dy*dy + dz*dz <= radius * radius) {
                    auto& px = space->pixels[x][y][z];
                    px.brightness   = line_params.line_brightness;
                    px.transparency = medium_params.medium_transparency;
                    px.r = 255; px.g = 0; px.b = 0;
                }
            }

    // Surface lines
    for (int i = 0; i <= segments; i++) {
        float theta = static_cast<float>(M_PI) * i / segments;
        for (int j = 0; j < 2 * segments; j++) {
            float phi = 2.0f * static_cast<float>(M_PI) * j / (2 * segments);

            Point p1 = { center + radius * std::sin(theta) * std::cos(phi),
                         center + radius * std::sin(theta) * std::sin(phi),
                         center + radius * std::cos(theta) };

            if (j > 0) {
                float prev_phi = 2.0f * static_cast<float>(M_PI) * (j - 1) / (2 * segments);
                Point p2 = { center + radius * std::sin(theta) * std::cos(prev_phi),
                              center + radius * std::sin(theta) * std::sin(prev_phi),
                              center + radius * std::cos(theta) };
                lines->add(p1, p2, line_params.line_brightness);
                update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
            }
        }
    }

    return new ConstructedSpace(space, lines, N);
}

ConstructedSpace* construct_donut(MediumParameters medium_params, LineParameters line_params, int object_params[2]) {
    int N            = medium_params.N;
    int segments     = 10 * N;
    int major_radius = object_params[0];
    int minor_radius = object_params[1];

    Cube* space = create_empty_3d_space(N);
    LineSpace* lines = new LineSpace();

    init_medium(space, medium_params);

    int center = N / 2;

    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * static_cast<float>(M_PI) * i / segments;
        for (int j = 0; j <= segments; j++) {
            float phi = 2.0f * static_cast<float>(M_PI) * j / segments;

            Point p1 = {
                center + (major_radius + minor_radius * std::cos(phi)) * std::cos(theta),
                center + (major_radius + minor_radius * std::cos(phi)) * std::sin(theta),
                center + minor_radius * std::sin(phi)
            };

            if (j > 0) {
                float prev_phi = 2.0f * static_cast<float>(M_PI) * (j - 1) / segments;
                Point p2 = {
                    center + (major_radius + minor_radius * std::cos(prev_phi)) * std::cos(theta),
                    center + (major_radius + minor_radius * std::cos(prev_phi)) * std::sin(theta),
                    center + minor_radius * std::sin(prev_phi)
                };
                lines->add(p1, p2, line_params.line_brightness);
                update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
            }

            if (i > 0) {
                float prev_theta = 2.0f * static_cast<float>(M_PI) * (i - 1) / segments;
                Point p2 = {
                    center + (major_radius + minor_radius * std::cos(phi)) * std::cos(prev_theta),
                    center + (major_radius + minor_radius * std::cos(phi)) * std::sin(prev_theta),
                    center + minor_radius * std::sin(phi)
                };
                lines->add(p1, p2, line_params.line_brightness);
                update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
            }
        }
    }

    return new ConstructedSpace(space, lines, N);
}

ConstructedSpace* construct_filled_donut(MediumParameters medium_params, LineParameters line_params, int object_params[2]) {
    int N            = medium_params.N;
    int segments     = 10 * N;
    int major_radius = object_params[0];
    int minor_radius = object_params[1];

    Cube* space = create_empty_3d_space(N);
    LineSpace* lines = new LineSpace();

    init_medium(space, medium_params);

    int center = N / 2;

    // Fill interior
    for (int x = 0; x < N; x++)
        for (int y = 0; y < N; y++)
            for (int z = 0; z < N; z++) {
                float dx = x - center, dy = y - center, dz = z - center;
                float dist_major = std::sqrt(dx*dx + dy*dy) - major_radius;
                if (dist_major*dist_major + dz*dz <= minor_radius * minor_radius) {
                    auto& px = space->pixels[x][y][z];
                    px.brightness   = line_params.line_brightness;
                    px.transparency = medium_params.medium_transparency;
                    px.r = 255; px.g = 255; px.b = 0;
                }
            }

    // Surface lines
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * static_cast<float>(M_PI) * i / segments;
        for (int j = 0; j <= segments; j++) {
            float phi = 2.0f * static_cast<float>(M_PI) * j / segments;

            Point p1 = {
                center + (major_radius + minor_radius * std::cos(phi)) * std::cos(theta),
                center + (major_radius + minor_radius * std::cos(phi)) * std::sin(theta),
                center + minor_radius * std::sin(phi)
            };

            if (j > 0) {
                float prev_phi = 2.0f * static_cast<float>(M_PI) * (j - 1) / segments;
                Point p2 = {
                    center + (major_radius + minor_radius * std::cos(prev_phi)) * std::cos(theta),
                    center + (major_radius + minor_radius * std::cos(prev_phi)) * std::sin(theta),
                    center + minor_radius * std::sin(prev_phi)
                };
                lines->add(p1, p2, line_params.line_brightness);
                update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
            }

            if (i > 0) {
                float prev_theta = 2.0f * static_cast<float>(M_PI) * (i - 1) / segments;
                Point p2 = {
                    center + (major_radius + minor_radius * std::cos(phi)) * std::cos(prev_theta),
                    center + (major_radius + minor_radius * std::cos(phi)) * std::sin(prev_theta),
                    center + minor_radius * std::sin(phi)
                };
                lines->add(p1, p2, line_params.line_brightness);
                update_3d_space_along_line(space, p1, p2, line_params.line_brightness);
            }
        }
    }

    return new ConstructedSpace(space, lines, N);
}

// ─── Test Hook for Volumetric Engine ─────────────────────────────────────────

ConstructedSpace* construct_test_ray(MediumParameters medium_params) {
    int N = medium_params.N;
    Cube* space = create_empty_3d_space(N);
    LineSpace* lines = new LineSpace(); // We leave lines empty for the ray simulation
    
    init_medium(space, medium_params);

    Ray test_ray;
    
    // Start way outside the box to the left, pointing straight right
    test_ray.origin = { -10.0f, static_cast<float>(N/2), static_cast<float>(N/2) };
    test_ray.direction = { 1.0f, 0.0f, 0.0f };
    test_ray.intensity = 100.0f;

    // Shoot the ray! 
    shoot_ray(space, test_ray, medium_params.medium_transparency);

    return new ConstructedSpace(space, lines, N);
}

// ─── Secondary Lighting (Volumetric Scattering) ──────────────────────────────

void apply_secondary_lighting(Cube* space, int max_depth) {
    int N = space->N;
    
    // 3D grid to store the "active" light energy waiting to scatter
    // We initialize it with the brightness deposited by the primary rays
    std::vector<std::vector<std::vector<float>>> active_light(
        N, std::vector<std::vector<float>>(N, std::vector<float>(N, 0.0f)));
        
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            for (int z = 0; z < N; z++) {
                active_light[x][y][z] = space->pixels[x][y][z].brightness;
            }
        }
    }
    
    // 6 cardinal directions (up, down, left, right, forward, back)
    int dx[] = {1, -1, 0, 0, 0, 0};
    int dy[] = {0, 0, 1, -1, 0, 0};
    int dz[] = {0, 0, 0, 0, 1, -1};
    
    for (int depth = 0; max_depth == -1 || depth < max_depth; depth++) {
        
        std::vector<std::vector<std::vector<float>>> next_light(
            N, std::vector<std::vector<float>>(N, std::vector<float>(N, 0.0f)));
            
        float total_energy_in_wave = 0.0f;
        
        for (int x = 0; x < N; x++) {
            for (int y = 0; y < N; y++) {
                for (int z = 0; z < N; z++) {
                    float energy = active_light[x][y][z];
                    if (energy < 0.01f) continue; // Skip dark voxels to save CPU
                    
                    // Scatter energy equally to 6 neighbors (Energy Conservation)
                    float scatter_amount = energy / 6.0f;
                    
                    for (int i = 0; i < 6; i++) {
                        int nx = x + dx[i];
                        int ny = y + dy[i];
                        int nz = z + dz[i];
                        
                        if (nx >= 0 && nx < N && ny >= 0 && ny < N && nz >= 0 && nz < N) {
                            float transparency = space->pixels[nx][ny][nz].transparency;
                            float opacity = 1.0f - transparency;
                            
                            // 1. The neighbor absorbs some light and glows
                            float absorbed = scatter_amount * opacity;
                            space->pixels[nx][ny][nz].brightness += absorbed;
                            
                            // 2. The rest of the light continues propagating
                            float transmitted = scatter_amount * transparency;
                            next_light[nx][ny][nz] += transmitted;
                            
                            total_energy_in_wave += transmitted;
                        }
                    }
                }
            }
        }
        
        // Swap buffers
        active_light = next_light;
        
        // Early exit: If the wave dies out, stop calculating
        if (total_energy_in_wave < 0.1f) {
            std::cout << "(Stabilized after " << depth + 1 << " passes) ";
            break;
        }
    }
}

// ─── Save / Load ─────────────────────────────────────────────────────────────

ConstructedSpace* load_3d_space(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open file for reading: " << filename << "\n";
        return nullptr;
    }

    int N;
    std::string header;
    file >> header >> N;
    if (header != "CUBE") {
        std::cerr << "Invalid file format: Missing CUBE header\n";
        return nullptr;
    }

    Cube* space = create_empty_3d_space(N);
    LineSpace* lines = new LineSpace();

    int i, j, k, r, g, b;
    float transparency, brightness;
    while (file >> i >> j >> k >> transparency >> brightness >> r >> g >> b) {
        if (i < 0 || i >= N || j < 0 || j >= N || k < 0 || k >= N) break;
        space->pixels[i][j][k] = { r, g, b, transparency, brightness };
    }

    int line_count;
    std::string line_header;
    if (!(file >> line_header >> line_count) || line_header != "LINES") {
        std::cerr << "Invalid file format: Missing LINES header\n";
        delete space; delete lines;
        return nullptr;
    }

    for (int idx = 0; idx < line_count; idx++) {
        Line ln;
        if (!(file >> ln.start.x >> ln.start.y >> ln.start.z
                   >> ln.end.x   >> ln.end.y   >> ln.end.z
                   >> ln.brightness)) {
            std::cerr << "Invalid file format: Line data mismatch\n";
            delete space; delete lines;
            return nullptr;
        }
        lines->lines.push_back(ln);
    }

    return new ConstructedSpace(space, lines, N);
}

void save_3d_space(ConstructedSpace* constructed, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Failed to open file for writing: " << filename << "\n";
        return;
    }

    Cube* space = constructed->space;
    LineSpace* lines = constructed->lines;

    file << "CUBE " << space->N << "\n";
    for (int i = 0; i < space->N; i++)
        for (int j = 0; j < space->N; j++)
            for (int k = 0; k < space->N; k++) {
                const Pixel& p = space->pixels[i][j][k];
                file << i << " " << j << " " << k << " "
                     << p.transparency << " " << p.brightness << " "
                     << p.r << " " << p.g << " " << p.b << "\n";
            }

    file << "LINES " << lines->size() << "\n";
    for (const Line& ln : lines->lines) {
        file << ln.start.x << " " << ln.start.y << " " << ln.start.z << " "
             << ln.end.x   << " " << ln.end.y   << " " << ln.end.z   << " "
             << ln.brightness << "\n";
    }

    std::cout << "3D space saved to " << filename << "\n";
}

// ─── Rendering ───────────────────────────────────────────────────────────────

// ─── REPLACE THIS FUNCTION IN animation.cpp ──────────────────────────────────

void render_2d_face(Cube* space, Face face, std::vector<std::string>& screen) {
    int N = space->N;
    screen.assign(N, std::string(N, ' '));

    std::vector<std::vector<float>> brightness_map(N, std::vector<float>(N, 0.0f));
    float min_b =  FLT_MAX;
    float max_b = -FLT_MAX;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            float total   = 0.0f;
            
            // This multiplier perfectly calculates transparency^distance
            float opacity = 1.0f; 

            for (int k = 0; k < N; k++) {
                Pixel p;

                switch (face) {
                    case FRONT:  p = space->pixels[i][j][k];             break;
                    case LEFT:   p = space->pixels[k][i][j];             break;
                    case RIGHT:  p = space->pixels[N - 1 - k][i][j];     break;
                    case BACK:   p = space->pixels[i][j][N - 1 - k];     break;
                    case TOP:    p = space->pixels[i][k][j];             break;
                    case BOTTOM: p = space->pixels[i][N - 1 - k][j];     break;
                    default:     continue;
                }

                // Removed inv_sq falloff. 
                // Only exponential mist transparency is applied!
                total  += p.brightness * opacity;
                opacity *= p.transparency;

                if (opacity <= 0.01f) break;
            }

            if (total < min_b) min_b = total;
            if (total > max_b) max_b = total;
            brightness_map[i][j] = total;
        }
    }

    const std::string ascii_map = " .:-=+*%#@";
    int map_len = static_cast<int>(ascii_map.size());

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            float norm = (max_b - min_b > 0.0f)
                         ? (brightness_map[i][j] - min_b) / (max_b - min_b)
                         : 0.0f;
            int idx = static_cast<int>(norm * (map_len - 1));
            screen[i][j] = ascii_map[idx];
        }
}

void print_2d_screen(const std::vector<std::string>& screen) {
    for (const auto& row : screen) {
        for (char c : row)
            std::cout << c << ' ';
        std::cout << '\n';
    }
}

void save_2d_screen(const std::vector<std::string>& screen, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not open file " << filename << " for writing\n";
        return;
    }
    for (const auto& row : screen)
        file << row << "\n";

    std::cout << "2D screen saved to " << filename << "\n";
}

// ─── Transformations ─────────────────────────────────────────────────────────

void rotate_point(Point& p, float theta_x, float theta_y, float theta_z) {
    // X-axis
    float ny = p.y * std::cos(theta_x) - p.z * std::sin(theta_x);
    float nz = p.y * std::sin(theta_x) + p.z * std::cos(theta_x);
    p.y = ny; p.z = nz;

    // Y-axis
    float nx = p.x * std::cos(theta_y) + p.z * std::sin(theta_y);
    nz = -p.x * std::sin(theta_y) + p.z * std::cos(theta_y);
    p.x = nx; p.z = nz;

    // Z-axis
    nx = p.x * std::cos(theta_z) - p.y * std::sin(theta_z);
    ny = p.x * std::sin(theta_z) + p.y * std::cos(theta_z);
    p.x = nx; p.y = ny;
}

ConstructedSpace* rotate_constructed_space(ConstructedSpace* input_space, int transformation_params[3]) {
    int   N       = input_space->N;
    
    // We negate the angles to perform backward/inverse mapping
    float theta_x = -transformation_params[0] * static_cast<float>(M_PI) / 180.0f;
    float theta_y = -transformation_params[1] * static_cast<float>(M_PI) / 180.0f;
    float theta_z = -transformation_params[2] * static_cast<float>(M_PI) / 180.0f;

    Cube* new_space = create_empty_3d_space(N);
    int   center    = N / 2;

    for (int ni = 0; ni < N; ni++) {
        for (int nj = 0; nj < N; nj++) {
            for (int nk = 0; nk < N; nk++) {
                
                // 1. Center the destination coordinate
                Point p = { static_cast<float>(ni - center),
                             static_cast<float>(nj - center),
                             static_cast<float>(nk - center) };
                             
                // 2. Rotate backward to find where it came from
                rotate_point(p, theta_x, theta_y, theta_z);

                // 3. Round to the nearest source voxel
                int i = static_cast<int>(std::roundf(p.x)) + center;
                int j = static_cast<int>(std::roundf(p.y)) + center;
                int k = static_cast<int>(std::roundf(p.z)) + center;

                // 4. Sample if within bounds
                if (i >= 0 && i < N && j >= 0 && j < N && k >= 0 && k < N) {
                    new_space->pixels[ni][nj][nk] = input_space->space->pixels[i][j][k];
                }
            }
        }
    }

    return new ConstructedSpace(new_space, nullptr, N);
}