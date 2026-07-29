# Contributing

Contributions should improve one bounded visual-computing engine or one shared validation mechanism at a time.

## Before coding

- Search existing pull requests for overlapping engines.
- State whether the proposal is a reference demonstration, integration candidate or supporting document.
- Define virtual extent separately from resident CPU/GPU work.
- Identify browser, native and network dependencies.
- State what the interface does not claim.

## Run the merged validator

```bash
node scripts/validate.mjs
```

New engines should include a dedicated validator or focused automated test. A visual inspection alone is insufficient.

## Required runtime properties

A browser engine should address:

- deterministic or explicitly seeded initialization;
- fixed-step or bounded-delta simulation;
- viewport and device-pixel-ratio changes;
- WebGL context loss where applicable;
- browser API capability checks;
- bounded history, geometry and resident state;
- error handling that produces a visible diagnostic instead of a black screen;
- snapshot or trace export;
- accessible controls and labels.

A native engine should additionally provide:

- exact build dependencies;
- a headless or syntax-level validation path when possible;
- platform and graphics-profile assumptions;
- resource cleanup and failure behavior.

## Performance claims

Provide:

- browser and version;
- operating system and hardware;
- viewport and device-pixel ratio;
- virtual extent and actual resident/rendered counts;
- warm-up duration and sample count;
- frame-time percentiles, not only a peak FPS;
- memory and quality settings;
- comparison against a simpler baseline.

## Pull requests

Describe:

- the operational loop;
- authoritative versus visual state;
- validation commands and results;
- capability boundaries;
- external scripts or assets;
- merge dependencies or superseded work.

Do not combine unrelated engines in one PR merely because they share 3D or neural terminology.

## Security

Do not commit tokens, personal data, private media or unreviewed executable dependencies. Follow `SECURITY.md` for vulnerability reports.
