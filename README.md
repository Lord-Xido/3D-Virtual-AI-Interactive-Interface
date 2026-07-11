# 3D Virtual AI Interactive Interface

Browser-native experimental visual computing engines for sparse 3D state, auto-encoding/decoding, electromagnetic-field metaphors, and deterministic virtual-machine control loops.

## Engines

### `voxel-video-machine.html`

A runnable **2048 × 2048 × 2048 virtual voxel video animation machine** implemented with Three.js and plain JavaScript.

The engine exposes a 33-bit virtual coordinate space without allocating all 8,589,934,592 voxels. It uses:

- `32³` voxel bricks arranged as a `64³` virtual brick lattice;
- sparse virtual-to-resident page mapping;
- a bounded GPU `InstancedMesh` working set;
- 3-bit semantic voxel classes;
- procedural field programs;
- predictive camera-centred paging;
- residual memory `Ω`;
- animation, simulation, camera and render loops;
- direct WebM recording through `canvas.captureStream()` and `MediaRecorder`.

Open the file in a modern browser. Internet access is required for the Three.js CDN scripts.

### `em-rom-engine.html`

The original EM-ROM cognitive byte-agent visualization.

## Operational loop

```text
Observe → Encode₃ᴅ → Predict → Page → Animate → Simulate
        → ProjectΛ → Render → Capture → CommitΩ → Advance
```

## Important distinction

`2048³` is the virtual world address space, not the number of cubes rendered every frame. The browser demo materializes only the currently useful working set and projects it into a conventional 2D video frame.

See [`docs/VOXEL_VIDEO_MACHINE.md`](docs/VOXEL_VIDEO_MACHINE.md) for the full runtime model.
