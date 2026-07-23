# World Wide Web Virtual Auto-Encoding/Decoding 3D ANN

`world-wide-web-vann.html` is a browser-native, deterministic reference runtime that turns the World Wide Web VANN architecture into an executable simulation.

## Operational contract

The runtime implements the complete transactional cycle:

```text
Observe → Validate → Parse → Encode modality → Map 3D → Update graph
→ Activate sparse tile → Encode inward → Evolve latent → Decode outward
→ Calculate residual → Update Ω candidate → Project Λ
→ Buffer transaction → Commit / rollback
```

## Neural topology

The executable autoencoder is:

```text
12 input features → 8 encoder neurons → 4 latent neurons
                  → 8 decoder neurons → 12 reconstructed features
```

Every Web resource is represented as a bounded 12-element vector containing content density, graph relations, vocabulary diversity, provenance, permission, modality, domain, semantic coordinates, temporal phase and graph centrality.

The browser positions resources inside a sparse `64 × 64 × 64` virtual field. The dense ANN is drawn in a 3D inward/outward geometry while non-local resource edges remain an explicit graph overlay.

## Learning semantics

For resource vector `x`, graph-conditioned target `y`, encoder `E`, and decoder `D`:

```text
z = E(x)
y_hat = D(z)
E_t = y - y_hat
Ω_(t+1) = decay · Ω_t + gain · E_t
```

The network performs actual gradient descent in JavaScript. A transaction snapshot is taken before mutation. The updated weights and Ω field are committed only when the Λ gate accepts the reconstruction loss, provenance and permission state. Failed transactions restore the previous weights.

## Determinism

The runtime uses a fixed seed (`0x4D4F4147`), a fixed-step clock, bounded graph storage, bounded gradient updates and explicit rollback. Given identical injected packets and control settings, model initialization and packet feature encoding are deterministic.

## Run

```bash
python -m http.server 8000
```

Open:

```text
http://localhost:8000/world-wide-web-vann.html
```

The runtime has no external JavaScript dependencies.

## Controls

- **Pause / Resume** — suspend or continue staged execution.
- **Single Cycle** — execute one complete 15-stage transaction.
- **Inject Web Node** — encode the supplied URL and representation payload.
- **Residual Pulse** — perturb the persistent Ω correction field.
- **Reset Engine** — restore the deterministic initial state.
- **Export Snapshot** — download provenance, learning and transaction telemetry.
- Drag to rotate, scroll to zoom, `Space` to pause, `S` for one cycle, and `P` for an Ω pulse.

## Scope boundary

The simulation does not crawl the public Web. Browsers cannot reliably fetch arbitrary third-party pages because of CORS, authentication, robots policy and permission constraints. Instead, the runtime encodes user-supplied or synthetic resource packets. A production deployment would place an authorized ingestion service in front of the same neural pipeline.
