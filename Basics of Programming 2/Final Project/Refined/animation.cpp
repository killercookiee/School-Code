#include "animation.hpp"

// ─── VoxelSpace ──────────────────────────────────────────────────────────────

VoxelSpace::VoxelSpace(int n)
    : N_(n),
      pixels_(n, std::vector<std::vector<Pixel>>(n, std::vector<Pixel>(n)))
{}

void VoxelSpace::initMedium(float transparency, float brightness) {
    for (int i = 0; i < N_; i++)
        for (int j = 0; j < N_; j++)
            for (int k = 0; k < N_; k++)
                pixels_[i][j][k] = { 0, 0, 0, transparency, brightness };
}

// ─── LineSpace ────────────────────────────────────────────────────────────────

void LineSpace::add(Point start, Point end, float brightness) {
    lines_.push_back({ start, end, brightness });
}

// ─── Scene ───────────────────────────────────────────────────────────────────

Scene::Scene(int n)
    : space_(std::make_unique<VoxelSpace>(n)),
      lines_(std::make_unique<LineSpace>())
{}

Scene::Scene(VoxelSpace* vs, LineSpace* ls)
    : space_(vs), lines_(ls)
{}

// Rendering ───────────────────────────────────────────────────────────────────

void Scene::render2DFace(Face face, std::vector<std::string>& screen) const {
    int N = space_->size();
    screen.assign(N, std::string(N, ' '));

    std::vector<std::vector<float>> brightness_map(N, std::vector<float>(N, 0.0f));
    float min_b =  FLT_MAX;
    float max_b = -FLT_MAX;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            float total   = 0.0f;
            float opacity = 1.0f;

            for (int k = 0; k < N; k++) {
                Pixel p;
                switch (face) {
                    case FRONT:  p = space_->at(i, j, k);           break;
                    case LEFT:   p = space_->at(k, i, j);           break;
                    case RIGHT:  p = space_->at(N-1-k, i, j);       break;
                    case BACK:   p = space_->at(i, j, N-1-k);       break;
                    case TOP:    p = space_->at(i, k, j);           break;
                    case BOTTOM: p = space_->at(i, N-1-k, j);       break;
                    default:     continue;
                }
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

void Scene::printScreen(const std::vector<std::string>& screen) const {
    for (const auto& row : screen) {
        for (char c : row) std::cout << c << ' ';
        std::cout << '\n';
    }
}

void Scene::saveScreen(const std::vector<std::string>& screen,
                       const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not open file " << filename << " for writing\n";
        return;
    }
    for (const auto& row : screen) file << row << "\n";
    std::cout << "2D screen saved to " << filename << "\n";
}

// Persistence ─────────────────────────────────────────────────────────────────

void Scene::save(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Failed to open file for writing: " << filename << "\n";
        return;
    }
    int N = space_->size();
    file << "CUBE " << N << "\n";
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            for (int k = 0; k < N; k++) {
                const Pixel& p = space_->at(i, j, k);
                file << i << " " << j << " " << k << " "
                     << p.transparency << " " << p.brightness << " "
                     << p.r << " " << p.g << " " << p.b << "\n";
            }

    file << "LINES " << lines_->size() << "\n";
    for (const Line& ln : lines_->lines()) {
        file << ln.start.x << " " << ln.start.y << " " << ln.start.z << " "
             << ln.end.x   << " " << ln.end.y   << " " << ln.end.z   << " "
             << ln.brightness << "\n";
    }
    std::cout << "3D space saved to " << filename << "\n";
}

Scene* Scene::load(const std::string& filename) {
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

    auto* vs = new VoxelSpace(N);
    auto* ls = new LineSpace();

    int i, j, k, r, g, b;
    float transparency, brightness;
    while (file >> i >> j >> k >> transparency >> brightness >> r >> g >> b) {
        if (i < 0 || i >= N || j < 0 || j >= N || k < 0 || k >= N) break;
        vs->at(i, j, k) = { r, g, b, transparency, brightness };
    }

    int line_count;
    std::string line_header;
    if (!(file >> line_header >> line_count) || line_header != "LINES") {
        std::cerr << "Invalid file format: Missing LINES header\n";
        delete vs; delete ls;
        return nullptr;
    }

    for (int idx = 0; idx < line_count; idx++) {
        Line ln;
        if (!(file >> ln.start.x >> ln.start.y >> ln.start.z
                   >> ln.end.x   >> ln.end.y   >> ln.end.z
                   >> ln.brightness)) {
            std::cerr << "Invalid file format: Line data mismatch\n";
            delete vs; delete ls;
            return nullptr;
        }
        ls->lines().push_back(ln);
    }

    return new Scene(vs, ls);
}

