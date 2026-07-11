# 3D Virtual AI Interactive Interface

Browser-native experimental visual-computing engines for sparse 3D state, auto-encoding/decoding, electromagnetic-field metaphors and deterministic virtual-machine control loops.

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

Open `http://localhost:8000/voxel-video-machine.html`.

The runtime currently loads pinned Three.js `r134` scripts from public CDNs, so first load requires internet access.

## Controls

- **Space**: play or pause
- **P**: inject a residual pulse
- **R**: start or stop video recording
- Orbit controls activate when automatic orbit is disabled
- **Export Snapshot** downloads runtime settings, state, memory counters and performance telemetry

## Operational loop

```text
Observe → Encode₃ᴅ → Predict → Page → Animate → Simulate
        → ProjectΛ → Render → Capture → CommitΩ → Advance
```

## Validation

```bash
node scripts/validate.mjs
```

The validator parses the inline runtime JavaScript, checks architectural invariants and verifies `voxel-video-machine.html` against `ARCHITECTURE_LOCK.json`.

## Engines

- `voxel-video-machine.html` — sparse 2048³ virtual video animation machine
- `em-rom-engine.html` — original EM-ROM cognitive byte-agent visualization

## Documentation

- [`docs/VOXEL_VIDEO_MACHINE.md`](docs/VOXEL_VIDEO_MACHINE.md)
- [`docs/PERFORMANCE_CONTRACT.md`](docs/PERFORMANCE_CONTRACT.md)

## Architectural distinction

`2048³` is the virtual world address space, not the number of cubes rendered each frame. The browser materializes only a bounded working set and projects it into a conventional 2D frame.
