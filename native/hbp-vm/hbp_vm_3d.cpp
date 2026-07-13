// ============================================================================
// HBP-VM :: 3D AUTO-ANIMATION ENGINE
// Humanoid Bit Processor Virtual Machine — Inward Turn
// C++17 / OpenGL 3.3 Core Profile
// ============================================================================
//
// Canonical execution law:
//   Delta_t       = U_q(E(S(H_t)), I_t, Omega_t)
//   H~_{t+1}      = B(H_t XOR Delta_t)
//   H_{t+1}       = Pi_Lambda(H~_{t+1})
//   Omega_{t+1}   = Omega_t || Hash(t, q_t, H_t, H_{t+1}, violations)
//
// The body is a deterministic projection of the bit register. Motion amplitude,
// local perturbation, colour, glow and field connectivity are all derived from
// the current HBP-VM state and operational phase.
//
// Build (Linux):
//   g++ -std=c++17 -O3 hbp_vm_3d.cpp -o hbp_vm_3d -lGL -lGLEW -lglfw -lm
// ============================================================================

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace hbp {

constexpr int WINDOW_WIDTH = 1440;
constexpr int WINDOW_HEIGHT = 900;
constexpr std::size_t HBP_STATE_BITS = 1024;
constexpr float VOXEL_SIZE = 0.25F;
constexpr float FOCAL_LENGTH = 800.0F;
constexpr float PI = 3.14159265358979323846F;
constexpr double VM_STEP_HZ = 8.0;
constexpr double VM_STEP_SECONDS = 1.0 / VM_STEP_HZ;
constexpr int MAX_STEPS_PER_FRAME = 4;

struct Vec3 {
    float x{0.0F};
    float y{0.0F};
    float z{0.0F};

    constexpr Vec3() = default;
    constexpr Vec3(float xValue, float yValue, float zValue)
        : x(xValue), y(yValue), z(zValue) {}

    constexpr Vec3 operator+(const Vec3& other) const {
        return {x + other.x, y + other.y, z + other.z};
    }
    constexpr Vec3 operator-(const Vec3& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }
    constexpr Vec3 operator*(float scalar) const {
        return {x * scalar, y * scalar, z * scalar};
    }
};

inline Vec3 rotateX(const Vec3& value, float angle) {
    const float c = std::cos(angle);
    const float s = std::sin(angle);
    return {value.x, value.y * c - value.z * s, value.y * s + value.z * c};
}

inline Vec3 rotateY(const Vec3& value, float angle) {
    const float c = std::cos(angle);
    const float s = std::sin(angle);
    return {value.x * c + value.z * s, value.y, -value.x * s + value.z * c};
}

struct Color {
    std::uint8_t r{0};
    std::uint8_t g{0};
    std::uint8_t b{0};
    std::uint8_t a{255};

    constexpr Color() = default;
    constexpr Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue,
                    std::uint8_t alpha = 255)
        : r(red), g(green), b(blue), a(alpha) {}
};

struct TraceEntry {
    std::uint64_t cycle{0};
    std::uint8_t phase{0};
    std::uint64_t stateHash{0};
    std::uint64_t chainHash{0};
    std::uint64_t violations{0};
};

class BitStateMachine {
public:
    inline static constexpr std::array<const char*, 8> PHASE_NAMES = {
        "SENSE", "PERCEIVE", "DECIDE", "PLAN",
        "ACT", "CHECK", "LEARN", "REST"
    };

    std::array<std::uint8_t, HBP_STATE_BITS> bits{};
    std::uint64_t cycle{0};
    int phase{0};
    std::uint64_t violations{0};
    float entropy{0.0F};
    float coherence{1.0F};

    BitStateMachine() : generator_(std::random_device{}()) {
        seed();
    }

    void seed() {
        std::bernoulli_distribution bitDistribution(0.5);
        for (auto& bit : bits) {
            bit = static_cast<std::uint8_t>(bitDistribution(generator_));
        }
        updateMetrics();
    }

    void reset() {
        bits.fill(0);
        cycle = 0;
        phase = 0;
        violations = 0;
        trace_.clear();
        traceHead_ = FNV_OFFSET;
        seed();
    }

    void step() {
        phase = static_cast<int>(cycle % PHASE_NAMES.size());
        auto proposed = bits;

        switch (phase) {
            case 0:
                sense(proposed);
                break;
            case 1:
                perceive(proposed);
                break;
            case 2:
                decide(proposed);
                break;
            case 3:
                plan(proposed);
                break;
            case 4:
                act(proposed);
                break;
            case 5:
                check(proposed);
                break;
            case 6:
                learn(proposed);
                break;
            case 7:
                break;
            default:
                break;
        }

        // B: force every cell into F_2, then Pi_Lambda is applied last.
        for (auto& bit : proposed) {
            bit &= 1U;
        }
        enforceLambda(proposed);

        const std::uint64_t previousHash = computeStateHash(bits);
        bits = proposed;
        updateMetrics();

        if (phase == 7) {
            commitTrace(previousHash);
        }
        ++cycle;
    }

