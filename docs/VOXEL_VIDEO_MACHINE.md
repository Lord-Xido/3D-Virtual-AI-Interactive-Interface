# 2048³ Virtual Video Animation Machine

## 1. Address space

The virtual world is

\[
\mathcal{V}_t[x,y,z], \qquad 0 \le x,y,z < 2048.
\]

Because `2048 = 2¹¹`, each axis uses 11 bits and the complete coordinate uses 33 bits:

\[
N = 2048^3 = 2^{33} = 8,589,934,592.
\]

The row-major address is

\[
A(x,y,z)=x+2048y+2048^2z.
\]

## 2. Sparse brick memory

The world is divided into `32³` bricks:

- local voxel coordinate: 5 bits per axis;
- brick coordinate: 6 bits per axis;
- bricks per axis: `2048 / 32 = 64`;
- virtual bricks: `64³ = 262,144`.

Only active bricks are resident. A page table maps virtual brick IDs to runtime state. The demonstration stores compact page metadata and renders a bounded sample through a GPU instance pool rather than allocating the dense volume.

## 3. Runtime state

\[
\Sigma_t=(V_t,Z_t,M_t,P_t,E_t,\Omega_t,G_t,C_t,L_t,R_t,A_t,Q_t).
\]

- `V`: virtual voxel field;
- `Z`: encoded/latent state;
- `M`: material and physical state;
- `P`: predicted state and page demand;
- `E`: residual;
- `Ω`: persistent correction memory;
- `G`: scene geometry and transforms;
- `C`: camera;
- `L`: lighting;
- `R`: rendered frame;
- `A`: audio/capture synchronization;
- `Q`: scheduler and page queue.

## 4. Three-bit semantic state

| Bits | Meaning |
|---|---|
| `000` | empty |
| `001` | geometry |
| `010` | emissive |
| `011` | volumetric medium |
| `100` | dynamic/particle |
| `101` | latent/AI-controlled |
| `110` | control/bytecode |
| `111` | residual/compound |

The 3-bit value is a semantic selector. Rich properties remain in optional channels or procedural functions.

## 5. Frame transition

\[
\Sigma_{t+\Delta t}=\Pi_\Lambda\left[
\Sigma_t+\mathcal{F}_{animation}+\mathcal{F}_{simulation}+P_t-E_t+\Omega_t
\right].
\]

The browser runtime executes:

1. read clock and controls;
2. evaluate the procedural field program;
3. predict camera-centred brick demand;
4. page in useful virtual regions;
5. encode voxel semantics;
6. evaluate prediction residuals;
7. update `Ω`;
8. write an instanced GPU working set;
9. project through the camera;
10. render the frame;
11. optionally encode the canvas stream into WebM;
12. advance time.

## 6. Rendering

The current implementation uses `THREE.InstancedMesh`, which avoids one draw call per voxel. The virtual field is sampled into at most 12,000 visible instances. This is an operational visualization of the sparse architecture, not a dense 8.59-billion-voxel renderer.

A production implementation can replace this layer with WebGPU compute shaders, sparse 3D textures, octree traversal, ray marching, meshlet extraction, or neural reconstruction while preserving the same virtual-memory contract.

## 7. Video output

The 3D world is projected into a conventional 2D frame:

\[
R_n \in \mathbb{R}^{W\times H\times4}.
\]

The browser captures the WebGL canvas using `captureStream(fps)` and writes a VP9, VP8, or generic WebM stream through `MediaRecorder`, depending on browser support.

## 8. Invariants

- Never allocate the dense `2048³` state by default.
- Virtual extent is independent of resident memory.
- Paging, culling and level of detail precede rendering.
- The frame clock is the single source of animation time.
- `Ω` stores correction history, not an unverifiable claim of complete reality.
- Capture records rendered output; it does not serialize the entire virtual world.