// Transformation ──────────────────────────────────────────────────────────────

Scene* Scene::rotate(int degX, int degY, int degZ) const {
    int   N       = space_->size();
    float theta_x = -degX * static_cast<float>(M_PI) / 180.0f;
    float theta_y = -degY * static_cast<float>(M_PI) / 180.0f;
    float theta_z = -degZ * static_cast<float>(M_PI) / 180.0f;

    auto* new_vs  = new VoxelSpace(N);
    int   center  = N / 2;

    for (int ni = 0; ni < N; ni++) {
        for (int nj = 0; nj < N; nj++) {
            for (int nk = 0; nk < N; nk++) {
                Point p = { static_cast<float>(ni - center),
                             static_cast<float>(nj - center),
                             static_cast<float>(nk - center) };
                rotatePoint(p, theta_x, theta_y, theta_z);

                int i = static_cast<int>(std::roundf(p.x)) + center;
                int j = static_cast<int>(std::roundf(p.y)) + center;
                int k = static_cast<int>(std::roundf(p.z)) + center;

                if (i >= 0 && i < N && j >= 0 && j < N && k >= 0 && k < N)
                    new_vs->at(ni, nj, nk) = space_->at(i, j, k);
            }
        }
    }

    return new Scene(new_vs, new LineSpace());
}

// ─── RayTracer ───────────────────────────────────────────────────────────────

void RayTracer::shootRay(VoxelSpace& space, Ray ray, float mediumTransparency) {
    ray.direction.normalize();

    int x = static_cast<int>(std::floor(ray.origin.x));
    int y = static_cast<int>(std::floor(ray.origin.y));
    int z = static_cast<int>(std::floor(ray.origin.z));

    int stepX = (ray.direction.dx > 0) ? 1 : ((ray.direction.dx < 0) ? -1 : 0);
    int stepY = (ray.direction.dy > 0) ? 1 : ((ray.direction.dy < 0) ? -1 : 0);
    int stepZ = (ray.direction.dz > 0) ? 1 : ((ray.direction.dz < 0) ? -1 : 0);

    float tDeltaX = (stepX != 0) ? std::abs(1.0f / ray.direction.dx) : std::numeric_limits<float>::infinity();
    float tDeltaY = (stepY != 0) ? std::abs(1.0f / ray.direction.dy) : std::numeric_limits<float>::infinity();
    float tDeltaZ = (stepZ != 0) ? std::abs(1.0f / ray.direction.dz) : std::numeric_limits<float>::infinity();

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
    int   N                 = space.size();
    bool  has_entered_grid  = false;

    while (current_intensity > 0.01f) {
        bool in_bounds = (x >= 0 && x < N && y >= 0 && y < N && z >= 0 && z < N);

        if (in_bounds) {
            has_entered_grid = true;
            float absorbed = current_intensity * (1.0f - mediumTransparency);
            space.at(x, y, z).brightness += absorbed;
            current_intensity *= mediumTransparency;
        } else if (has_entered_grid) {
            break;
        }

        if (tMaxX < tMaxY) {
            if (tMaxX < tMaxZ) { x += stepX; tMaxX += tDeltaX; }
            else               { z += stepZ; tMaxZ += tDeltaZ; }
        } else {
            if (tMaxY < tMaxZ) { y += stepY; tMaxY += tDeltaY; }
            else               { z += stepZ; tMaxZ += tDeltaZ; }
        }

        if (std::min({tMaxX, tMaxY, tMaxZ}) > N * 3.0f) break;
    }
}

