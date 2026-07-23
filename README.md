# 3D Virtual AI Interactive Interface

Browser-native experimental visual-computing engines for sparse 3D state, auto-encoding/decoding, electromagnetic-field metaphors and deterministic virtual-machine control loops.

## World Wide Web virtual 3D ANN

`world-wide-web-vann.html` is a dependency-free, browser-native neural runtime that maps bounded Web-resource packets into a sparse `64³` virtual field and executes a real auto-encoding/decoding learning loop:

```text
Web graph → 12 input features → 8 encoder neurons → 4 latent neurons
          → 8 decoder neurons → 12 reconstructed features
```

Its operational cycle is transactional:

```text
Observe → Validate → Parse → Encode modality → Map 3D → Update graph
→ Activate sparse tile → Encode inward → Evolve latent → Decode outward
→ Calculate residual → Update Ω candidate → Project Λ
→ Buffer transaction → Commit / rollback
```

The runtime performs gradient descent in JavaScript, snapshots weights before mutation, commits accepted learning updates, and restores the previous weights when the Λ gate rejects a transaction.

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

Serve the repository over localhost:

```bash
python -m http.server 8000
```

Open either runtime:

```text
http://localhost:8000/world-wide-web-vann.html
http://localhost:8000/voxel-video-machine.html
```

`world-wide-web-vann.html` has no external JavaScript dependencies. `voxel-video-machine.html` loads pinned Three.js `r134` scripts from public CDNs, so its first load requires internet access.

## Controls

### World Wide Web VANN

- **Pause / Resume**: suspend or continue staged execution
- **Single Cycle**: execute one complete 15-stage transaction
- **Inject Web Node**: encode the supplied URL and representation payload
- **Residual Pulse**: perturb the persistent Ω correction field
- **Export Snapshot**: download learning and transaction telemetry
- Drag to rotate, scroll to zoom, `Space` to pause, `S` for one cycle and `P` for an Ω pulse

### 2048³ virtual video machine

- **Space**: play or pause
- **P**: inject a residual pulse
- **R**: start or stop video recording
- Orbit controls activate when automatic orbit is disabled
- **Export Snapshot** downloads runtime settings, state, memory counters and performance telemetry

## Validation

```bash
node scripts/validate.mjs
node scripts/validate-www-vann.mjs
```

The validators parse the inline runtime JavaScript and check their respective architectural invariants. The voxel machine is additionally verified against `ARCHITECTURE_LOCK.json`.

## Engines

- `world-wide-web-vann.html` — graph-connected, auto-encoding/decoding 3D ANN with Ω learning and Λ rollback
- `voxel-video-machine.html` — sparse 2048³ virtual video animation machine
- `em-rom-engine.html` — original EM-ROM cognitive byte-agent visualization

## Documentation

- [`docs/WORLD_WIDE_WEB_VANN.md`](docs/WORLD_WIDE_WEB_VANN.md)
- [`docs/VOXEL_VIDEO_MACHINE.md`](docs/VOXEL_VIDEO_MACHINE.md)
- [`docs/PERFORMANCE_CONTRACT.md`](docs/PERFORMANCE_CONTRACT.md)

## Architectural distinction

The virtual field is an address and coordination space, not a promise to allocate every voxel. Each runtime materializes only a bounded working set and projects it into a conventional 2D display while preserving explicit 3D coordinates and state transitions.
