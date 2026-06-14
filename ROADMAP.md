# 🗺️ Roadmap : Urban Terror Optimized

**Objectif** : transformer ce fork de Quake3e en une base **moderne, propre et solide** pour
Urban Terror 4.x. La priorité est donnée aux **fondations** (conventions, outillage, CI/CD,
identité, hygiène des fichiers) **avant** l'intégration des features UrT.

**Stratégie** : `code/` reste **aligné sur `ec-/Quake3e` upstream** (pour continuer à
cherry-pick ses fixes) ; la réorganisation ne touche que le **non-code** (racine, `docs/`,
`scripts/`, CI). Build canonique = **Makefile** (+ MSVC pour Windows natif).

Ordre : **M0 → M1 → M2 → M3 → M4** (fondations) **→ M5** (features) **→ M6** (release).

---

## ✅ M0 — Fondations posées (acquis)

*État réel à date (2026-06-14). Ne pas refaire.*

- [x] **Sécurité downloads** — `FS_CheckIdPaks` désactivé, `FS_GamePak`, `CL_FirstDownload`
- [x] **Master server + modversion** (`modversion` / `sv_modversion`)
- [x] **Window margins** CSS-like (`r_windowMargin*`)
- [x] **Branding Makefile** — `CNAME`, `RENDERER_DEFAULT=vulkan`, `BASEGAME`/`DEFAULT_GAME=q3ut4`
- [x] **JIT NaN fix** — vérifié **déjà présent** via upstream (`56f03bca` + `cdb374ec`)
- [x] **Conventions dev** — `.clang-format`, `.editorconfig`, `.gitattributes`, `.gitignore`
- [x] **Config Claude Code** — `.claude/settings.json` (allowlist + hook clang-format),
      `.claude/agents/memory-bank-manager.md` ; dédup `.clinerules`/`.instructions.md`
- [x] **Base de connaissances** — memory bank (repo) + vault Obsidian + auto-mémoire
- [x] **Outillage** — bear/ccache/mold/cppcheck installés, `compile_commands.json`, Context7 MCP
- [x] **Remotes sécurisés** — push `DISABLE` sur `slim` et `upstream`

---

## M1 — Identité & hygiène des fichiers

*Le projet ressemble encore à Quake3e. Nettoyer l'identité et l'arborescence non-code.*
**Risque : faible** (aucun fichier de `code/` touché, hors `.sln`).

### Build : abandon de CMake
- [ ] Supprimer `CMakeLists.txt` et `cmake_modules/` (build = Makefile + MSVC ;
      IDE via `compile_commands.json`)
- [ ] Vérifier qu'aucune identité « quake3e » résiduelle ne subsiste côté build
- [ ] (Optionnel) Renommer la solution MSVC `code/win32/msvc2017/quake3e.sln`

### Racine & scripts
- [ ] Réécrire **`README.md`** pour UrT (présentation, build, features, lignée/crédits)
- [ ] Créer **`scripts/`** : déplacer `make-macosx-app.sh`, `make-macosx-ub2.sh`
- [ ] Créer **`LICENSE`** (alias GPL v2 pour GitHub)

### Nettoyage `docs/` (legacy Q3A)
- [ ] Supprimer `quake3e-changes.txt`, `quake3e-FAQ.txt`, `quake3e.htm`, `quake3e.md`
- [ ] Supprimer `README.Linux`, `README.Q3Test`, `LinuxSupport/`, `firewall/`
- [ ] Revoir `filter.txt`, `q3url_add.cmd`, `q3url_rem.cmd` (garder/adapter ou retirer)

### Restructuration `docs/`
- [ ] Créer `docs/legal/` et `docs/analysis/`
- [ ] `id-readme.txt` → `docs/legal/`
- [ ] `COPYING.txt` → `docs/legal/GPL.txt` (copie racine conservée)
- [ ] `ANALYSIS_REPORT.md` → `docs/analysis/slim-comparison.md`
- [ ] `BUILD.md` → `docs/BUILD.md` (maj contenu UrT + bear/ccache/mold)

### Identité GitHub (actions externes)
- [ ] `gh repo rename urbanterror-optimized`
- [ ] `git remote set-url origin git@github.com:miccarrer/urbanterror-optimized.git`
- [ ] Purger les tags Quake3e ; créer le tag `v0.1.0` (base fondations)

---

## M2 — Gouvernance & configuration qualité

*Rendre le projet contribuable et outillé pour la qualité.*
**Risque : très faible.**

