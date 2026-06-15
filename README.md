# Urban Terror Optimized

[![CI](https://github.com/miccarrer/urbanterror-optimized/actions/workflows/ci.yml/badge.svg)](https://github.com/miccarrer/urbanterror-optimized/actions/workflows/ci.yml)
[![CodeQL](https://github.com/miccarrer/urbanterror-optimized/actions/workflows/codeql.yml/badge.svg)](https://github.com/miccarrer/urbanterror-optimized/actions/workflows/codeql.yml)
[![Release](https://img.shields.io/github/v/release/miccarrer/urbanterror-optimized?sort=semver)](https://github.com/miccarrer/urbanterror-optimized/releases)
[![License: GPL v2](https://img.shields.io/badge/License-GPL_v2-blue.svg)](LICENSE)
![Platforms](https://img.shields.io/badge/platforms-Linux%20%7C%20Windows%20%7C%20macOS%20%7C%20Raspberry%20Pi-lightgrey)

A modern, fast and secure **client + server engine for Urban Terror 4.x**, based on
[ec-/Quake3e](https://github.com/ec-/Quake3e) (itself a fork of
[ioquake3](https://github.com/ioquake/ioq3) / id Software's Quake III Arena).

> This repository contains the **engine only** — no game content. To play, copy the resulting
> binaries into your existing Urban Terror installation (game data lives in `q3ut4/`).

See **[Build Instructions](docs/BUILD.md)** to compile, and the **[Roadmap](ROADMAP.md)** for
project direction.

## Why this project

The historical Urban Terror engine (ioq3-for-UrT) is old and unmaintained. `ec-/Quake3e` is an
excellent modern engine but lacks UrT-specific features. Urban Terror Optimized combines the
Quake3e engine improvements with the features Urban Terror needs, on a clean and maintained base.

## Key features

### Engine (from Quake3e)
* optimized **Vulkan** renderer (recommended) and optimized **OpenGL** renderer
* significantly reworked QVM (Quake Virtual Machine), reworked Zone memory allocator
* improved server-side DoS protection, much reduced memory usage
* raised filesystem limits (handles very large map directories)
* raw mouse input, external-ffmpeg video pipe, SDL2 backend (selectable at compile time)
* multi-platform: Linux, Windows (MinGW + MSVC), macOS (Intel & Apple Silicon), Raspberry Pi

### Urban Terror specific
* master-server / `modversion` reporting so the engine is visible in the UrT server browser
* security: download filtering to prevent UrT pak downloads from modded servers
* CSS-like window margins (`r_windowMargin*`) for windowed-mode positioning

The Vulkan renderer adds per-pixel dynamic lighting, fast flares, anisotropic filtering, VBO
world caching, reversed depth buffer, FBO effects (MSAA/SSAA, bloom, HDR, screen-map
reflections) and more. See the [Quake3e documentation](https://github.com/ec-/Quake3e) for the
full renderer feature set.

## Build

Quick start (Linux):

```bash
make
make install DESTDIR=<path_to_urt_install>
```

Full instructions for all platforms (Windows MSVC/MSYS2/MinGW, macOS, Raspberry Pi, PowerPC)
and developer tooling are in **[docs/BUILD.md](docs/BUILD.md)**.

## Running

Release archives ship a single client (`urbanterror-optimized`) plus a dedicated server
(`urbanterror-optimized.ded`). The client uses **dynamically loaded renderers** and starts with
**Vulkan by default**. If Vulkan does not start on your system, switch to OpenGL from the
in-game console without any other download:

```
\cl_renderer opengl
\vid_restart
```

## Documentation

- [Build instructions](docs/BUILD.md) · [Architecture](ARCHITECTURE.md) · [Cvars](docs/CVARS.md) · [UrT features](docs/urt-features.md)
- [Contributing](CONTRIBUTING.md) · [Code of Conduct](CODE_OF_CONDUCT.md) · [Changelog](CHANGELOG.md) · [Security policy](SECURITY.md)
- [Credits](docs/CREDITS.md) · [Roadmap](ROADMAP.md) · [Audit & modernization](AUDIT.md)

## Lineage & credits

```
id Software (Quake III Arena, GPL)
 └── ioquake3
      └── ec-/Quake3e
           ├── omg-urt/urbanterror-slim   (UrT features reference)
           └── urbanterror-optimized       (this project)
```

## License

GPL v2 — see [LICENSE](LICENSE). Original id Software readme:
[docs/legal/id-readme.txt](docs/legal/id-readme.txt).
