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
- [x] Supprimer `CMakeLists.txt` et `cmake_modules/` (build = Makefile + MSVC ;
      IDE via `compile_commands.json`)
- [x] Vérifier qu'aucune identité « quake3e » résiduelle ne subsiste côté build (CNAME OK)
- [ ] (Optionnel) Renommer la solution MSVC `code/win32/msvc2017/quake3e.sln` — non fait

### Racine & scripts
- [x] Réécrire **`README.md`** pour UrT (présentation, build, features, lignée/crédits)
- [x] Créer **`scripts/`** : déplacer `make-macosx-app.sh`, `make-macosx-ub2.sh` (+ fix chemin)
- [x] Créer **`LICENSE`** (GPL v2 racine, à côté de `COPYING.txt`)

### Nettoyage `docs/` (legacy Q3A)
- [x] Supprimer `quake3e-changes.txt`, `quake3e-FAQ.txt`, `quake3e.htm`, `quake3e.md`
- [x] Supprimer `README.Linux`, `README.Q3Test`, `LinuxSupport/`, `firewall/`, `q3url_*.cmd`
- [x] Revoir `filter.txt` → **conservé** (documente le cvar fonctionnel `sv_filter`)

### Restructuration `docs/`
- [x] Créer `docs/legal/` et `docs/analysis/`
- [x] `id-readme.txt` → `docs/legal/`
- [x] (déviation) Pas de `docs/legal/GPL.txt` : `LICENSE` + `COPYING.txt` en racine suffisent (évite triple copie)
- [x] `ANALYSIS_REPORT.md` → `docs/analysis/slim-comparison.md`
- [x] `BUILD.md` → `docs/BUILD.md` (maj UrT + section outillage bear/ccache/mold)

### Identité GitHub (actions externes)
- [x] `gh repo rename urbanterror-optimized` (repo renommé)
- [x] `git remote set-url origin …/urbanterror-optimized.git`
- [x] Créer le tag `v0.1.0` (base fondations) — poussé
- [x] `git push origin main` + `git push origin v0.1.0`
- [x] Purger les 26 anciens tags Quake3e (local) + tag `latest` (remote)
- [x] Supprimer la branche `master` legacy (remote ; `master ⊆ main`), défaut GitHub → `main`

---

## M2 — Gouvernance & configuration qualité

*Rendre le projet contribuable et outillé pour la qualité.*
**Risque : très faible.** ✅ **Terminé (2026-06-14).**

- [x] `CONTRIBUTING.md` — workflow git, conventional commits, build & format, process PR
- [x] `SECURITY.md` — signalement via GitHub private advisories (pas d'email exposé)
- [x] `CHANGELOG.md` — format [Keep a Changelog] (+ entrée 0.1.0)
- [x] `CODEOWNERS`
- [x] `.github/ISSUE_TEMPLATE/` (+ config) + `.github/PULL_REQUEST_TEMPLATE.md`
- [x] `.clang-tidy` — jeu de checks conservateur pour le C legacy (validé `--verify-config`)
- [x] Documenter l'outillage (bear/ccache/mold) dans `docs/BUILD.md` (fait en M1)

---

## M3 — CI/CD modernisation

*Le `build.yml` (17 KB) hérité de Quake3e a été scindé et durci.*
**Risque : moyen.** ✅ **Écrit (2026-06-14) — à valider par un run CI réel.**

- [x] Scinder `.github/workflows/build.yml` → `ci.yml` (PR/push) + `release.yml` (tag `v*`)
- [x] `ci.yml` :
  - [x] Matrix build Linux / macOS / Windows (MSYS2) via **Makefile** + ccache
  - [x] Lint **clang-format** sur les **lignes modifiées** (`git clang-format`, pas le legacy)
  - [x] Analyse statique **cppcheck** (informatif, `continue-on-error`)
  - [x] Build **sanitizers** (ASan/UBSan) sur Linux
  - [x] **Caching** (ccache via hendrikmuhs/ccache-action)
- [x] `release.yml` : build multi-plateforme, artifacts `urbanterror-optimized-*`, release auto
- [x] Retirer le `build.yml` legacy (jobs ARM morts, msys32, artifacts `quake3e-*`)
- [x] **Validation** : `ci.yml` vérifié vert sur run réel (clang-format, builds Linux/macOS/
      Windows-MSYS2, ASan/UBSan ✓ ; cppcheck informatif). clang-tidy : reporté (manuel via
      `.clang-tidy`) ; MSVC : couvert par `release.yml`

---

## M4 — Documentation de référence

*Documenter ce qui existe avant d'ajouter du neuf.*
**Risque : faible.** ✅ **Terminé (2026-06-14).**

- [x] `docs/CVARS.md` — cvars UrT (window margins, modversion, sv_filter)
- [x] `docs/CREDITS.md` — id Software / ioquake3 / ec-Quake3e / omg-urt
- [x] `docs/legal/third-party.txt` — libs vendored (libcurl, libjpeg, libogg, libvorbis, SDL2)
- [x] `docs/urt-features.md` — features actuelles + planifiées (s'enrichit au fil de M5)
- [x] Section Documentation ajoutée au `README.md`

---

## M5 — Intégration features UrT

*Une fois la base solide, intégrer la valeur joueur depuis `omg-urt/urbanterror-slim`.*
**Risque : moyen** (touche `code/` — tests requis).

### Features client
- [x] **Tellme / self-tell** — réimplémenté proprement : `messagemode5` (interactif, cohérent
      avec messagemode1-4) + `tellme <text>` (bind) + sauvegarde/rappel historique. Build OK,
      commit `d158fc96`. (Le referee fix `b834398f` et la feature « /cmd dans le chat » de slim
      ne sont **pas repris** : non nécessaires au self-tell ; à rediscuter si voulu.)
- [ ] Console à onglets multiples (`2c70fdc0`, `03b932d1`) — `code/client/cl_console.c`
- [ ] Demo UrT (`9579fc7e`, `ec15c2c6`) — `USE_URTDEMO` (Makefile) + `cl_main.c`, `client.h`, …

### Cvars serveur optionnelles
- [x] `sv_sayprefix` / `sv_tellprefix` (`dd52e95f`) — porté manuellement, build OK
- [x] `sv_nofallDamage` (`be301ebf`) — porté manuellement, build OK
- [x] `sv_infiniteStamina` (`bbb587d4`) — porté manuellement, build OK
- [ ] Referee fix (`b834398f`) — `code/client/cl_keys.c` — **dépend de `tell_me` (tellme)** → après tellme

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
