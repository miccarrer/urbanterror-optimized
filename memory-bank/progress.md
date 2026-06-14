# Progress — Urban Terror Optimized

## État global du projet

**Milestone actuel** : **M1 — Identité & hygiène des fichiers**. Fondations (M0) posées. Roadmap réorganisée « fondations d'abord » (voir `ROADMAP.md`).

---

## ✅ Terminé

### Analyse & Planification (2026-06-14)
- [x] Analyse des 66 commits de `omg-urt/urbanterror-slim` vs notre projet
- [x] Identification de 11 fonctionnalités manquantes, 5 partielles, 13 déjà présentes
- [x] Génération de `ANALYSIS_REPORT.md`
- [x] Création de `ROADMAP.md` (6 phases détaillées)
- [x] Configuration du Memory Bank + `.clinerules` + `.instructions.md` + `.agents/`

### Phase 1A — Fixes sécurité & branding (2026-06-14)
- [x] Branding : Q3_VERSION, CLIENT_WINDOW_TITLE, BASEGAME → `q3ut4`, DEFAULT_GAME → `q3ut4`
- [x] Build config : CNAME → `urbanterror-optimized`, RENDERER_DEFAULT → `vulkan` (Makefile)
- [x] Sécurité : Désactivation de `FS_CheckIdPaks()` (Urban Terror utilise ses propres paks)
- [x] Sécurité : `FS_GamePak` check pour empêcher le téléchargement de paks UrT depuis serveurs moddés
- [x] Sécurité : `CL_FirstDownload` pour filtrer les téléchargements sur serveurs moddés
- [x] Feature : Master server + cvars `modversion` / `sv_modversion`
- [x] Feature : Système de window margins CSS-like (`r_windowMarginTop/Bottom/Left/Right`)
- [x] Build test : ✅ Succès

### Setup repo (2026-06-14)
- [x] CLAUDE.md créé (guidance pour Claude Code)
- [x] Git remotes reconfigurés : `origin` → `miccarrer/Quake3e`, `upstream` → `ec-/Quake3e`
- [x] Feature branch mergée en FF dans `main`, worktree supprimé, branches nettoyées
- [x] `main` pushé sur `origin`

### M0 — Fondations / outillage (2026-06-14, session 3)
- [x] Conventions dev : `.clang-format`, `.editorconfig`, `.gitattributes`, `.gitignore`
- [x] Config Claude Code : `.claude/settings.json` (allowlist + hook clang-format),
      `.claude/agents/memory-bank-manager.md` ; dédup `.clinerules`/`.instructions.md`
- [x] Outillage Arch : bear, ccache, mold, cppcheck installés ; `compile_commands.json` généré
- [x] MCP Context7 ajouté (scope user) ; remotes `slim`/`upstream` push `DISABLE`
- [x] Base de connaissances : vault Obsidian (PARA-lite, `github.com/miccarrer/notes` privé)
      + symlink memory-bank ; auto-mémoire Claude
- [x] Roadmap réorganisée « fondations d'abord » (M0→M6)

---

## ✅ Résolu

### JIT NaN fix — DÉJÀ PRÉSENT via Quake3e upstream (vérifié 2026-06-14)
- `56f03bca` (x86 JIT: Fix comparison ops to account for NaN operands) est **dans l'historique de `main`** (`git merge-base --is-ancestor 56f03bca HEAD` → YES). La logique est visible dans `code/qcommon/vm_x86.c` (`shouldNaNCheck`, ~ligne 1775).
- `cdb374ec` (Makefile: disable unsafe FP optimizations for QVM modules) est **aussi présent** (`-fno-fast-math` sur les modules QVM, voir `Makefile:687,718`).
- Le fix aarch64/armv7l équivalent (`85ff5501`) est également présent.
- **Conclusion** : aucune action requise. Le fix de slim était redondant avec l'upstream.

---

## 📋 À faire — par milestone (voir `ROADMAP.md` pour le détail complet)

> Contrainte : `code/` reste aligné upstream ; réorg du **non-code** uniquement (M1–M4).

### M1 — Identité & hygiène fichiers ← **en cours (local fait, GitHub en attente)**
- [x] Supprimer `CMakeLists.txt` + `cmake_modules/` (abandon CMake)
- [x] Réécrire `README.md` pour UrT ; créer `scripts/` (+ fix chemin) ; créer `LICENSE`
- [x] Purger docs Q3A legacy ; restructurer `docs/` (legal/, analysis/) ; `BUILD.md` → `docs/`
- [ ] **GitHub (externe, à déclencher)** : rename repo → `urbanterror-optimized`, maj `origin`,
      tags Quake3e → `v0.1.0`, `git push origin main` (commits M1 locaux non poussés)

### M2 — Gouvernance & qualité ✅ FAIT (2026-06-14)
- [x] CONTRIBUTING.md, SECURITY.md, CHANGELOG.md, CODEOWNERS, templates `.github/` (issue+PR)
- [x] `.clang-tidy` (validé `--verify-config`)

### M3 — CI/CD (complète) ✅ écrit (2026-06-14), à valider par un run réel
- [x] Scinder `build.yml` → `ci.yml` + `release.yml`
- [x] ci : matrix Makefile (Linux/macOS/Win-MSYS2) + clang-format (lignes modifiées) + cppcheck
      (informatif) + ASan/UBSan + cache ccache
- [x] release : build multi-plateforme, artifacts `urbanterror-optimized-*`, release auto
- [ ] À surveiller : 1er run de `ci.yml` (clang-tidy en CI reporté ; MSVC couvert par release.yml)

### M4 — Docs de référence
- [ ] `docs/CVARS.md`, `docs/CREDITS.md`, `docs/legal/third-party.txt`, `docs/urt-features.md`

### M5 — Features UrT (ex-1B/1C/1D)
- [ ] Console à onglets (`2c70fdc0`), Tellme (`d4f12aa7`+), Demo UrT (`9579fc7e`)
- [ ] Cvars serveur : sv_sayprefix (`dd52e95f`), sv_nofalldamage (`be301ebf`), sv_infiniteStamina (`bbb587d4`), referee fix (`b834398f`)
- [ ] (Optionnel) dmaHD (`213e0e5d`)

### M6 — Release v1.0.0
- [ ] Build test complet via CI ; tag `v1.0.0` ; CHANGELOG

---

## 📊 Métriques

| Indicateur | Valeur |
|------------|--------|
| Commits urbanterror-slim analysés | 66 |
| Fonctionnalités déjà intégrées | 13 |
| Fonctionnalités intégrées en M0 | 4 (sécurité + modversion + window margins) |
| Fonctionnalités manquantes restantes | ~7 (M5) |
| Milestones roadmap | 7 (M0–M6) |
| Milestones terminés | 1 (M0 — fondations) |
