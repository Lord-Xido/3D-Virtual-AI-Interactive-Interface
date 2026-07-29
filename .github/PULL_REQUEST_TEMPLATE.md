## Purpose

## Engine classification

- [ ] Merged-reference improvement
- [ ] Draft integration candidate
- [ ] New demonstration
- [ ] Validator or infrastructure change
- [ ] Documentation only

## What changed

## Operational loop

```text
Input → State transition → Validation → Render → Telemetry → Advance
```

## Virtual versus resident scale

<!-- State virtual dimensions, resident data, rendered instances and memory bounds separately. -->

## Determinism and timing

<!-- Seed, fixed timestep, accumulator limits, browser scheduling and environmental inputs. -->

## Browser or native compatibility

<!-- Dependencies, APIs, fallbacks, resize, context loss, build assumptions. -->

## Validation

```text
node scripts/validate.mjs
```

<!-- Add engine-specific commands and results. -->

## Performance evidence

<!-- Hardware, browser, viewport, workload, samples, frame-time percentiles and memory. -->

## Implemented boundary

<!-- What the engine demonstrates and what it does not claim. -->

## Security and privacy

<!-- External scripts, user media, exports, model loading or network behavior. -->

## Checklist

- [ ] Resident work is bounded
- [ ] Invalid or unavailable capabilities fail visibly
- [ ] Controls are accessible
- [ ] Validation is automated
- [ ] Snapshot or telemetry behavior is documented
- [ ] No secrets or private media are included
- [ ] Project status and README are updated where needed