    [[nodiscard]] std::uint64_t computeStateHash() const {
        return computeStateHash(bits);
    }

    [[nodiscard]] std::uint64_t traceHead() const {
        return traceHead_;
    }

    [[nodiscard]] std::size_t traceCount() const {
        return trace_.size();
    }

private:
    static constexpr std::size_t SAFETY_BEGIN = 896;
    static constexpr std::size_t SAFETY_END = 960;
    static constexpr std::uint64_t FNV_OFFSET = 1469598103934665603ULL;
    static constexpr std::uint64_t FNV_PRIME = 1099511628211ULL;

    std::mt19937_64 generator_;
    std::vector<TraceEntry> trace_;
    std::uint64_t traceHead_{FNV_OFFSET};

    [[nodiscard]] bool chance(double probability) {
        return std::bernoulli_distribution(probability)(generator_);
    }

    void sense(std::array<std::uint8_t, HBP_STATE_BITS>& proposed) {
        std::bernoulli_distribution sensorDistribution(0.70);
        for (std::size_t i = 88; i < 240; ++i) {
            proposed[i] = static_cast<std::uint8_t>(sensorDistribution(generator_));
        }
    }

    void perceive(std::array<std::uint8_t, HBP_STATE_BITS>& proposed) {
        for (std::size_t i = 104; i < 136; ++i) {
            const std::uint8_t neighbours = static_cast<std::uint8_t>(
                bits[(i + HBP_STATE_BITS - 1) % HBP_STATE_BITS] + bits[i] +
                bits[(i + 1) % HBP_STATE_BITS]);
            proposed[i] = static_cast<std::uint8_t>(neighbours >= 2U);
        }
    }

    void decide(std::array<std::uint8_t, HBP_STATE_BITS>& proposed) {
        for (std::size_t i = 8; i < 80; ++i) {
            proposed[i] = static_cast<std::uint8_t>(
                bits[i] ^ bits[(i + 1) % HBP_STATE_BITS] ^ bits[(i + 17) % HBP_STATE_BITS]);
        }
    }

    void plan(std::array<std::uint8_t, HBP_STATE_BITS>& proposed) {
        for (std::size_t i = 168; i < 248; ++i) {
            const std::size_t cognitiveIndex = 8 + ((i - 168) % 72);
            proposed[i] = static_cast<std::uint8_t>(bits[cognitiveIndex] & bits[i]);
        }
    }

    void act(std::array<std::uint8_t, HBP_STATE_BITS>& proposed) {
        for (std::size_t i = 168; i < 248; ++i) {
            proposed[i] = static_cast<std::uint8_t>(bits[i] | chance(0.30));
        }
    }

    void check(std::array<std::uint8_t, HBP_STATE_BITS>& proposed) {
        // Inject attempted constraint faults. Pi_Lambda catches and records them.
        for (std::size_t i = SAFETY_BEGIN; i < SAFETY_END; ++i) {
            if (chance(0.04)) {
                proposed[i] = 0;
            }
        }
    }

    void learn(std::array<std::uint8_t, HBP_STATE_BITS>& proposed) {
        for (std::size_t i = 256; i < 512; ++i) {
            proposed[i] = static_cast<std::uint8_t>(
                bits[i] ^ bits[(i + 256) % HBP_STATE_BITS]);
        }
    }

    void enforceLambda(std::array<std::uint8_t, HBP_STATE_BITS>& proposed) {
        std::uint64_t corrections = 0;
        for (std::size_t i = SAFETY_BEGIN; i < SAFETY_END; ++i) {
            if (proposed[i] == 0U) {
                proposed[i] = 1U;
                ++corrections;
            }
        }
        violations += corrections;
    }

    void updateMetrics() {
        std::size_t ones = 0;
        std::size_t neighbourAgreements = 0;
        for (std::size_t i = 0; i < HBP_STATE_BITS; ++i) {
            ones += bits[i];
            neighbourAgreements += static_cast<std::size_t>(bits[i] == bits[(i + 1) % HBP_STATE_BITS]);
        }

        const float p = static_cast<float>(ones) / static_cast<float>(HBP_STATE_BITS);
        if (p <= 0.0F || p >= 1.0F) {
            entropy = 0.0F;
        } else {
            entropy = -(p * std::log2(p) + (1.0F - p) * std::log2(1.0F - p));
        }
        coherence = static_cast<float>(neighbourAgreements) / static_cast<float>(HBP_STATE_BITS);
    }

    static std::uint64_t fnvByte(std::uint64_t hash, std::uint8_t value) {
        return (hash ^ value) * FNV_PRIME;
    }

    static std::uint64_t hashU64(std::uint64_t hash, std::uint64_t value) {
        for (int shift = 0; shift < 64; shift += 8) {
            hash = fnvByte(hash, static_cast<std::uint8_t>((value >> shift) & 0xFFU));
        }
        return hash;
    }

    static std::uint64_t computeStateHash(
        const std::array<std::uint8_t, HBP_STATE_BITS>& state) {
        std::uint64_t hash = FNV_OFFSET;
        for (const auto bit : state) {
            hash = fnvByte(hash, bit);
        }
        return hash;
    }

