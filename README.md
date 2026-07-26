# 3D Virtual AI Interactive Interface

Browser-native experimental visual-computing engines for sparse 3D state, auto-encoding/decoding, electromagnetic-field metaphors and deterministic virtual-machine control loops.

## Autopoietic multimodal chat engine

`autopoietic-multimodal-chat-engine.html` is a deterministic browser simulation built around an instantiated `8 × 8 × 8` voxel lattice. Each submitted message:

```text
Input text → deterministic hash → core voxel perturbation
→ compile 3³ region into 60 latent values
→ interpret and propagate → calculate residual RMS
→ apply bounded correction → normalize modal state
→ render text, visual, waveform, code and data outputs
```

The runtime includes responsive Three.js rendering, deterministic seeded initialization, live bytecode telemetry and JSON snapshot export. It is explicitly scoped as a bounded visual simulation rather than a deployed self-rewriting AI system.

## 2048³ virtual video machine

`voxel-video-machine.html` exposes a **2048 × 2048 × 2048** virtual voxel address space without allocating all 8,589,934,592 voxels. Version 2 adds:

- deterministic seeded field generation;
- fixed-step simulation with an accumulator safety guard;
- sparse `32³` brick paging across a `64³` virtual lattice;
- velocity-led page prediction and score-based eviction;
- bounded GPU instancing from 512 to 12,000 visible voxels;
- adaptive frame-budget control for 30 or 60 FPS targets;
- page hits, faults, evictions, residual RMS and phase timing telemetry;
- WebGL context-loss handling;
- browser-native WebM recording with capability checks;
- JSON runtime snapshots for provenance and performance analysis;
- a SHA-256 architecture lock validated in CI.

## Run

Serve the repository over localhost so browser security policies and CDN loading behave consistently:

```bash
python -m http.server 8000
```

Open a runtime:

```text
http://localhost:8000/autopoietic-multimodal-chat-engine.html
http://localhost:8000/voxel-video-machine.html
```

The runtimes currently load pinned Three.js scripts from public CDNs, so first load requires internet access.

## Controls

### Autopoietic multimodal chat engine

- Enter a message and select **Send & Evolve**
- Enter submits; Shift+Enter creates a new line
- Switch between Visual, Audio, Code and Data tabs
- **Export Runtime Snapshot** downloads the current bounded engine state

### 2048³ virtual video machine

- **Space**: play or pause
- **P**: inject a residual pulse
- **R**: start or stop video recording
- Orbit controls activate when automatic orbit is disabled
- **Export Snapshot** downloads runtime settings, state, memory counters and performance telemetry

## Validation

```bash
node scripts/validate.mjs
node scripts/validate-autopoietic.mjs
```

The validators parse the inline runtime JavaScript and check their architectural invariants. The voxel machine is additionally verified against `ARCHITECTURE_LOCK.json`.

## Engines

- `autopoietic-multimodal-chat-engine.html` — deterministic 8³ multimodal voxel feedback simulation
- `voxel-video-machine.html` — sparse 2048³ virtual video animation machine
- `em-rom-engine.html` — original EM-ROM cognitive byte-agent visualization

## Documentation

- [`docs/AUTOPOIETIC_MULTIMODAL_CHAT_ENGINE.md`](docs/AUTOPOIETIC_MULTIMODAL_CHAT_ENGINE.md)
- [`docs/VOXEL_VIDEO_MACHINE.md`](docs/VOXEL_VIDEO_MACHINE.md)
- [`docs/PERFORMANCE_CONTRACT.md`](docs/PERFORMANCE_CONTRACT.md)

## Architectural distinction

The virtual field is an address and coordination space, not a promise to allocate every voxel. Each runtime materializes only a bounded working set and projects it into a conventional 2D frame.
