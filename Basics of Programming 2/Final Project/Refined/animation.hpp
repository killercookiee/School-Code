#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <cfloat>
#include <ctime>
#include <limits>
#include <algorithm>
#include <dirent.h>
#include <stdexcept>
#include <memory>

// ─── Primitive Data Types ─────────────────────────────────────────────────────

struct Pixel {
    int   r = 0, g = 0, b = 0;
    float transparency = 1.0f;
    float brightness   = 0.0f;
};

enum Face { FRONT, LEFT, RIGHT, BACK, TOP, BOTTOM };

struct Point { float x, y, z; };

struct Vector3 {
    float dx, dy, dz;
    void normalize() {
        float len = std::sqrt(dx*dx + dy*dy + dz*dz);
        if (len > 0) { dx /= len; dy /= len; dz /= len; }
    }
};

struct Ray {
    Point   origin;
    Vector3 direction;
    float   intensity;
};

struct Line {
    Point start, end;
    float brightness;
};

// ─── Parameters (plain data, no behaviour) ───────────────────────────────────

struct MediumParameters {
    int   N;
    float medium_transparency;
    float medium_brightness;
};

struct LineParameters {
    float line_brightness;
};

// ─── VoxelSpace ──────────────────────────────────────────────────────────────
// Owns and manages a 3-D grid of Pixels.

class VoxelSpace {
public:
    explicit VoxelSpace(int n);

    int    size() const { return N_; }
    Pixel& at(int x, int y, int z)       { return pixels_[x][y][z]; }
    const Pixel& at(int x, int y, int z) const { return pixels_[x][y][z]; }

    // Fill every voxel with the given medium properties.
    void initMedium(float transparency, float brightness);

private:
    int N_;
    std::vector<std::vector<std::vector<Pixel>>> pixels_;
};

// ─── LineSpace ────────────────────────────────────────────────────────────────
// Stores the skeletal line segments used for wireframe objects.

class LineSpace {
public:
    void  add(Point start, Point end, float brightness);
    int   size() const { return static_cast<int>(lines_.size()); }
    const std::vector<Line>& lines() const { return lines_; }
          std::vector<Line>& lines()       { return lines_; }

private:
    std::vector<Line> lines_;
};

// ─── Scene ───────────────────────────────────────────────────────────────────
// Owns one VoxelSpace and one LineSpace; the central working object.

class Scene {
public:
    Scene(int n);                          // blank scene
    Scene(VoxelSpace* vs, LineSpace* ls);  // takes ownership of raw ptrs

    int          gridSize()  const { return space_->size(); }
    VoxelSpace&  voxels()          { return *space_; }
    const VoxelSpace& voxels() const { return *space_; }
    LineSpace&   lines()           { return *lines_; }
    const LineSpace& lines() const { return *lines_; }

    // Rendering
    void render2DFace(Face face, std::vector<std::string>& screen) const;
    void printScreen(const std::vector<std::string>& screen) const;
    void saveScreen(const std::vector<std::string>& screen,
                    const std::string& filename) const;

    // Persistence
    void save(const std::string& filename) const;
    static Scene* load(const std::string& filename); // caller owns result

    // Transformation — returns a new Scene (caller owns it)
    Scene* rotate(int degX, int degY, int degZ) const;

private:
    std::unique_ptr<VoxelSpace> space_;
    std::unique_ptr<LineSpace>  lines_;
};

// ─── RayTracer ───────────────────────────────────────────────────────────────
// Stateless helper that shoots rays into a VoxelSpace.

class RayTracer {
public:
    // 3-D DDA march; deposits light into voxels.
    static void shootRay(VoxelSpace& space, Ray ray, float mediumTransparency);

    // Multi-bounce volumetric scattering (max_depth == -1 → run until decay).
    static void applySecondaryLighting(VoxelSpace& space, int maxDepth);
};

// ─── SceneBuilder ────────────────────────────────────────────────────────────
// Factory: constructs parametric objects and returns an owning Scene*.

class SceneBuilder {
public:
    static Scene* buildCube(MediumParameters mp, LineParameters lp, int sideLen);
    static Scene* buildFilledCube(MediumParameters mp, LineParameters lp, int sideLen);
    static Scene* buildSphere(MediumParameters mp, LineParameters lp, int diameter);
    static Scene* buildFilledSphere(MediumParameters mp, LineParameters lp, int diameter);
    static Scene* buildDonut(MediumParameters mp, LineParameters lp,
                             int majorRadius, int minorRadius);
    static Scene* buildFilledDonut(MediumParameters mp, LineParameters lp,
                                   int majorRadius, int minorRadius);
    static Scene* buildTestRay(MediumParameters mp);

private:
    static void initMedium(VoxelSpace& space, const MediumParameters& mp);
    static void drawLineInSpace(VoxelSpace& space, LineSpace& ls,
                                Point p1, Point p2, float brightness);
};

// ─── Application ─────────────────────────────────────────────────────────────
// Owns the current scene and drives the interactive menu loop.

class Application {
public:
    Application();
    void run();   // main loop

private:
    // Menu handlers
    void menuCreateObject();
    void menuRayTracing();
    void menuTransform();
    void menuSaveLoad();

    // Ray-tracing sub-menus
    void doStaticRayTracing();
    void doAnimatedRayTracedSphere();

    // Display helpers
    void renderAndDisplay() const;
    void displayMenu()      const;

    // Utility
    static void listFilesInDirectory(const std::string& dir,
                                     std::vector<std::string>& out);
    static std::vector<Vector3> buildProjectorDirs(int layout);

    // State
    Scene* currentScene_;   // raw owning pointer; replaced as needed
};

// ─── Free helper (kept for Bresenham compatibility) ───────────────────────────

void paintLineIntoSpace(VoxelSpace& space, Point start, Point end, float brightness);
void rotatePoint(Point& p, float tx, float ty, float tz);

#endif // ANIMATION_HPP