    void commitTrace(std::uint64_t previousHash) {
        const std::uint64_t stateHash = computeStateHash(bits);
        std::uint64_t chain = traceHead_;
        chain = hashU64(chain, cycle);
        chain = hashU64(chain, static_cast<std::uint64_t>(phase));
        chain = hashU64(chain, previousHash);
        chain = hashU64(chain, stateHash);
        chain = hashU64(chain, violations);

        trace_.push_back(TraceEntry{
            cycle,
            static_cast<std::uint8_t>(phase),
            stateHash,
            chain,
            violations
        });
        traceHead_ = chain;
    }
};

struct Voxel {
    float x{0.0F};
    float y{0.0F};
    float z{0.0F};
    float baseX{0.0F};
    float baseY{0.0F};
    float baseZ{0.0F};
    std::string part;
    std::size_t bitIndex{0};
    Color color;
    float glowIntensity{0.0F};
};

class HumanoidVoxel {
public:
    std::vector<Voxel> voxels;

    HumanoidVoxel() {
        buildBody();
    }

    void animate(float timeMilliseconds,
                 const std::array<std::uint8_t, HBP_STATE_BITS>& bitState,
                 int phase) {
        const float timeSeconds = timeMilliseconds * 0.001F;
        const float sensorDrive = bitActivity(bitState, 88, 168);
        const float cognitiveDrive = bitActivity(bitState, 8, 80);
        const float motorDrive = bitActivity(bitState, 168, 248);
        const float memoryDrive = bitActivity(bitState, 256, 512);
        const float phaseDrive = static_cast<float>(phase + 1) / 8.0F;

        const float gaitFrequency = 1.2F + motorDrive * 2.4F;
        const float walkCycle = std::sin(timeSeconds * gaitFrequency * 2.0F);
        const float breath = std::sin(timeSeconds * (0.7F + sensorDrive)) *
                             (0.03F + 0.09F * sensorDrive);
        const float pulsePhase = timeSeconds * (1.5F + memoryDrive * 3.0F);

        for (auto& voxel : voxels) {
            float px = voxel.baseX;
            float py = voxel.baseY;
            float pz = voxel.baseZ;

            if (voxel.part == "torso") {
                const float radial = std::sqrt(px * px + pz * pz);
                const float expansion = 1.0F + breath * 0.05F * radial;
                px *= expansion;
                pz *= expansion;
                py += breath * 0.12F;
            }

            if (voxel.part == "head") {
                py += breath * 0.18F;
                px += std::sin(timeSeconds * (0.4F + cognitiveDrive)) *
                      0.08F * cognitiveDrive;
            }

            const float actionGate = (phase == 3 || phase == 4) ? 1.0F : 0.35F;
            const float limbAmplitude = motorDrive * actionGate;

            if (isLeftArm(voxel.part)) {
                const float swing = walkCycle * 0.38F * limbAmplitude;
                px += swing;
                pz += swing * 0.45F;
                py += std::abs(swing) * 0.10F;
            } else if (isRightArm(voxel.part)) {
                const float swing = -walkCycle * 0.38F * limbAmplitude;
                px += swing;
                pz += swing * 0.45F;
                py += std::abs(swing) * 0.10F;
            }

            if (isLeftLeg(voxel.part)) {
                const float swing = walkCycle * 0.42F * limbAmplitude;
                pz += swing;
                py += std::abs(swing) * 0.15F;
            } else if (isRightLeg(voxel.part)) {
                const float swing = -walkCycle * 0.42F * limbAmplitude;
                pz += swing;
                py += std::abs(swing) * 0.15F;
            }

            const bool active = bitState[voxel.bitIndex] != 0U;
            if (active) {
                const float localPhase = pulsePhase + static_cast<float>(voxel.bitIndex) * 0.071F;
                const float perturbation = 0.008F + 0.018F * phaseDrive;
                px += std::sin(localPhase) * perturbation;
                py += std::cos(localPhase * 0.83F) * perturbation;
                pz += std::sin(localPhase * 0.61F) * perturbation;
            }

            voxel.x = px;
            voxel.y = py;
            voxel.z = pz;
            voxel.color = computeColor(voxel, phase, bitState, pulsePhase);
            voxel.glowIntensity = active
                ? 0.25F + 0.75F * (0.5F + 0.5F * std::sin(
                      pulsePhase + static_cast<float>(voxel.bitIndex) * 0.1F))
                : 0.0F;
        }
    }

private:
    static float bitActivity(const std::array<std::uint8_t, HBP_STATE_BITS>& state,
                             std::size_t begin,
                             std::size_t end) {
        if (begin >= end || end > state.size()) {
            return 0.0F;
        }
        std::size_t active = 0;
        for (std::size_t i = begin; i < end; ++i) {
            active += state[i];
        }
        return static_cast<float>(active) / static_cast<float>(end - begin);
    }

