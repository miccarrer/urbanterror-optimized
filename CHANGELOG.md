# Changelog

All notable changes to this project are documented here.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.2.0] - 2026-06-15

Modernization release — quality, security and developer-experience hardening on top of the
0.1.0 foundations, plus the first UrT features (server cvars, tellme). UrT console tabs and
demo playback remain deferred (need fresh reimplementation + in-game testing).

### Added
- Governance & quality config: `CONTRIBUTING.md`, `SECURITY.md`, `CODEOWNERS`,
  GitHub issue/PR templates, `.clang-tidy` (M2); `CODE_OF_CONDUCT.md`, `ARCHITECTURE.md`,
  `AUDIT.md`, README badges, `.devcontainer/` (Dockerfile + devcontainer.json) (M7).
- CI/CD: `ci.yml` (clang-format on changed lines, cppcheck, unit tests, fuzz smoke,
  ASan/UBSan, Linux/macOS/Windows-MinGW build matrix) and `release.yml` (multi-platform
  Vulkan+OpenGL builds, GitHub Releases) (M3).
- Reference docs: `docs/CVARS.md`, `docs/CREDITS.md`, `docs/urt-features.md`,
  `docs/legal/third-party.txt` (M4).
- Server cvars: `sv_sayprefix` / `sv_tellprefix`, `sv_nofallDamage`, `sv_infiniteStamina` (M5).
- Tellme / `messagemode5` chat mode: a leading `/` runs a client console command locally,
  while text or `!cmd` is self-told to the server; with input-history recall (M5).
- Test & fuzz harness (`tests/`): Unity unit tests for `q_math.c` and a libFuzzer target for
  the userinfo parsers (`Info_*`), run under ASan/UBSan and wired into CI (M7).
- Supply-chain & security tooling: CodeQL workflow, Dependabot (GitHub Actions),
  SHA-pinned Actions, and Sigstore build-provenance attestation on releases (M7).

### Changed
- Build: the version banner is now stamped from `git describe` at build time (was a static
  string); pinned the C dialect to `-std=gnu99` (M7).

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

[Unreleased]: https://github.com/miccarrer/urbanterror-optimized/compare/v0.2.0...HEAD
[0.2.0]: https://github.com/miccarrer/urbanterror-optimized/compare/v0.1.0...v0.2.0
[0.1.0]: https://github.com/miccarrer/urbanterror-optimized/releases/tag/v0.1.0
