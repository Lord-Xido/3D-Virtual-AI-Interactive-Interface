# Performance and Determinism Contract

This document defines targets and invariants for the browser demonstration. Targets are budgets to measure against, not claims about every device.

## Timing model

The renderer uses `requestAnimationFrame`, while simulation advances through a fixed accumulator:

\[
\Delta t_{sim}=\frac{1}{60}\text{ s},\qquad N_{substeps}\le 4.
\]

A long display stall cannot trigger an unbounded simulation catch-up. If four substeps are insufficient, accumulated time is dropped and journalled. This prevents the spiral-of-death failure mode.

## Primary budgets

| Metric | Healthy target | Reflex action |
|---|---:|---|
| 60 FPS frame time | ≤ 16.67 ms EWMA | reduce effective voxel budget |
| 30 FPS frame time | ≤ 33.33 ms EWMA | reduce effective voxel budget |
| GPU instances | 512–12,000 | hard clamp |
| Resident pages | ≤ 900 | score-based eviction |
| Simulation substeps | ≤ 4/display frame | drop excess accumulated time |
| Page hit rate | ≥ 90% after warm-up | expand prediction lead or retain hot pages |

## Adaptive quality

The requested voxel budget is the user's upper bound. The effective budget is controlled by `AdaptiveQualityController`:

\[
B_{t+1}=\begin{cases}
\max(B_{min},0.88B_t), & \bar T_t>1.12T_{target}\\
\min(B_{request},B_t+256), & \bar T_t<0.72T_{target}\\
B_t, & \text{otherwise.}
\end{cases}
\]

A cooldown prevents quality oscillation. Disabling adaptive mode makes the effective budget equal the requested budget.

## Paging metrics

The pager records total and per-frame requests, hits, faults and evictions. Pages are evicted by a score combining demand priority, field heat and age:

\[
S(page)=8p+2|h|-0.04a.
\]

Low-score pages leave first. The camera predictor includes a velocity lead, so paging follows the expected camera trajectory rather than only the current pose.

## Determinism boundary

Given the same runtime version, controls, fixed-step sequence and camera trajectory, field occupancy is generated from a seeded integer hash rather than `Math.random`. Browser and GPU floating-point implementations may still create small visual differences. Video codec output is not bit-identical across browsers.

## Failure containment

- Dense `2048³` allocation is prohibited.
- GPU instances and resident pages are hard-bounded.
- WebGL context loss pauses simulation and is surfaced in the UI.
- Recorder capability is detected before capture.
- Long-frame accumulation is capped.
- Runtime snapshots export settings, state, memory counters and timing telemetry.

## Measurement protocol

For a meaningful baseline:

1. Serve the repository over localhost.
2. Select 60 FPS, adaptive quality on, budget 12,000.
3. Run each field program for at least 2,000 display frames.
4. Export a snapshot at the end of each run.
5. Compare frame-time EWMA, effective budget, page hit rate, residual RMS, faults and evictions.