    static bool isLeftArm(const std::string& part) {
        return part == "leftArm" || part == "leftForearm";
    }
    static bool isRightArm(const std::string& part) {
        return part == "rightArm" || part == "rightForearm";
    }
    static bool isLeftLeg(const std::string& part) {
        return part == "leftLeg" || part == "leftShin" || part == "leftFoot";
    }
    static bool isRightLeg(const std::string& part) {
        return part == "rightLeg" || part == "rightShin" || part == "rightFoot";
    }

    void addBox(float x, float y, float z, float width, float height, float depth,
                const std::string& part) {
        for (float ix = 0.0F; ix < width; ix += VOXEL_SIZE) {
            for (float iy = 0.0F; iy < height; iy += VOXEL_SIZE) {
                for (float iz = 0.0F; iz < depth; iz += VOXEL_SIZE) {
                    Voxel voxel;
                    voxel.baseX = x + ix;
                    voxel.baseY = y + iy;
                    voxel.baseZ = z + iz;
                    voxel.x = voxel.baseX;
                    voxel.y = voxel.baseY;
                    voxel.z = voxel.baseZ;
                    voxel.part = part;
                    voxel.bitIndex = voxels.size() % HBP_STATE_BITS;
                    voxels.push_back(std::move(voxel));
                }
            }
        }
    }

    void buildBody() {
        addBox(-0.8F, 5.5F, -0.8F, 1.6F, 1.8F, 1.6F, "head");
        addBox(-0.3F, 5.0F, -0.3F, 0.6F, 0.5F, 0.6F, "neck");
        addBox(-1.3F, 2.5F, -0.7F, 2.6F, 2.5F, 1.4F, "torso");
        addBox(-2.3F, 3.0F, -0.4F, 0.7F, 2.2F, 0.7F, "leftArm");
        addBox(-2.3F, 0.5F, -0.4F, 0.6F, 2.0F, 0.6F, "leftForearm");
        addBox(1.6F, 3.0F, -0.4F, 0.7F, 2.2F, 0.7F, "rightArm");
        addBox(1.7F, 0.5F, -0.4F, 0.6F, 2.0F, 0.6F, "rightForearm");
        addBox(-1.1F, -0.5F, -0.5F, 0.9F, 3.0F, 0.9F, "leftLeg");
        addBox(-1.1F, -3.5F, -0.5F, 0.8F, 3.0F, 0.8F, "leftShin");
        addBox(-1.2F, -4.5F, -0.6F, 1.0F, 1.0F, 1.3F, "leftFoot");
        addBox(0.2F, -0.5F, -0.5F, 0.9F, 3.0F, 0.9F, "rightLeg");
        addBox(0.3F, -3.5F, -0.5F, 0.8F, 3.0F, 0.8F, "rightShin");
        addBox(0.2F, -4.5F, -0.6F, 1.0F, 1.0F, 1.3F, "rightFoot");
    }

    static Color computeColor(const Voxel& voxel,
                              int phase,
                              const std::array<std::uint8_t, HBP_STATE_BITS>& bitState,
                              float pulsePhase) {
        static constexpr std::array<Color, 8> phaseColors = {
            Color(0, 240, 255), Color(204, 102, 255), Color(0, 255, 136),
            Color(255, 204, 0), Color(255, 51, 102), Color(255, 102, 51),
            Color(102, 204, 255), Color(153, 153, 153)
        };

        const bool active = bitState[voxel.bitIndex] != 0U;
        Color color = phaseColors[static_cast<std::size_t>(phase) % phaseColors.size()];

        if (voxel.part == "head") {
            color = active ? Color(255, 255, 200) : Color(100, 100, 90);
        } else if (voxel.part == "torso" || voxel.part == "neck") {
            color = active ? Color(205, 105, 115) : Color(90, 45, 50);
        } else if (isLeftArm(voxel.part) || isRightArm(voxel.part)) {
            color = active ? Color(100, 210, 125) : Color(45, 90, 55);
        } else if (isLeftLeg(voxel.part) || isRightLeg(voxel.part)) {
            color = active ? Color(105, 125, 220) : Color(45, 55, 100);
        }

        if (active) {
            const float glow = 0.5F + 0.5F * std::sin(
                pulsePhase + static_cast<float>(voxel.bitIndex) * 0.1F);
            color.r = static_cast<std::uint8_t>(std::min(255.0F, color.r + glow * 60.0F));
            color.g = static_cast<std::uint8_t>(std::min(255.0F, color.g + glow * 60.0F));
            color.b = static_cast<std::uint8_t>(std::min(255.0F, color.b + glow * 60.0F));
        }
        return color;
    }
};

struct Camera {
    Vec3 position{0.0F, 1.5F, 14.0F};
    float rotX{0.10F};
    float rotY{0.0F};
    bool autoRotate{true};
    bool dragging{false};
    float lastMouseX{0.0F};
    float lastMouseY{0.0F};

    void update(float timeMilliseconds) {
        if (autoRotate) {
            rotY = timeMilliseconds * 0.00022F;
            rotX = 0.10F + std::sin(timeMilliseconds * 0.00035F) * 0.08F;
        }
    }
};

