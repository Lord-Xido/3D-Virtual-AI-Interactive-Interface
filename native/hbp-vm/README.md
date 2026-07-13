# HBP-VM 3D Auto-Animation Engine

The HBP-VM native engine renders a humanoid as a direct projection of a 1,024-bit virtual-machine state. Voxel colour, glow, local displacement, gait amplitude, breathing amplitude and neural-field connectivity are derived from the current register and its eight-phase control cycle.

## Canonical state law

```text
Delta_t     = U_q(E(S(H_t)), I_t, Omega_t)
H~_{t+1}    = B(H_t XOR Delta_t)
H_{t+1}     = Pi_Lambda(H~_{t+1})
Omega_{t+1} = Omega_t || Hash(t, q_t, H_t, H_{t+1}, violations)
```

`Pi_Lambda` is deliberately the final state operation. Trace data is appended to a separate hash chain and cannot reactivate a bit rejected by the safety projection.

## Eight-phase machine

```text
SENSE -> PERCEIVE -> DECIDE -> PLAN -> ACT -> CHECK -> LEARN -> REST
```

The VM advances at a fixed 8 Hz while rendering remains frame-rate independent. This makes every phase observable and prevents the state machine from accelerating with display refresh rate.

## Build

Ubuntu/Debian dependencies:

```bash
sudo apt-get install build-essential cmake libgl1-mesa-dev libglew-dev libglfw3-dev
```

Compile with CMake:

```bash
cmake -S . -B build
cmake --build build --config Release
./build/hbp_vm_3d
```

Or compile directly:

```bash
g++ -std=c++17 -O3 hbp_vm_3d.cpp -o hbp_vm_3d -lGL -lGLEW -lglfw -lm
```

## Controls

- **Mouse drag** — rotate the camera and disable automatic orbit
- **Mouse wheel** — zoom
- **Space** — HALT/RUN
- **R** — reseed the VM state and reset the trace chain
- **A** — toggle automatic camera motion
- **Esc** — exit

## Operational corrections incorporated

- Uses VAO/VBO draw calls compatible with an OpenGL 3.3 core context; no deprecated immediate-mode calls.
- Uses one application context for all GLFW callbacks, avoiding the overwritten-user-pointer bug.
- Toggles pause on key events rather than a duplicated static latch.
- Projects camera-space points with a consistent negative-Z forward convention.
- Decouples VM stepping from render FPS.
- Counts every Lambda correction and records REST states in an append-only FNV-1a hash chain.
- Keeps Omega trace state separate from executable bits so audit feedback cannot bypass Lambda.