void RayTracer::applySecondaryLighting(VoxelSpace& space, int maxDepth) {
    int N = space.size();

    std::vector<std::vector<std::vector<float>>> active_light(
        N, std::vector<std::vector<float>>(N, std::vector<float>(N, 0.0f)));

    for (int x = 0; x < N; x++)
        for (int y = 0; y < N; y++)
            for (int z = 0; z < N; z++)
                active_light[x][y][z] = space.at(x, y, z).brightness;

    int dx[] = {1, -1, 0, 0, 0, 0};
    int dy[] = {0, 0, 1, -1, 0, 0};
    int dz[] = {0, 0, 0, 0, 1, -1};

    for (int depth = 0; maxDepth == -1 || depth < maxDepth; depth++) {
        std::vector<std::vector<std::vector<float>>> next_light(
            N, std::vector<std::vector<float>>(N, std::vector<float>(N, 0.0f)));
        float total_energy = 0.0f;

        for (int x = 0; x < N; x++) {
            for (int y = 0; y < N; y++) {
                for (int z = 0; z < N; z++) {
                    float energy = active_light[x][y][z];
                    if (energy < 0.01f) continue;
                    float scatter = energy / 6.0f;

                    for (int i = 0; i < 6; i++) {
                        int nx = x + dx[i], ny = y + dy[i], nz = z + dz[i];
                        if (nx >= 0 && nx < N && ny >= 0 && ny < N && nz >= 0 && nz < N) {
                            float trans   = space.at(nx, ny, nz).transparency;
                            float opacity = 1.0f - trans;
                            space.at(nx, ny, nz).brightness += scatter * opacity;
                            float transmitted = scatter * trans;
                            next_light[nx][ny][nz] += transmitted;
                            total_energy += transmitted;
                        }
                    }
                }
            }
        }

        active_light = next_light;
        if (total_energy < 0.1f) {
            std::cout << "(Stabilized after " << depth + 1 << " passes) ";
            break;
        }
    }
}

// ─── SceneBuilder (private helpers) ──────────────────────────────────────────

void SceneBuilder::initMedium(VoxelSpace& space, const MediumParameters& mp) {
    space.initMedium(mp.medium_transparency, mp.medium_brightness);
}

void SceneBuilder::drawLineInSpace(VoxelSpace& space, LineSpace& ls,
                                   Point p1, Point p2, float brightness) {
    ls.add(p1, p2, brightness);
    paintLineIntoSpace(space, p1, p2, brightness);
}

// ─── SceneBuilder (public factories) ─────────────────────────────────────────

Scene* SceneBuilder::buildCube(MediumParameters mp, LineParameters lp, int cube_size) {
    auto* vs = new VoxelSpace(mp.N);
    auto* ls = new LineSpace();
    initMedium(*vs, mp);

    int co = (mp.N - cube_size) / 2;   // center_offset

    for (int i = 0; i < cube_size; i++) {
        for (int j = 0; j < cube_size; j++) {
            for (int k = 0; k < cube_size; k++) {
                if (i == 0 || i == cube_size-1 ||
                    j == 0 || j == cube_size-1 ||
                    k == 0 || k == cube_size-1) {

                    Point p1 = { float(i+co), float(j+co), float(k+co) };
                    if (i < cube_size-1)
                        drawLineInSpace(*vs, *ls, p1, {float(i+1+co), float(j+co), float(k+co)}, lp.line_brightness);
                    if (j < cube_size-1)
                        drawLineInSpace(*vs, *ls, p1, {float(i+co), float(j+1+co), float(k+co)}, lp.line_brightness);
                    if (k < cube_size-1)
                        drawLineInSpace(*vs, *ls, p1, {float(i+co), float(j+co), float(k+1+co)}, lp.line_brightness);
                }
            }
        }
    }
    return new Scene(vs, ls);
}