- [ ] `CONTRIBUTING.md` — workflow git, conventional commits, build & format, process PR
- [ ] `SECURITY.md` — politique de signalement
- [ ] `CHANGELOG.md` — format [Keep a Changelog]
- [ ] `CODEOWNERS`
- [ ] `.github/ISSUE_TEMPLATE/` + `.github/PULL_REQUEST_TEMPLATE.md`
- [ ] `.clang-tidy` — jeu de checks adapté au C (style ioq3) + suppressions `cppcheck` si besoin
- [ ] Documenter l'outillage (bear/ccache/mold, `compile_commands.json`) dans `docs/BUILD.md`

---

## M3 — CI/CD modernisation

*Le `build.yml` actuel (17 KB) est hérité de Quake3e. Scinder et durcir.*
**Risque : moyen.**

- [ ] Scinder `.github/workflows/build.yml` → `ci.yml` (PR/push) + `release.yml` (tag `v*`)
- [ ] `ci.yml` :
  - [ ] Matrix build Linux / Windows (MSYS2 + MSVC) / macOS via **Makefile**
  - [ ] Lint **clang-format** (`--dry-run -Werror`)
  - [ ] Analyse statique **cppcheck** + **clang-tidy**
  - [ ] Build **sanitizers** (ASan/UBSan) sur Linux
  - [ ] **Caching** (ccache)
- [ ] `release.yml` :
  - [ ] Build + artifacts renommés `urbanterror-optimized-*`
  - [ ] Release notes auto depuis `CHANGELOG.md`
- [ ] Retirer les jobs morts/commentés (ARM commentés, msys32 redondant)

---

## M4 — Documentation de référence

*Documenter ce qui existe avant d'ajouter du neuf.*
**Risque : faible.**

- [ ] `docs/CVARS.md` — cvars UrT (dont window margins, modversion déjà ajoutées)
- [ ] `docs/CREDITS.md` — id Software / ioquake3 / ec-Quake3e / omg-urt
- [ ] `docs/legal/third-party.txt` — libs vendored (libcurl, libjpeg, libogg, libvorbis, SDL2)
- [ ] `docs/urt-features.md` — (s'enrichit au fil de M5)

---

## M5 — Intégration features UrT

*Une fois la base solide, intégrer la valeur joueur depuis `omg-urt/urbanterror-slim`.*
**Risque : moyen** (touche `code/` — tests requis).

### Features client
- [ ] Console à onglets multiples (`2c70fdc0`, `03b932d1`) — `code/client/cl_console.c`
- [ ] Tellme (`d4f12aa7`, `7716e57a`, `002d179d`, `588c4cb1`) — `cl_console.c`, `cl_keys.c`
- [ ] Demo UrT (`9579fc7e`, `ec15c2c6`) — `USE_URTDEMO` (Makefile) + `cl_main.c`, `client.h`, …

### Cvars serveur optionnelles
- [ ] `sv_sayprefix` / `sv_tellprefix` (`dd52e95f`)
- [ ] `sv_nofalldamage` (`be301ebf`)
- [ ] `sv_infiniteStamina` (`bbb587d4`)
- [ ] Referee fix (`b834398f`) — `code/client/cl_keys.c`

### Optionnel
- [ ] dmaHD (`213e0e5d`, ~1524 lignes) — décision en attente

---

## M6 — Release v1.0.0

**Risque : faible.**

- [ ] Build test complet toutes plateformes via CI
- [ ] Validation finale (`git status` propre, README à jour, build OK)
- [ ] Tag `v1.0.0` + entrée `CHANGELOG.md`

---

## 📋 Ordre d'exécution

| # | Milestone | Durée | Risque |
|---|-----------|-------|--------|
| ✅ | M0 — Fondations posées | — | — |
| 1 | M1 — Identité & hygiène fichiers | Moyen | Faible |
| 2 | M2 — Gouvernance & qualité | Court | Très faible |
| 3 | M3 — CI/CD | Moyen | Moyen |
| 4 | M4 — Docs de référence | Court | Faible |
| 5 | M5 — Features UrT | Long | Moyen |
| 6 | M6 — Release v1.0.0 | Court | Faible |

---

## 📊 Référence : Fonctionnalités déjà présentes (13)

Ces fonctionnalités de urbanterror-slim sont **déjà intégrées** :

1. ✅ MAX_RELIABLE_COMMANDS = 128
2. ✅ qkey fix
3. ✅ MAX_LOCATIONS
4. ✅ condump actif
5. ✅ con_autoclear
6. ✅ r_ext_compressed_textures
7. ✅ .mtrx shader loading
8. ✅ CalcSpecular epsilon fix
9. ✅ r_parallaxMapOffset
10. ✅ teleport command
11. ✅ spoof/forcecvar/sendclientcommand
12. ✅ kicknum/kickall/kickbots
13. ✅ Authorize server disabled