struct ProjectedVoxel {
    float screenX{0.0F};
    float screenY{0.0F};
    float depth{0.0F};
    const Voxel* voxel{nullptr};
};

struct GpuVertex {
    float x{0.0F};
    float y{0.0F};
    float z{0.0F};
    float r{1.0F};
    float g{1.0F};
    float b{1.0F};
    float a{1.0F};
};

class Renderer {
public:
    Camera camera;

    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    ~Renderer() {
        shutdown();
    }

    bool initialize() {
        program_ = createProgram();
        if (program_ == 0U) {
            return false;
        }

        pointModeLocation_ = glGetUniformLocation(program_, "uPointMode");
        pointSizeLocation_ = glGetUniformLocation(program_, "uPointSize");

        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);
        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);

        constexpr GLsizei stride = static_cast<GLsizei>(sizeof(GpuVertex));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                              reinterpret_cast<const void*>(offsetof(GpuVertex, x)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride,
                              reinterpret_cast<const void*>(offsetof(GpuVertex, r)));

        glBindVertexArray(0);
        glEnable(GL_PROGRAM_POINT_SIZE);
        return true;
    }

    void shutdown() {
        if (vbo_ != 0U) {
            glDeleteBuffers(1, &vbo_);
            vbo_ = 0U;
        }
        if (vao_ != 0U) {
            glDeleteVertexArrays(1, &vao_);
            vao_ = 0U;
        }
        if (program_ != 0U) {
            glDeleteProgram(program_);
            program_ = 0U;
        }
    }

    void render(GLFWwindow* window,
                HumanoidVoxel& humanoid,
                BitStateMachine& machine,
                float timeMilliseconds,
                bool paused) {
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        if (width <= 0 || height <= 0) {
            return;
        }

        glViewport(0, 0, width, height);
        glClearColor(0.018F, 0.018F, 0.030F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        camera.update(timeMilliseconds);
        humanoid.animate(timeMilliseconds, machine.bits, machine.phase);

        auto projected = project(humanoid, width, height);
        renderFieldLines(projected, machine, width, height);
        renderVoxels(projected, width, height);
        renderHud(machine);
        updateWindowTitle(window, machine, humanoid.voxels.size(), paused, timeMilliseconds);
    }

private:
    GLuint program_{0};
    GLuint vao_{0};
    GLuint vbo_{0};
    GLint pointModeLocation_{-1};
    GLint pointSizeLocation_{-1};
    float lastTitleUpdate_{-1000.0F};

    static constexpr const char* VERTEX_SHADER = R"GLSL(
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
out vec4 vColor;
uniform float uPointSize;
void main() {
    gl_Position = vec4(aPosition, 1.0);
    gl_PointSize = uPointSize;
    vColor = aColor;
}
)GLSL";

    static constexpr const char* FRAGMENT_SHADER = R"GLSL(