Scene* SceneBuilder::buildFilledCube(MediumParameters mp, LineParameters lp, int cube_size) {
    auto* vs = new VoxelSpace(mp.N);
    auto* ls = new LineSpace();
    initMedium(*vs, mp);

    int co = (mp.N - cube_size) / 2;

    for (int i = 0; i < cube_size; i++)
        for (int j = 0; j < cube_size; j++)
            for (int k = 0; k < cube_size; k++) {
                auto& px = vs->at(i+co, j+co, k+co);
                px.brightness   = lp.line_brightness;
                px.transparency = mp.medium_transparency;
                px.r = 255; px.g = 0; px.b = 0;
            }

    for (int i = 0; i < cube_size; i++)
        for (int j = 0; j < cube_size; j++)
            for (int k = 0; k < cube_size; k++) {
                if (i == 0 || i == cube_size-1 ||
                    j == 0 || j == cube_size-1 ||
                    k == 0 || k == cube_size-1) {
                    Point p1 = { float(i+co), float(j+co), float(k+co) };
                    if (i < cube_size-1)
                        drawLineInSpace(*vs, *ls, p1, {float(i+1+co), float(j+co), float(k+co)}, lp.line_brightness);
                    if (j < cube_size-1)
                        drawLineInSpace(*vs, *ls, p1, {float(i+co), float(j+1+co), float(k+co)}, lp.line_brightness);
                    if (k < cube_size-1)
                        drawLineInSpace(*vs, *ls, p1, {float(i+co), float(j+co), float(k+1+co)}, lp.line_brightness);
                }
            }
    return new Scene(vs, ls);
}

Scene* SceneBuilder::buildSphere(MediumParameters mp, LineParameters lp, int diameter) {
    int N        = mp.N;
    int segments = 10 * N;
    int radius   = static_cast<int>(std::roundf(diameter / 2.0f));

    auto* vs = new VoxelSpace(N);
    auto* ls = new LineSpace();
    initMedium(*vs, mp);

    int center = N / 2;
    for (int i = 0; i <= segments; i++) {
        float theta = static_cast<float>(M_PI) * i / segments;
        for (int j = 0; j < 2*segments; j++) {
            float phi = 2.0f * static_cast<float>(M_PI) * j / (2*segments);
            Point p1 = { center + radius*std::sin(theta)*std::cos(phi),
                         center + radius*std::sin(theta)*std::sin(phi),
                         center + radius*std::cos(theta) };
            if (j > 0) {
                float pphi = 2.0f * static_cast<float>(M_PI) * (j-1) / (2*segments);
                Point p2 = { center + radius*std::sin(theta)*std::cos(pphi),
                              center + radius*std::sin(theta)*std::sin(pphi),
                              center + radius*std::cos(theta) };
                drawLineInSpace(*vs, *ls, p1, p2, lp.line_brightness);
            }
        }
    }
    return new Scene(vs, ls);
}

Scene* SceneBuilder::buildFilledSphere(MediumParameters mp, LineParameters lp, int diameter) {
    int N        = mp.N;
    int segments = 10 * N;
    int radius   = static_cast<int>(std::roundf(diameter / 2.0f));

    auto* vs = new VoxelSpace(N);
    auto* ls = new LineSpace();
    initMedium(*vs, mp);

    int center = N / 2;

    for (int x = 0; x < N; x++)
        for (int y = 0; y < N; y++)
            for (int z = 0; z < N; z++) {
                float ddx = x - center, ddy = y - center, ddz = z - center;
                if (ddx*ddx + ddy*ddy + ddz*ddz <= radius*radius) {
                    auto& px = vs->at(x, y, z);
                    px.brightness   = lp.line_brightness;
                    px.transparency = mp.medium_transparency;
                    px.r = 255; px.g = 0; px.b = 0;
                }
            }

    for (int i = 0; i <= segments; i++) {
        float theta = static_cast<float>(M_PI) * i / segments;
        for (int j = 0; j < 2*segments; j++) {
            float phi = 2.0f * static_cast<float>(M_PI) * j / (2*segments);
            Point p1 = { center + radius*std::sin(theta)*std::cos(phi),
                         center + radius*std::sin(theta)*std::sin(phi),
                         center + radius*std::cos(theta) };
            if (j > 0) {
                float pphi = 2.0f * static_cast<float>(M_PI) * (j-1) / (2*segments);
                Point p2 = { center + radius*std::sin(theta)*std::cos(pphi),
                              center + radius*std::sin(theta)*std::sin(pphi),
                              center + radius*std::cos(theta) };
                drawLineInSpace(*vs, *ls, p1, p2, lp.line_brightness);
            }
        }
    }
    return new Scene(vs, ls);
}

