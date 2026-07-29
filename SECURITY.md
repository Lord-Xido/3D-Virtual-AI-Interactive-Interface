# Security Policy

This repository contains experimental browser and native graphics runtimes. It is not a hardened execution environment.

## Reporting

Use GitHub private vulnerability reporting when available. Otherwise, open a minimal public issue requesting a private channel without including exploit details, secrets or sensitive data.

## Browser boundaries

- External CDN scripts execute with the privileges of the page. Review and pin dependencies.
- Client-side policy checks do not protect server-side resources.
- Media capture, file export and graphics APIs vary by browser and must be capability-checked.
- Do not load untrusted HTML, scripts, model files or binary state into demonstrations without isolation and validation.
- Serve development copies from a controlled local origin.

## Native boundaries

C++ and graphics code may expose memory-safety, driver and platform risks. Build and run native experiments in an isolated user environment with least privilege.

## Snapshots and media

Runtime snapshots and recordings may contain user-supplied text, media metadata or environment details. Review artifacts before sharing them.

## Secrets

Never commit API keys, model-hosting tokens, cloud credentials or personal media. Rotate any exposed secret immediately.

## Integrity locks

Architecture hashes detect a mismatch against expected repository content. They do not provide encryption, immutable storage or proof that the locked design is secure.

## Supported code

Security fixes target `main`. Unmerged pull requests and historical demonstrations receive best-effort support only.
