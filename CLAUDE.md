# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**Urban Terror Optimized** is a game engine (client + server) for Urban Terror 4.x, forked from `ec-/Quake3e` (itself a fork of ioquake3/id-Quake3). It is a C codebase targeting Linux, macOS, Windows, and Raspberry Pi.

Lineage: `id Software Q3A → ioquake3 → ec-/Quake3e → [omg-urt/urbanterror-slim] → urbanterror-optimized (this repo)`

## Memory Bank — Read First

At the start of any non-trivial task, read the memory bank files to understand current state:

```
memory-bank/projectbrief.md     # Project scope & goals
memory-bank/activeContext.md    # Current task / in-progress work
memory-bank/progress.md         # Roadmap progress & blockers
memory-bank/systemPatterns.md   # Architecture & code conventions
memory-bank/techContext.md      # Build system & dependencies
```

Update `memory-bank/activeContext.md` and `memory-bank/progress.md` at significant milestones and at end of session.

The `.claude/agents/memory-bank-manager.md` subagent automates the full memory bank workflow.

## Build Commands

```bash
# Standard build (outputs to build/)
make

# Fast rebuild check after a code change
make clean && make -j$(nproc) 2>&1 | tail -5

# Install to game directory
make install DESTDIR=<path_to_urt_install>

# Common Makefile options
make BUILD_SERVER=0                          # Client only
make USE_RENDERER_DLOPEN=0 RENDERER_DEFAULT=vulkan  # Static Vulkan renderer
make USE_SDL=0                               # Disable SDL2 backend
```

Key Makefile variables: `CNAME`, `DNAME`, `ARCH`, `USE_SDL`, `USE_VULKAN`, `USE_OPENGL`, `USE_OPENGL2`, `USE_RENDERER_DLOPEN`, `RENDERER_DEFAULT`, `BUILD_CLIENT`, `BUILD_SERVER`.

MSVC (Windows): open `code/win32/msvc2017/quake3e.sln`.

CMake is an alternative build system but `CNAME` is still set to `"quake3e"` — fix pending (Phase 2).

## Shell Warning

**The user's default shell is `fish`, which is not POSIX-compliant.** Prefix complex commands with `bash -c '...'` to avoid syntax issues (`$()`, `&&`, glob `*` all behave differently in fish).

## Code Architecture

```
code/
├── qcommon/        # Shared client/server: VM, network, files, cvars, commands
├── client/         # Client: console, input, sound, UI, downloads
├── server/         # Server: game bridge, network, snapshots, commands
├── renderer/       # OpenGL 1.x renderer (modular, dlopen)
├── renderer2/      # OpenGL2 renderer (unmaintained, ioq3 original)
├── renderervk/     # Vulkan renderer (recommended)
├── renderercommon/ # Shared renderer code
├── sdl/            # SDL2 backend (video, audio, input)
├── botlib/         # Bot AI (AAS pathfinding)
├── asm/            # x86/x64 optimized assembly (audio mixing)
└── win32/          # Windows-specific code + MSVC solution
```

**Renderers** are either static (linked at compile time with `USE_RENDERER_DLOPEN=0`) or dynamic (loaded at runtime via `dlopen`; switch with `\cl_renderer` cvar).

**VM system** (`code/qcommon/vm.c`) loads game logic as QVM bytecode or native DLLs. JIT compilers: `vm_x86.c`, `vm_aarch64.c`, `vm_powerpc.c`.

**Cvars & Commands**: `code/qcommon/cvar.c` and `code/qcommon/cmd.c`.

## Coding Style (C, Quake3/ioq3 convention)

- Indentation: **tabs**
- Opening brace on same line as control statement; closing `}` aligned with keyword
- Types/structs: `PascalCase`; variables: `camelCase` or `snake_case`
- Header guards: `#ifndef`/`#define` (no `#pragma once`)

```c
void SomeFunction( int arg ) {
	int localVar;

	if ( arg > 0 ) {
		localVar = arg * 2;
	} else {
		localVar = 0;
	}
}
```

## Git Conventions

- Branch `main` is primary
- Feature branches: `feature/<short-name>`, fix branches: `fix/<short-name>`
- Conventional commits: `feat:`, `fix:`, `docs:`, `refactor:`, `chore:`, `ci:`
- After any C code change, verify `make` compiles before committing

## Active Roadmap (foundations first)

See `ROADMAP.md` for full details. Milestone order — **foundations before features**:

- ✅ **M0** — Foundations laid (security fixes, branding Makefile, JIT NaN verified, dev
  conventions, Claude Code config, tooling). Done — do not redo.
- **M1** — Identity & file hygiene (drop CMake, README, `scripts/`, purge Q3A legacy docs,
  restructure `docs/`, LICENSE, GitHub rename + `v0.1.0`) ← **current**
- **M2** — Governance & quality config (CONTRIBUTING/SECURITY/CHANGELOG/CODEOWNERS,
  templates, `.clang-tidy`)
- **M3** — CI/CD modernization (`ci.yml` + `release.yml`, clang-format/cppcheck/clang-tidy/
  ASan-UBSan/cache)
- **M4** — Reference docs (CVARS, CREDITS, third-party, urt-features)
- **M5** — UrT feature integration (tabbed console, tellme, demo, server cvars, opt. dmaHD)
- **M6** — Release v1.0.0

**Constraint**: `code/` stays aligned with `ec-/Quake3e` upstream (preserve cherry-pick);
reorganize **non-code only** (root, `docs/`, `scripts/`, CI).

`ANALYSIS_REPORT.md` documents which commits from `omg-urt/urbanterror-slim` are missing and need integration.
