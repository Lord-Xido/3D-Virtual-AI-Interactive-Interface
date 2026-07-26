# Autopoietic Multimodal Chat Engine

`autopoietic-multimodal-chat-engine.html` is a browser-native visual simulation of a bounded multimodal feedback engine operating on an `8 × 8 × 8` voxel lattice.

## Scope

The runtime is an experimental deterministic simulation. It does not claim to be a deployed autonomous general intelligence, does not fetch external model responses, and does not rewrite its own executable source.

Each message is converted into a deterministic numeric perturbation using a fixed hash and seed. The engine then runs five local update cycles and synthesizes five visible outputs:

- text response;
- visual pattern;
- audio waveform visualization;
- generated code sample;
- JSON state report.

## Operational loop

```text
Input text
→ deterministic message hash
→ core-voxel perturbation
→ compile 3 × 3 × 3 region into 60 values
→ interpret and propagate to six neighbours
→ recompile and calculate residual RMS
→ bounded correction of semantic bias
→ modal-channel normalization
→ update 3D visualization
→ render text, visual, waveform, code and data outputs
```

## Runtime topology

- Lattice: `8³ = 512` instantiated voxels
- Neighbourhood: six orthogonal neighbours
- Modal state: text, visual, audio and code channels
- Compiled region: `3 × 3 × 3`
- Latent bytecode width: 60 floating-point values
- Cycles per message: 5
- Default learning rate: `0.05`
- Deterministic seed: `0x4D4F4147`

## Main classes

- `RNG` — seeded xorshift32 random-number generator
- `MultimodalVoxel` — local activation, semantic bias, tension and modal state
- `MultimodalLattice` — 3D indexing, bounded correction and Three.js meshes
- `MultimodalCompiler` — local region-to-bytecode projection
- `MultimodalInterpreter` — bytecode-to-state mutation and neighbour signalling
- `MultimodalFeedback` — residual calculation and tension propagation
- `MultimodalGenerator` — deterministic output synthesis
- `AutopoieticChatEngine` — message processing and runtime orchestration

## Run

```bash
python -m http.server 8000
```

Open:

```text
http://localhost:8000/autopoietic-multimodal-chat-engine.html
```

The runtime loads Three.js r128 from cdnjs, so the initial page load requires internet access.

## Controls

- Enter a message and select **Send & Evolve**.
- Press Enter to submit; Shift+Enter creates a new line.
- Switch between Visual, Audio, Code and Data tabs.
- Select **Export Runtime Snapshot** to download the current bounded engine state as JSON.

## Validation

```bash
node scripts/validate-autopoietic.mjs
```

The validator parses the inline JavaScript and checks the deterministic RNG, bounded lattice, compiler/interpreter/feedback classes, responsive rendering, snapshot export and simulation scope marker.
