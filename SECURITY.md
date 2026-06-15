# Security Policy

## Supported versions

This project is in active early development. Security fixes target the `main` branch and the
latest tagged release.

| Version | Supported |
|---------|:---------:|
| `main` / latest tag | ✅ |
| older tags | ❌ |

## Reporting a vulnerability

**Please do not open a public issue for security vulnerabilities.**

Use GitHub's **private vulnerability reporting**:
[Security → Report a vulnerability](https://github.com/miccarrer/urbanterror-optimized/security/advisories/new).

Include where possible:
- affected component (client / dedicated server / renderer / VM / netcode)
- reproduction steps or a proof of concept
- impact assessment (e.g. remote crash, memory corruption, info leak)

We will acknowledge the report, investigate, and coordinate a fix and disclosure timeline.

## Scope notes

This repository is the **engine** (forked from ec-/Quake3e / ioquake3). Issues in bundled
third-party libraries (libcurl, libjpeg, libogg, libvorbis, SDL2) should also be reported
upstream when applicable. Game content and the Urban Terror game logic (QVM mod) are out of
scope.

## Static analysis

CodeQL (`security-extended`) runs on every PR and weekly. How findings are triaged — and why we
do not mass-rewrite upstream-aligned code — is documented in
[docs/security-triage.md](docs/security-triage.md).
