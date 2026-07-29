# DM–vΩΞ⁺ Permeated 3D QSOL Engine

## Classification

**Deterministic browser reference demonstration.**

`dm-vomegaxi-qsol-engine.html` visualizes a bounded inward meta-evolution loop. It combines a resident sample field, an `8 × 8 × 8` latent credit memory, objective-gated state transitions, five symbolic stack layers and a WebGL instanced renderer.

The name and symbols are a research vocabulary. They do not establish artificial general intelligence, consciousness, physical quantum hardware or a billion resident tensor cores.

## Authority boundary

The engine is an isolated visual and numerical demonstration. It does not mutate or control authoritative Jarvis-X VM state. Snapshot files are experiment records, not executable checkpoints for the canonical Jarvis-X runtime.

The displayed `10^9` QSOL field is a **logical visualization label**. The browser materializes:

- 10,240 state samples on a normal desktop;
- 3,840 samples on compact displays;
- 2,560 samples when reduced motion is requested;
- 512 latent memory cells in all modes.

## Objective-grounded recurrence

Let the resident sample state be

```text
x_t ∈ R^N
```

with a deterministic synthetic target

```text
y ∈ R^N.
```

The demonstrator minimizes

```text
J(x_t) = (1 / 2N) Σ_i (x_t[i] - y[i])².
```

Each sample maps to one latent cell:

```text
b(i) = i mod 512.
```

### 1. Temporal motion

```text
Δ_t[i] = x_t[i] - x_(t-1)[i]
```

### 2. Blockwise evolutionary credit

For each latent block `b`:

```text
n_t[b] = Σ_(i:b(i)=b) -(x_t[i] - y[i]) Δ_t[i]
d_t[b] = Σ_(i:b(i)=b) |(x_t[i] - y[i]) Δ_t[i]|
c_t[b] = n_t[b] / (d_t[b] + ε)
```

This measures whether the previous motion aligned with descent of the defined objective.

### 3. Bounded latent memory

```text
h_t[b] = clip(λ h_(t-1)[b] + η c_t[b], -1, 1)
```

The implementation uses:

```text
λ = 0.90
η = 0.22
```

The centered gate is

```text
g_t[b] = tanh((h_t[b] - mean(h_t)) f)
```

where `f ∈ [0.1, 1]` is the latent-lens control.

### 4. Candidate state

```text
x_candidate[i]
  = clip(
      x_t[i]
      - γ Φ (x_t[i] - y[i])
      + μ Δ_t[i] exp(α g_t[b(i)]),
      -L,
      L
    )
```

The browser controls `Φ`, while all other constants remain bounded in the runtime configuration.

### 5. Transactional descent gate

The candidate commits only when

```text
J(x_candidate)
  ≤ J(x_t) - ρ mean((x_candidate - x_t)²).
```

Otherwise the state rolls back and latent memory is damped:

```text
h_t ← 0.65 h_t.
```

The event console exposes each commit, rollback and convergence decision.

### 6. Fixed-point projection

The explicit fixed-point control sets

```text
x* = y
h* = 0
J(x*) = 0.
```

This is a projection onto the demonstrator's synthetic target, not evidence that an external engineering or scientific objective has been solved.

## Numerical memory contract

For the desktop resident set `N = 10,240`, the typed-array numerical state is:

| Object | Representation | Bytes |
|---|---:|---:|
| current, previous, candidate and target state | `4 × N × FP32` | 163,840 |
| latent-block indices | `N × UINT16` | 20,480 |
| memory and centered gates | `2 × 512 × FP32` | 4,096 |
| credit numerator and denominator | `2 × 512 × FP64` | 8,192 |
| **Total** |  | **196,608 bytes (192 KiB)** |

The numerical state is therefore below 1 MiB. GPU geometry, instance transforms, JavaScript objects, browser internals and Three.js resources are additional resident memory and are not included in this numerical-state figure.

## Determinism contract

For the same runtime version, sample count and controls:

- initialization uses fixed-seed `XorShift32`;
- the target field is generated from deterministic trigonometric functions;
- the numerical update contains no wall-clock input;
- the same accepted iteration sequence produces the same typed-array values within one JavaScript numerical environment.

Browser scheduling, GPU timing, frame rate and floating-point implementation details remain environmental. Cross-browser bit identity is not claimed.

## Rendering and lifecycle controls

The renderer provides:

- five symbolic `Ψ–Φ–Λ–Ω–Θ` shells;
- bounded instanced node counts selected by display and reduced-motion state;
- fixed-step simulation with a maximum of three catch-up steps;
- accumulator reset to prevent a spiral of death;
- adaptive device-pixel ratio based on measured frame rate;
- pause while the page is hidden;
- WebGL context-loss handling;
- explicit CDN-load failure reporting;
- responsive safe-area-aware controls;
- keyboard focus indicators and ARIA live telemetry;
- JSON snapshot export.

## Run

```bash
python -m http.server 8000
```

Open:

```text
http://localhost:8000/dm-vomegaxi-qsol-engine.html
```

The first load requires network access for pinned Three.js `r134` and OrbitControls assets.

## Validate

```bash
node scripts/validate-dm-vomegaxi.mjs
```

The validator checks:

- inline JavaScript syntax;
- fixed-step and resource-bound constants;
- deterministic initialization;
- monotonic objective descent on the reference fixture;
- fixed-point projection;
- the `8³` latent-memory contract;
- a sub-1-MiB desktop numerical-state footprint;
- rollback, browser lifecycle, reduced-motion, context-loss and snapshot hooks.
