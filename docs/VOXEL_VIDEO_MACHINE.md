# 2048³ Virtual Video Animation Machine

## 1. System definition

The machine represents a time-varying virtual field:

\[
\mathcal V_t[x,y,z],\qquad 0\le x,y,z<2048.
\]

Because `2048 = 2¹¹`, each coordinate axis requires 11 bits and a complete coordinate requires 33 bits:

\[
N=2048^3=2^{33}=8,589,934,592.
\]

The row-major address is

\[
A(x,y,z)=x+2048y+2048^2z.
\]

The virtual dimensions describe the addressable domain. They do not imply dense physical allocation.

## 2. Sparse brick memory

The world is divided into `32³` bricks. Each address splits into a 6-bit brick coordinate and a 5-bit local coordinate on every axis:

\[
(x,y,z)\rightarrow(b_x,b_y,b_z,l_x,l_y,l_z).
\]

This produces a `64³` virtual brick lattice containing 262,144 possible bricks. The browser runtime keeps no more than 900 page records resident and renders at most 12,000 instanced voxel samples.

The page table tracks demand priority, last-touch frame, semantic state and field heat. A velocity-led camera predictor requests likely future pages. Low-value pages are removed through a score combining priority, heat and age.

## 3. Runtime state

The conceptual state is

\[
\Sigma_t=(V_t,Z_t,M_t,P_t,E_t,\Omega_t,G_t,C_t,L_t,R_t,A_t,Q_t).
\]

- `V`: virtual voxel field;
- `Z`: encoded or latent state;
- `M`: material and physical metadata;
- `P`: predicted state and page demand;
- `E`: prediction or rendering residual;
- `Ω`: persistent correction memory;
- `G`: scene geometry and transforms;
- `C`: camera state;
- `L`: lighting state;
- `R`: rendered frame;
- `A`: capture or audiovisual synchronization state;
- `Q`: scheduler, page table and execution queues.

The browser demonstration operationalizes a bounded subset of these registers while preserving the full state contract for future render backends.

## 4. Three-bit semantic state

| Bits | Meaning |
|---|---|
| `000` | empty |
| `001` | geometry |
| `010` | emissive |
| `011` | volumetric medium |
| `100` | dynamic or particle |
| `101` | latent or AI-controlled |
| `110` | control or bytecode |
| `111` | residual or compound |

The 3-bit value selects an operational class. Rich properties remain in optional channels or procedural functions.

## 5. Deterministic fixed-step transition

Display refresh and simulation time are separated. The display uses `requestAnimationFrame`; the simulation advances in fixed increments:

\[
\Delta t=\frac{1}{60}\text{ s}.
\]

The transition is

\[
\Sigma_{t+\Delta t}=\Pi_\Lambda\left[
\Sigma_t+\mathcal F_{animation}+\mathcal F_{simulation}+P_t-E_t+\Omega_t
\right].
\]

At most four simulation substeps may execute per display frame. Excess accumulated time is discarded and recorded, preventing runaway catch-up after stalls.

## 6. Frame pipeline

Each active cycle performs:

1. acquire the display timestamp and user controls;
2. advance the fixed-step accumulator;
3. evaluate the selected procedural field program;
4. predict camera-centred brick demand with velocity lead;
5. page in useful virtual regions;
6. encode 3-bit voxel semantics;
7. calculate residual RMS;
8. update `Ω` per active page;
9. evict low-score pages above the residency limit;
10. write the bounded GPU instance buffer;
11. render the scene through the camera and lights;
12. update frame, phase and paging telemetry;
13. adapt effective quality against the selected frame budget;
14. optionally capture the canvas to WebM;
15. optionally export a JSON state snapshot.

## 7. Adaptive quality gate

The requested voxel budget is an upper bound. The `Λ` quality gate lowers the effective budget when the frame-time EWMA exceeds the target and restores detail gradually when sufficient headroom returns.

The controller supports 30 FPS and 60 FPS targets, enforces a 512-instance minimum, and never exceeds the 12,000-instance GPU pool. A cooldown avoids rapid oscillation.

## 8. Rendering and capture

`THREE.InstancedMesh` represents the active field with one instanced draw structure rather than one JavaScript object per voxel. The current engine is an architectural visualization, not a dense renderer of all 8.59 billion cells.

A production backend can replace the instanced renderer with WebGPU compute, sparse 3D textures, octree traversal, ray marching, meshlet extraction or neural reconstruction while preserving the same virtual-memory and state-transition contract.

The 3D world is projected into a conventional frame:

\[
R_n\in\mathbb R^{W\times H\times4}.
\]

The recorder negotiates VP9, VP8 or generic WebM support at runtime. Codec output is browser-dependent and is not expected to be bit-identical across platforms.

## 9. Telemetry and provenance

The HUD reports:

- display and simulation frame counters;
- simulation time;
- active pages and resident voxel instances;
- requested and effective quality budgets;
- residual RMS;
- FPS and frame-time EWMA;
- update and render phase times;
- page hit rate, faults and evictions;
- adaptive-quality state and simulation substep count.

`Export Snapshot` writes a JSON record containing configuration, camera state, counters, timing values and sparse-memory statistics. `ARCHITECTURE_LOCK.json` stores the canonical invariants and the SHA-256 digest of the runtime HTML. The Git commit remains the authoritative provenance record.

## 10. Failure containment

- Dense `2048³` allocation is prohibited.
- GPU instances and resident page metadata are bounded.
- Large frame deltas are clamped.
- Simulation catch-up is capped.
- WebGL context loss pauses execution and is surfaced to the user.
- Media recording support is checked before capture begins.
- Missing CDN or WebGL capability produces a visible boot error.

See [`PERFORMANCE_CONTRACT.md`](PERFORMANCE_CONTRACT.md) for measurement targets and benchmark procedure.
