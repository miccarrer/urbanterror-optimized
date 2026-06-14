# Changelog

All notable changes to this project are documented here.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Governance & quality config: `CONTRIBUTING.md`, `SECURITY.md`, `CODEOWNERS`,
  GitHub issue/PR templates, `.clang-tidy` (M2).

## [0.1.0] - 2026-06-14

Foundations release — modern base for Urban Terror Optimized, before UrT feature integration.

### Added
- Urban Terror branding: `BASEGAME`/`DEFAULT_GAME` → `q3ut4`, window title, version.
- Master-server `modversion` / `sv_modversion` reporting.
- CSS-like window margins (`r_windowMargin*`).
- Developer conventions: `.clang-format`, `.editorconfig`, `.gitattributes`, `.gitignore`.
- Project Claude Code config (`.claude/`), memory bank, `LICENSE`.

### Changed
- Build: `CNAME` → `urbanterror-optimized`, default renderer → Vulkan (Makefile).
- Dropped CMake; build is Makefile + MSVC (IDE via `compile_commands.json`).
- Restructured non-code: `docs/` (`legal/`, `analysis/`), `scripts/`; rewrote `README.md`.
- Repository renamed to `urbanterror-optimized`; default branch `main`.

### Security
- Disabled `FS_CheckIdPaks`; added `FS_GamePak` and `CL_FirstDownload` filtering to prevent
  UrT pak downloads from modded servers.

### Removed
- Quake3e/Q3A legacy documentation and the legacy `master` branch / date-based tags.

[Unreleased]: https://github.com/miccarrer/urbanterror-optimized/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/miccarrer/urbanterror-optimized/releases/tag/v0.1.0
