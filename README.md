# 3D Virtual AI Interactive Interface

[![Validate architecture](https://github.com/Lord-Xido/3D-Virtual-AI-Interactive-Interface/actions/workflows/validate.yml/badge.svg)](https://github.com/Lord-Xido/3D-Virtual-AI-Interactive-Interface/actions/workflows/validate.yml)
[![Status: Experimental](https://img.shields.io/badge/status-experimental-orange.svg)](PROJECT_STATUS.md)

**Browser-native and native experimental visual-computing engines for sparse 3D state, deterministic simulation, auto-encoding/decoding and auditable virtual-machine control loops.**

This repository is the interactive demonstration layer of the broader Jarvis-X research programme. It prioritizes runnable artifacts, bounded resource use, measurable telemetry and explicit separation between visual simulation and authoritative computation.

> A large virtual coordinate space is not a dense allocation. A deterministic animation is not evidence of deployed autonomous intelligence. See [Project Status](PROJECT_STATUS.md) for the implemented capability boundary.

## Reference engines on `main`

### 2048³ virtual video machine

`voxel-video-machine.html` exposes a **2048 × 2048 × 2048** virtual voxel address space without allocating all 8,589,934,592 voxels.

The engine provides:

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

### DM–vΩΞ⁺ bounded meta-evolution engine

`dm-vomegaxi-qsol-engine.html` is a deterministic visual and numerical demonstration of an inward update loop. It provides:

- an `8 × 8 × 8` latent credit memory;
- a bounded resident sample field selected for desktop, compact and reduced-motion modes;
- fixed-seed state and target generation;
- objective-grounded blockwise evolutionary credit;
- centered multiplicative update gates;
- transactional candidate commit or rollback;
- explicit fixed-point projection;
- a desktop numerical-state footprint of 192 KiB;
- adaptive pixel ratio and bounded fixed-step rendering;
- visibility, resize, reduced-motion and WebGL context-loss handling;
- JSON experiment snapshot export;
- focused executable validation in CI.

The displayed `10^9` QSOL field is a logical visualization label, not a billion allocated tensor cores. See [DM–vΩΞ⁺ QSOL Engine](docs/DM_VOMEGAXI_QSOL_ENGINE.md) for the equations, memory accounting and authority boundary.

## Run

Serve the repository over localhost so browser security policies and asset loading behave consistently:

```bash
python -m http.server 8000
```

Open either reference engine:

```text
http://localhost:8000/voxel-video-machine.html
http://localhost:8000/dm-vomegaxi-qsol-engine.html
```

The merged browser runtimes load pinned Three.js scripts from public CDNs, so their first load requires network access.

## Controls

### Voxel video machine

| Control | Action |
|---|---|
| `Space` | Play or pause |
| `P` | Inject a residual pulse |
| `R` | Start or stop video recording |
| Orbit controls | Enabled when automatic orbit is disabled |
| Export Snapshot | Save settings, state, memory counters and performance telemetry |

### DM–vΩΞ⁺ engine

| Control | Action |
|---|---|
| Description operator `Φ` | Scale the bounded gradient step |
| Latent lens `Δcontext` | Scale centered latent-memory gates |
| Bounded automatic evolution | Execute objective-gated updates periodically |
| Focus Lens | Restore full latent-gate focus |
| Apply DM | Execute one candidate transaction |
| Lock `Ψ*` | Project the sample field onto its deterministic target |
| Pause Render | Pause or resume animation |
| Export Snapshot | Download numerical state, latent memory and provenance |

## Operational loops

### Sparse voxel renderer

```text
Observe → Encode₃ᴅ → Predict → Page → Animate → Simulate
        → ProjectΛ → Render → Capture → CommitΩ → Advance
```

### Inward meta-evolution demonstration

```text
Differentiate → Attribute block credit → Integrate 8³ memory
              → Generate candidate → Evaluate objective
              → Commit or rollback → Render telemetry
```

The browser materializes only bounded working sets and projects them into conventional 2D frames.

## Validate

```bash
node scripts/validate.mjs
node scripts/validate-dm-vomegaxi.mjs
```

The validators:

- parse inline runtime JavaScript;
- check fixed-step and resource invariants;
- verify bounded virtual and resident geometry;
- compare `voxel-video-machine.html` against `ARCHITECTURE_LOCK.json`;
- test deterministic meta-evolution initialization;
- verify monotonic objective descent and fixed-point projection;
- enforce the `8³` latent contract and sub-1-MiB numerical-state bound.

## Engine portfolio

| Engine | Location | Status |
|---|---|---|
| 2048³ virtual video machine | `voxel-video-machine.html` | Merged reference demonstration |
| DM–vΩΞ⁺ bounded meta-evolution | `dm-vomegaxi-qsol-engine.html` | Merged reference demonstration |
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
- [`docs/DM_VOMEGAXI_QSOL_ENGINE.md`](docs/DM_VOMEGAXI_QSOL_ENGINE.md)
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
- artificial general intelligence;
- an external language model unless one is explicitly connected;
- unrestricted source-code self-modification;
- dense realization of advertised virtual spaces;
- physical quantum or tensor-core hardware from symbolic QSOL labels;
- production browser, network or model security;
- direct crawling of arbitrary websites from local demonstrations;
- audio generation merely because a waveform is rendered.

## Contributing and security

Read [`CONTRIBUTING.md`](CONTRIBUTING.md) before proposing a new engine. Report vulnerabilities according to [`SECURITY.md`](SECURITY.md); do not publish exploit details in a public issue.