#version 330 core
in vec4 vColor;
out vec4 FragColor;
uniform bool uPointMode;
void main() {
    float alpha = vColor.a;
    if (uPointMode) {
        float distanceFromCentre = length(gl_PointCoord - vec2(0.5));
        if (distanceFromCentre > 0.5) {
            discard;
        }
        alpha *= 1.0 - smoothstep(0.28, 0.5, distanceFromCentre);
    }
    FragColor = vec4(vColor.rgb, alpha);
}
)GLSL";

    static GLuint compileShader(GLenum type, const char* source) {
        const GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (success != GL_TRUE) {
            GLint length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> log(static_cast<std::size_t>(std::max(1, length)));
            glGetShaderInfoLog(shader, length, nullptr, log.data());
            std::cerr << "Shader compilation failed: " << log.data() << '\n';
            glDeleteShader(shader);
            return 0U;
        }
        return shader;
    }

    static GLuint createProgram() {
        const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, VERTEX_SHADER);
        if (vertexShader == 0U) {
            return 0U;
        }
        const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER);
        if (fragmentShader == 0U) {
            glDeleteShader(vertexShader);
            return 0U;
        }

        const GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        GLint success = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (success != GL_TRUE) {
            GLint length = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> log(static_cast<std::size_t>(std::max(1, length)));
            glGetProgramInfoLog(program, length, nullptr, log.data());
            std::cerr << "Program linking failed: " << log.data() << '\n';
            glDeleteProgram(program);
            return 0U;
        }
        return program;
    }

    std::vector<ProjectedVoxel> project(const HumanoidVoxel& humanoid,
                                        int width,
                                        int height) const {
        std::vector<ProjectedVoxel> projected;
        projected.reserve(humanoid.voxels.size());

        for (const auto& voxel : humanoid.voxels) {
            Vec3 point(voxel.x, voxel.y, voxel.z);
            point = point - camera.position;
            point = rotateY(point, -camera.rotY);
            point = rotateX(point, -camera.rotX);

            const float depth = -point.z;
            if (depth <= 0.1F) {
                continue;
            }

            const float scale = FOCAL_LENGTH / depth;
            projected.push_back(ProjectedVoxel{
                static_cast<float>(width) * 0.5F + point.x * scale,
                static_cast<float>(height) * 0.5F - point.y * scale,
                depth,
                &voxel
            });
        }

        std::sort(projected.begin(), projected.end(),
                  [](const ProjectedVoxel& left, const ProjectedVoxel& right) {
                      return left.depth > right.depth;
                  });
        return projected;
    }

    static GpuVertex toGpuVertex(const ProjectedVoxel& projected,
                                 int width,
                                 int height,
                                 float alphaScale = 1.0F) {
        const Color& color = projected.voxel->color;
        return GpuVertex{
            (projected.screenX / static_cast<float>(width)) * 2.0F - 1.0F,
            1.0F - (projected.screenY / static_cast<float>(height)) * 2.0F,
            0.0F,
            color.r / 255.0F,
            color.g / 255.0F,
            color.b / 255.0F,
            std::clamp(alphaScale, 0.0F, 1.0F)
        };
    }

    void draw(GLenum primitive,
              const std::vector<GpuVertex>& vertices,
              bool pointMode,
              float pointSize = 1.0F) {
        if (vertices.empty()) {
            return;
        }

        glUseProgram(program_);
        glUniform1i(pointModeLocation_, pointMode ? GL_TRUE : GL_FALSE);
        glUniform1f(pointSizeLocation_, pointSize);
        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(vertices.size() * sizeof(GpuVertex)),
                     vertices.data(),
                     GL_STREAM_DRAW);
        glDrawArrays(primitive, 0, static_cast<GLsizei>(vertices.size()));
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void renderVoxels(const std::vector<ProjectedVoxel>& projected,
                      int width,
                      int height) {
        std::vector<GpuVertex> basePoints;
        std::vector<GpuVertex> glowPoints;
        basePoints.reserve(projected.size());
        glowPoints.reserve(projected.size() / 2U);

        for (const auto& point : projected) {
            const float depthAlpha = std::clamp(16.0F / point.depth, 0.20F, 1.0F);
            basePoints.push_back(toGpuVertex(point, width, height, depthAlpha));
            if (point.voxel->glowIntensity > 0.0F) {
                glowPoints.push_back(toGpuVertex(
                    point, width, height,
                    0.45F * depthAlpha * point.voxel->glowIntensity));
            }
        }

        draw(GL_POINTS, glowPoints, true, 9.0F);
        draw(GL_POINTS, basePoints, true, 4.0F);
    }

    void renderFieldLines(const std::vector<ProjectedVoxel>& projected,
                          const BitStateMachine& machine,
                          int width,
                          int height) {
        static constexpr std::array<Color, 8> phaseColors = {
            Color(0, 240, 255), Color(204, 102, 255), Color(0, 255, 136),
            Color(255, 204, 0), Color(255, 51, 102), Color(255, 102, 51),
            Color(102, 204, 255), Color(153, 153, 153)
        };
        const Color color = phaseColors[static_cast<std::size_t>(machine.phase)];

        std::vector<GpuVertex> lines;
        for (std::size_t i = 0; i < projected.size(); i += 5) {
            const auto& first = projected[i];
            if (machine.bits[first.voxel->bitIndex] == 0U) {
                continue;
            }

            const std::size_t localEnd = std::min(i + 20U, projected.size());
            for (std::size_t j = i + 1; j < localEnd; j += 3) {
                const auto& second = projected[j];
                if (machine.bits[second.voxel->bitIndex] == 0U) {
                    continue;
                }

                const float dx = first.screenX - second.screenX;
                const float dy = first.screenY - second.screenY;
                const float distance = std::sqrt(dx * dx + dy * dy);
                if (distance >= 80.0F) {
                    continue;
                }

                const float alpha = 0.16F * (1.0F - distance / 80.0F);
                const auto makeLineVertex = [&](const ProjectedVoxel& point) {
                    return GpuVertex{
                        (point.screenX / static_cast<float>(width)) * 2.0F - 1.0F,
                        1.0F - (point.screenY / static_cast<float>(height)) * 2.0F,
                        0.0F,
                        color.r / 255.0F,
                        color.g / 255.0F,
                        color.b / 255.0F,
                        alpha
                    };
                };
                lines.push_back(makeLineVertex(first));
                lines.push_back(makeLineVertex(second));
            }
        }
        draw(GL_LINES, lines, false);
    }

    static void appendRect(std::vector<GpuVertex>& vertices,
                           float left,
                           float bottom,
                           float right,
                           float top,
                           const Color& color,
                           float alpha) {
        const float r = color.r / 255.0F;
        const float g = color.g / 255.0F;
        const float b = color.b / 255.0F;
        const std::array<GpuVertex, 6> rect = {
            GpuVertex{left, bottom, 0.0F, r, g, b, alpha},
            GpuVertex{right, bottom, 0.0F, r, g, b, alpha},
            GpuVertex{right, top, 0.0F, r, g, b, alpha},
            GpuVertex{left, bottom, 0.0F, r, g, b, alpha},
            GpuVertex{right, top, 0.0F, r, g, b, alpha},
            GpuVertex{left, top, 0.0F, r, g, b, alpha}
        };
        vertices.insert(vertices.end(), rect.begin(), rect.end());
    }

    void renderHud(const BitStateMachine& machine) {
        static constexpr std::array<Color, 8> phaseColors = {
            Color(0, 240, 255), Color(204, 102, 255), Color(0, 255, 136),
            Color(255, 204, 0), Color(255, 51, 102), Color(255, 102, 51),
            Color(102, 204, 255), Color(153, 153, 153)
        };

        std::vector<GpuVertex> triangles;
        appendRect(triangles, -0.30F, 0.84F, 0.30F, 0.875F, Color(24, 24, 38), 0.86F);
        const float phaseWidth = 0.60F / 8.0F;
        const float activeLeft = -0.30F + phaseWidth * static_cast<float>(machine.phase);
        appendRect(triangles, activeLeft, 0.84F, activeLeft + phaseWidth, 0.875F,
                   phaseColors[static_cast<std::size_t>(machine.phase)], 0.95F);
        appendRect(triangles, 0.66F, -0.90F, 0.96F, 0.90F, Color(12, 12, 20), 0.72F);

        // Metric bars: entropy, coherence and normalised trace activity.
        appendRect(triangles, 0.70F, 0.52F, 0.92F, 0.56F, Color(25, 25, 40), 0.95F);
        appendRect(triangles, 0.70F, 0.52F,
                   0.70F + 0.22F * std::clamp(machine.entropy, 0.0F, 1.0F), 0.56F,
                   Color(204, 102, 255), 0.95F);
        appendRect(triangles, 0.70F, 0.42F, 0.92F, 0.46F, Color(25, 25, 40), 0.95F);
        appendRect(triangles, 0.70F, 0.42F,
                   0.70F + 0.22F * std::clamp(machine.coherence, 0.0F, 1.0F), 0.46F,
                   Color(0, 240, 255), 0.95F);

        draw(GL_TRIANGLES, triangles, false);

        std::vector<GpuVertex> border = {
            {0.66F, -0.90F, 0.0F, 1.0F, 0.2F, 0.4F, 0.5F},
            {0.96F, -0.90F, 0.0F, 1.0F, 0.2F, 0.4F, 0.5F},
            {0.96F, -0.90F, 0.0F, 1.0F, 0.2F, 0.4F, 0.5F},
            {0.96F, 0.90F, 0.0F, 1.0F, 0.2F, 0.4F, 0.5F},
            {0.96F, 0.90F, 0.0F, 1.0F, 0.2F, 0.4F, 0.5F},
            {0.66F, 0.90F, 0.0F, 1.0F, 0.2F, 0.4F, 0.5F},
            {0.66F, 0.90F, 0.0F, 1.0F, 0.2F, 0.4F, 0.5F},
            {0.66F, -0.90F, 0.0F, 1.0F, 0.2F, 0.4F, 0.5F}
        };
        draw(GL_LINES, border, false);
    }

    void updateWindowTitle(GLFWwindow* window,
                           const BitStateMachine& machine,
                           std::size_t voxelCount,
                           bool paused,
                           float timeMilliseconds) {
        if (timeMilliseconds - lastTitleUpdate_ < 250.0F) {
            return;
        }
        lastTitleUpdate_ = timeMilliseconds;

        std::ostringstream title;
        title << "HBP-VM | " << (paused ? "HALT" : "RUN")
              << " | phase=" << BitStateMachine::PHASE_NAMES[static_cast<std::size_t>(machine.phase)]
              << " | cycle=" << machine.cycle
              << " | voxels=" << voxelCount
              << " | H=" << std::fixed << std::setprecision(3) << machine.entropy
              << " | Phi=" << machine.coherence
              << " | Lambda=" << machine.violations
              << " | Omega=" << machine.traceCount();
        glfwSetWindowTitle(window, title.str().c_str());
    }
};