Scene* SceneBuilder::buildDonut(MediumParameters mp, LineParameters lp,
                                int majorRadius, int minorRadius) {
    int N        = mp.N;
    int segments = 10 * N;
    int center   = N / 2;

    auto* vs = new VoxelSpace(N);
    auto* ls = new LineSpace();
    initMedium(*vs, mp);

    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * static_cast<float>(M_PI) * i / segments;
        for (int j = 0; j <= segments; j++) {
            float phi = 2.0f * static_cast<float>(M_PI) * j / segments;
            Point p1 = {
                center + (majorRadius + minorRadius*std::cos(phi)) * std::cos(theta),
                center + (majorRadius + minorRadius*std::cos(phi)) * std::sin(theta),
                center + minorRadius * std::sin(phi)
            };
            if (j > 0) {
                float pphi = 2.0f * static_cast<float>(M_PI) * (j-1) / segments;
                Point p2 = {
                    center + (majorRadius + minorRadius*std::cos(pphi)) * std::cos(theta),
                    center + (majorRadius + minorRadius*std::cos(pphi)) * std::sin(theta),
                    center + minorRadius * std::sin(pphi)
                };
                drawLineInSpace(*vs, *ls, p1, p2, lp.line_brightness);
            }
            if (i > 0) {
                float ptheta = 2.0f * static_cast<float>(M_PI) * (i-1) / segments;
                Point p2 = {
                    center + (majorRadius + minorRadius*std::cos(phi)) * std::cos(ptheta),
                    center + (majorRadius + minorRadius*std::cos(phi)) * std::sin(ptheta),
                    center + minorRadius * std::sin(phi)
                };
                drawLineInSpace(*vs, *ls, p1, p2, lp.line_brightness);
            }
        }
    }
    return new Scene(vs, ls);
}

Scene* SceneBuilder::buildFilledDonut(MediumParameters mp, LineParameters lp,
                                      int majorRadius, int minorRadius) {
    int N        = mp.N;
    int segments = 10 * N;
    int center   = N / 2;

    auto* vs = new VoxelSpace(N);
    auto* ls = new LineSpace();
    initMedium(*vs, mp);

    for (int x = 0; x < N; x++)
        for (int y = 0; y < N; y++)
            for (int z = 0; z < N; z++) {
                float ddx = x - center, ddy = y - center, ddz = z - center;
                float dist_major = std::sqrt(ddx*ddx + ddy*ddy) - majorRadius;
                if (dist_major*dist_major + ddz*ddz <= minorRadius*minorRadius) {
                    auto& px = vs->at(x, y, z);
                    px.brightness   = lp.line_brightness;
                    px.transparency = mp.medium_transparency;
                    px.r = 255; px.g = 255; px.b = 0;
                }
            }

    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * static_cast<float>(M_PI) * i / segments;
        for (int j = 0; j <= segments; j++) {
            float phi = 2.0f * static_cast<float>(M_PI) * j / segments;
            Point p1 = {
                center + (majorRadius + minorRadius*std::cos(phi)) * std::cos(theta),
                center + (majorRadius + minorRadius*std::cos(phi)) * std::sin(theta),
                center + minorRadius * std::sin(phi)
            };
            if (j > 0) {
                float pphi = 2.0f * static_cast<float>(M_PI) * (j-1) / segments;
                Point p2 = {
                    center + (majorRadius + minorRadius*std::cos(pphi)) * std::cos(theta),
                    center + (majorRadius + minorRadius*std::cos(pphi)) * std::sin(theta),
                    center + minorRadius * std::sin(pphi)
                };
                drawLineInSpace(*vs, *ls, p1, p2, lp.line_brightness);
            }
            if (i > 0) {
                float ptheta = 2.0f * static_cast<float>(M_PI) * (i-1) / segments;
                Point p2 = {
                    center + (majorRadius + minorRadius*std::cos(phi)) * std::cos(ptheta),
                    center + (majorRadius + minorRadius*std::cos(phi)) * std::sin(ptheta),
                    center + minorRadius * std::sin(phi)
                };
                drawLineInSpace(*vs, *ls, p1, p2, lp.line_brightness);
            }
        }
    }
    return new Scene(vs, ls);
}

