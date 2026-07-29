# 3D Virtual AI Interactive Interface

[![Validate architecture](https://github.com/Lord-Xido/3D-Virtual-AI-Interactive-Interface/actions/workflows/validate.yml/badge.svg)](https://github.com/Lord-Xido/3D-Virtual-AI-Interactive-Interface/actions/workflows/validate.yml)
[![Status: Experimental](https://img.shields.io/badge/status-experimental-orange.svg)](PROJECT_STATUS.md)

**Browser-native and native experimental visual-computing engines for sparse 3D state, deterministic simulation, auto-encoding/decoding and auditable virtual-machine control loops.**

This repository is the interactive demonstration layer of the broader Jarvis-X research programme. It prioritizes runnable artifacts, bounded resource use, measurable telemetry and explicit separation between visual simulation and authoritative computation.

> A large virtual coordinate space is not a dense allocation. A deterministic animation is not evidence of deployed autonomous intelligence. See [Project Status](PROJECT_STATUS.md) for the implemented capability boundary.

## Canonical engine on `main`

### 2048³ virtual video machine

`voxel-video-machine.html` exposes a **2048 × 2048 × 2048** virtual voxel address space without allocating all 8,589,934,592 voxels.

The current merged engine provides:

- deterministic seeded field generation;
- fixed-step simulation with an accumulator safety guard;
- sparse `32³` brick paging across a `64³` virtual brick lattice;
- velocity-led page prediction and score-based eviction;
- bounded GPU instancing from 512 to 12,000 visible voxels;
- adaptive frame-budget control for 30 or 60 FPS targets;
- page hits, faults, evictions, residual RMS and phase telemetry;
- WebGL context-loss handling;
- browser-native WebM recording with capability checks;
- JSON runtime snapshots for provenance and performance analysis;
- a SHA-256 architecture lock validated in CI.

## Run

Serve the repository over localhost so browser security policies and asset loading behave consistently:

```bash
python -m http.server 8000
```

Open:

```text
http://localhost:8000/voxel-video-machine.html
```

The merged runtime loads pinned Three.js `r134` scripts from public CDNs, so its first load requires network access.

## Controls

| Control | Action |
|---|---|
| `Space` | Play or pause |
| `P` | Inject a residual pulse |
| `R` | Start or stop video recording |
| Orbit controls | Enabled when automatic orbit is disabled |
| Export Snapshot | Save settings, state, memory counters and performance telemetry |

## Operational loop

```text
Observe → Encode₃ᴅ → Predict → Page → Animate → Simulate
        → ProjectΛ → Render → Capture → CommitΩ → Advance
```

The browser materializes only a bounded working set and projects it into a conventional 2D frame.

## Validate

```bash
node scripts/validate.mjs
```

The validator:

- parses the inline runtime JavaScript;
- checks architecture invariants;
- verifies bounded virtual and resident geometry;
- compares `voxel-video-machine.html` against `ARCHITECTURE_LOCK.json`.

## Engine portfolio

| Engine | Location | Status |
|---|---|---|
| 2048³ virtual video machine | `voxel-video-machine.html` | Merged reference demonstration |
| EM-ROM engine | `em-rom-engine.html` | Legacy demonstration |
| HBP-VM humanoid engine | PR #3 | Draft integration candidate |
| World Wide Web 3D VANN | PR #4 | Draft browser demonstration |
| Autopoietic multimodal chat engine | PR #5 | Draft browser demonstration |

Draft pull requests are not canonical until validation, review and merge are complete.

## Repository standards

Every merged engine should include:

- a deterministic seed or documented stochastic protocol;
- a fixed or bounded simulation timestep;
- explicit virtual-versus-resident resource accounting;
- responsive rendering and context-loss behavior;
- capability checks for browser APIs;
- snapshot or trace export;
- executable validation;
- an implemented-versus-proposed boundary;
- no performance claim without measured hardware and workload details.

## Architecture and performance documents

- [`docs/VOXEL_VIDEO_MACHINE.md`](docs/VOXEL_VIDEO_MACHINE.md)
- [`docs/PERFORMANCE_CONTRACT.md`](docs/PERFORMANCE_CONTRACT.md)
- [`PROJECT_STATUS.md`](PROJECT_STATUS.md)
- [`CONTRIBUTING.md`](CONTRIBUTING.md)
- [`SECURITY.md`](SECURITY.md)

## Relationship to Jarvis-X

This repository hosts interactive renderers and demonstrations. The canonical bytecode VM, provenance mechanisms and research specifications live in [`Lord-Xido/Jarvis-X`](https://github.com/Lord-Xido/Jarvis-X).

Visual state must not silently become authoritative Jarvis-X state. Integration requires an explicit, validated data contract.

## Research boundary

The interfaces in this repository do not claim:

- consciousness or subjective experience;
- an external language model unless one is explicitly connected;
- unrestricted source-code self-modification;
- dense realization of the advertised virtual spaces;
- production browser, network or model security;
- direct crawling of arbitrary websites from local demonstrations;
- audio generation merely because a waveform is rendered.

## Contributing and security

Read [`CONTRIBUTING.md`](CONTRIBUTING.md) before proposing a new engine. Report vulnerabilities according to [`SECURITY.md`](SECURITY.md); do not publish exploit details in a public issue.