struct ApplicationContext {
    BitStateMachine* machine{nullptr};
    Renderer* renderer{nullptr};
    bool paused{false};
};

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
    if (action != GLFW_PRESS) {
        return;
    }

    auto* context = static_cast<ApplicationContext*>(glfwGetWindowUserPointer(window));
    if (context == nullptr || context->machine == nullptr || context->renderer == nullptr) {
        return;
    }

    switch (key) {
        case GLFW_KEY_R:
            context->machine->reset();
            std::cout << "[HBP-VM] State reset.\n";
            break;
        case GLFW_KEY_SPACE:
            context->paused = !context->paused;
            std::cout << (context->paused ? "[HALT]\n" : "[RUN]\n");
            break;
        case GLFW_KEY_A:
            context->renderer->camera.autoRotate = !context->renderer->camera.autoRotate;
            std::cout << "[HBP-VM] Auto-camera "
                      << (context->renderer->camera.autoRotate ? "enabled" : "disabled") << ".\n";
            break;
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        default:
            break;
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int) {
    auto* context = static_cast<ApplicationContext*>(glfwGetWindowUserPointer(window));
    if (context == nullptr || context->renderer == nullptr || button != GLFW_MOUSE_BUTTON_LEFT) {
        return;
    }

    auto& camera = context->renderer->camera;
    camera.dragging = (action == GLFW_PRESS);
    if (action == GLFW_PRESS) {
        camera.autoRotate = false;
        double x = 0.0;
        double y = 0.0;
        glfwGetCursorPos(window, &x, &y);
        camera.lastMouseX = static_cast<float>(x);
        camera.lastMouseY = static_cast<float>(y);
    }
}