Scene* SceneBuilder::buildTestRay(MediumParameters mp) {
    auto* vs = new VoxelSpace(mp.N);
    auto* ls = new LineSpace();
    initMedium(*vs, mp);

    Ray test_ray;
    test_ray.origin    = { -10.0f, static_cast<float>(mp.N/2), static_cast<float>(mp.N/2) };
    test_ray.direction = { 1.0f, 0.0f, 0.0f };
    test_ray.intensity = 100.0f;

    RayTracer::shootRay(*vs, test_ray, mp.medium_transparency);
    return new Scene(vs, ls);
}

// ─── Free helpers ─────────────────────────────────────────────────────────────

// Bresenham's 3-D line — paints brightness into a VoxelSpace.
void paintLineIntoSpace(VoxelSpace& space, Point start, Point end, float brightness) {
    int x0 = static_cast<int>(std::roundf(start.x));
    int y0 = static_cast<int>(std::roundf(start.y));
    int z0 = static_cast<int>(std::roundf(start.z));
    int x1 = static_cast<int>(std::roundf(end.x));
    int y1 = static_cast<int>(std::roundf(end.y));
    int z1 = static_cast<int>(std::roundf(end.z));

    int N  = space.size();
    int ddx = std::abs(x1-x0), ddy = std::abs(y1-y0), ddz = std::abs(z1-z0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int sz = (z0 < z1) ? 1 : -1;

    auto inBounds = [&](int x, int y, int z) {
        return x >= 0 && x < N && y >= 0 && y < N && z >= 0 && z < N;
    };

    int err1, err2;
    if (ddx >= ddy && ddx >= ddz) {
        err1 = 2*ddy - ddx; err2 = 2*ddz - ddx;
        for (; x0 != x1; x0 += sx) {
            if (inBounds(x0, y0, z0)) space.at(x0, y0, z0).brightness += brightness;
            if (err1 > 0) { y0 += sy; err1 -= 2*ddx; }
            if (err2 > 0) { z0 += sz; err2 -= 2*ddx; }
            err1 += 2*ddy; err2 += 2*ddz;
        }
    } else if (ddy >= ddx && ddy >= ddz) {
        err1 = 2*ddx - ddy; err2 = 2*ddz - ddy;
        for (; y0 != y1; y0 += sy) {
            if (inBounds(x0, y0, z0)) space.at(x0, y0, z0).brightness += brightness;
            if (err1 > 0) { x0 += sx; err1 -= 2*ddy; }
            if (err2 > 0) { z0 += sz; err2 -= 2*ddy; }
            err1 += 2*ddx; err2 += 2*ddz;
        }
    } else {
        err1 = 2*ddy - ddz; err2 = 2*ddx - ddz;
        for (; z0 != z1; z0 += sz) {
            if (inBounds(x0, y0, z0)) space.at(x0, y0, z0).brightness += brightness;
            if (err1 > 0) { y0 += sy; err1 -= 2*ddz; }
            if (err2 > 0) { x0 += sx; err2 -= 2*ddz; }
            err1 += 2*ddy; err2 += 2*ddx;
        }
    }
}

void rotatePoint(Point& p, float theta_x, float theta_y, float theta_z) {
    float ny = p.y * std::cos(theta_x) - p.z * std::sin(theta_x);
    float nz = p.y * std::sin(theta_x) + p.z * std::cos(theta_x);
    p.y = ny; p.z = nz;

    float nx = p.x * std::cos(theta_y) + p.z * std::sin(theta_y);
    nz = -p.x * std::sin(theta_y) + p.z * std::cos(theta_y);
    p.x = nx; p.z = nz;

    nx = p.x * std::cos(theta_z) - p.y * std::sin(theta_z);
    ny = p.x * std::sin(theta_z) + p.y * std::cos(theta_z);
    p.x = nx; p.y = ny;
}