void cursorPositionCallback(GLFWwindow* window, double x, double y) {
    auto* context = static_cast<ApplicationContext*>(glfwGetWindowUserPointer(window));
    if (context == nullptr || context->renderer == nullptr) {
        return;
    }

    auto& camera = context->renderer->camera;
    if (!camera.dragging) {
        return;
    }

    const float currentX = static_cast<float>(x);
    const float currentY = static_cast<float>(y);
    camera.rotY += (currentX - camera.lastMouseX) * 0.005F;
    camera.rotX += (currentY - camera.lastMouseY) * 0.005F;
    camera.rotX = std::clamp(camera.rotX, -1.3F, 1.3F);
    camera.lastMouseX = currentX;
    camera.lastMouseY = currentY;
}

void scrollCallback(GLFWwindow* window, double, double yOffset) {
    auto* context = static_cast<ApplicationContext*>(glfwGetWindowUserPointer(window));
    if (context == nullptr || context->renderer == nullptr) {
        return;
    }

    auto& camera = context->renderer->camera;
    camera.position.z -= static_cast<float>(yOffset) * 0.7F;
    camera.position.z = std::clamp(camera.position.z, 6.0F, 30.0F);
}

}  // namespace hbp

int main() {
    using namespace hbp;

    std::cout << "========================================================================\n"
              << "  HBP-VM :: 3D AUTO-ANIMATION ENGINE\n"
              << "  OpenGL 3.3 Core Profile / C++17\n"
              << "========================================================================\n"
              << "  H_next = Pi_Lambda(B(H XOR Delta))\n"
              << "  Omega_next = Omega || Hash(cycle, phase, H_prev, H_next, violations)\n\n"
              << "  Controls\n"
              << "    Mouse drag   Rotate camera\n"
              << "    Mouse wheel  Zoom\n"
              << "    Space        HALT/RUN\n"
              << "    R            Reseed state\n"
              << "    A            Toggle auto-camera\n"
              << "    Esc          Exit\n\n";

    if (glfwInit() != GLFW_TRUE) {
        std::cerr << "Failed to initialise GLFW.\n";
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(
        WINDOW_WIDTH, WINDOW_HEIGHT,
        "HBP-VM :: 3D Auto-Animation", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create the GLFW window.\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glewExperimental = GL_TRUE;
    const GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
        std::cerr << "Failed to initialise GLEW: "
                  << reinterpret_cast<const char*>(glewGetErrorString(glewStatus)) << '\n';
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }
    // GLEW may emit GL_INVALID_ENUM while probing a core context.
    while (glGetError() != GL_NO_ERROR) {}

    BitStateMachine machine;
    HumanoidVoxel humanoid;
    Renderer renderer;
    if (!renderer.initialize()) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    ApplicationContext context{&machine, &renderer, false};
    glfwSetWindowUserPointer(window, &context);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetScrollCallback(window, scrollCallback);

    auto previousTime = std::chrono::steady_clock::now();
    double accumulator = 0.0;
    double simulationTimeSeconds = 0.0;

    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        glfwPollEvents();

        const auto currentTime = std::chrono::steady_clock::now();
        double frameSeconds = std::chrono::duration<double>(currentTime - previousTime).count();
        previousTime = currentTime;
        frameSeconds = std::min(frameSeconds, 0.25);

        if (!context.paused) {
            accumulator += frameSeconds;
            simulationTimeSeconds += frameSeconds;
            int steps = 0;
            while (accumulator >= VM_STEP_SECONDS && steps < MAX_STEPS_PER_FRAME) {
                machine.step();
                accumulator -= VM_STEP_SECONDS;
                ++steps;
            }
            if (steps == MAX_STEPS_PER_FRAME) {
                accumulator = 0.0;
            }
        } else {
            accumulator = 0.0;
        }

        renderer.render(window, humanoid, machine,
                        static_cast<float>(simulationTimeSeconds * 1000.0),
                        context.paused);
        glfwSwapBuffers(window);
    }

    renderer.shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "\n========================================================================\n"
              << "  HBP-VM :: SHUTDOWN COMPLETE\n"
              << "  Final cycle:       " << machine.cycle << '\n'
              << "  Final entropy:     " << machine.entropy << '\n'
              << "  Final coherence:   " << machine.coherence << '\n'
              << "  Lambda corrections:" << machine.violations << '\n'
              << "  Omega entries:     " << machine.traceCount() << '\n'
              << "  Omega head:        0x" << std::hex << machine.traceHead() << std::dec << '\n'
              << "========================================================================\n";
    return EXIT_SUCCESS;
}
