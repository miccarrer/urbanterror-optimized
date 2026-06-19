# Progress — Urban Terror Optimized

## État global du projet

**Milestone actuel** : **scripting cfg — Lot 2** (session 13, branche `feature/cfg-scripting-lot2`).
Console UX render-side : `con_height`/`con_opacity`/`con_notifyLines`/`con_notifyY` (`cl_console.c`,
`NUM_CON_TIMES` 4→8). `make smoke-client` vert (cas `cases/client/console-ux.cfg`). **À committer +
push + PR.** Précédemment : Lot 1 scripting (`alias`/`if`/`cvarlock`/`time`) MERGÉ (session 12,
PR #28, `e0614bf2`) ; police des onglets `con_tabScale` (session 11, PR #27) ; `cm360` (session 10,
PR #25/#26) ; harnais headless (session 9, PR #24). Fondations M0–M4 terminées ; M5 partiel ;
M6 (release v0.2.0) publiée ; M7 terminé ; Feature #1 (identity) mergée PR #20.

**Session 13 (2026-06-19) — scripting cfg (Lot 2, console UX)** : 2e des 3 lots. UX console
render-side : `con_height` (0.1–1.0, remplace `0.5` hardcodé `Con_RunConsole`), `con_opacity` (0–1,
deux chemins `Con_DrawSolidConsole`), `con_notifyLines` (0–8) + `con_notifyY` (px) dans
`Con_DrawNotify` (`NUM_CON_TIMES` 4→8). Seul fichier code : `cl_console.c`. Test
`cases/client/console-ux.cfg` (3/3 PASS via `make smoke-client`). Build client+serveur OK. Branche
`feature/cfg-scripting-lot2`, à committer/push/PR. Lot 3 (recherche scrollback, smart condump,
`con_notifyFilter`) à venir. Détails § `activeContext.md` Session 13.

**Session 12 (2026-06-17) — scripting cfg (Lot 1, PR #28 mergée)** : 1er des 3 lots « console +
scripting `.cfg` » (idées `docs/FEATURE_IDEAS.md`). Ajout `alias`/`unalias`/`unaliasall` (persistés,
garde anti-récursion), `if <cvar> <op> <value> <cmd>` (réutilise `Com_Compare`, ex-`Com_AssertCompare`
rendu public), `cvarlock`/`cvarunlock` (flag runtime `CVAR_USER_LOCKED`), `time` (µs). Fichiers :
`cmd.c`/`cvar.c`/`common.c`/`qcommon.h`/`q_shared.h`. Test `tests/integration/cases/scripting.cfg`
(2/2 PASS). Build client+serveur OK. **Mergé dans `main` (`e0614bf2`)**, branche supprimée. Lots 2–3
(console UX render-side) à venir. Détails § `activeContext.md` Session 12.

**Session 11 (2026-06-17) — police des onglets** : la tabbed-console était déjà mergée (PR #19) ;
seule la taille des titres restait. Ajout cvar `con_tabScale` (défaut 1.25, range 1.0–3.0) + helper
`Con_DrawScaledChar` (`cl_console.c`), doc CVARS.md. **Saga CodeQL** : 4 clamps n'ont pas satisfait
`cpp/uncontrolled-arithmetic` (query bruyante, valeurs bornées, aucun overflow réel) → 3 alertes
dismissées en FP (consignées `docs/security-triage.md`). PR #27 mergée. Détails § `activeContext.md` Session 11.

**Session 10 (2026-06-17) — `cm360` + diagnostic DPI** : commande `cm360` (sensitivity ↔ cm/360) +
cvar `m_dpi` livrés (PR #25 mergée). Écart 2× signalé (cm360 51.95 cm vs ~26 cm ressenti) → diagnostic
complet : mod, accel KDE, XWayland, display scale tous réfutés ; lecture **evdev brute** → souris
**physiquement à ~1600 DPI** (preset onboard G305 via bouton DPI-shift), Piper/libratbag affichant un
800 non appliqué (profil `(disabled)`). Verdict : math/SDL/cm360 corrects, `m_dpi` mentait. Résolu côté
utilisateur (`m_dpi 1600`). Leçon documentée (`docs/CVARS.md`, PR #26). Détails § `activeContext.md` Session 10.

**Session 9 (2026-06-17) — autonomie agent / CI** : harnais de test runtime headless. Tier 0
(`assert`/`assert_cvar`, `quit <code>`, `com_logTimestamps`) + Tier 1 (`scripts/headless`, runner
`tests/integration/`, **null renderer** `code/renderernull/`, **`cl_noUI`** → client headless
**install-free**). Cibles `make smoke` (serveur) + `make smoke-client` (client) — install-free, en CI
(job `integration`). 8 commits. Détails dans `activeContext.md` § Session 9 ; idées de suite dans
`docs/FEATURE_IDEAS.md` § Autonomie de l'agent.

**Session 8 (2026-06-16) — test en jeu** : a révélé un bug `fs_gamedir` (le dossier `download` enregistré
dans `FS_Startup` écrasait `fs_gamedir` → écritures dans `q3ut4/download/`). Corrigé (`6398949c`). Plus
affinage identity : denylist des 8 vestiges Q3 (`model`/`headmodel`/`team_*`/`sex`/`color1/2`/`handicap`),
preview `listidentities` sans `model`, doc « sauver connecté en jeu » + note `race*` vs `model`
(`3f93b6e6`, `c8dbe967`, `e6603912`). **Leçon** : userinfo du mod flaggé seulement après chargement cgame
→ toujours sauver les identités **en jeu**.

---

## ✅ Terminé

### Session 12 — Scripting cfg, Lot 1 (2026-06-17) ← branche `feature/cfg-scripting`
- [x] **`alias`/`unalias`/`unaliasall`** (`cmd.c`) : séquences nommées, dispatch dans
      `Cmd_ExecuteString` (après commandes, avant cvars), persistance `q3config.cfg`
      (`Cmd_WriteAliases`), garde anti-récursion 1024/passe, refus de masquer un builtin
- [x] **`if <cvar> <op> <value> <cmd>`** : exec conditionnel ; `Com_AssertCompare`→`Com_Compare`
      rendu public (`qcommon.h`) ; limité aux conditions
- [x] **`cvarlock`/`cvarunlock`** (`cvar.c`) : flag `CVAR_USER_LOCKED` (0x40000), check dans
      `Cvar_Set2`, runtime-only (ni archivé ni réseau)
- [x] **`time <cmd>`** : profiling µs (`Sys_Microseconds`)
- [x] **Test** `tests/integration/cases/scripting.cfg` — `make smoke` 2/2 PASS ; build client+serveur OK
- [x] **Docs** : `docs/CVARS.md` § Cfg scripting ; `ROADMAP.md` M8 #6 + rattrapage
- [ ] **Restant** : commit + push + PR ; Lot 2 (console UX `con_height`/notify), Lot 3 (recherche/condump)

### Session 11 — Police des onglets de console (2026-06-17) ← PR #27 (mergée)
- [x] **`con_tabScale`** : cvar taille des titres d'onglets (défaut 1.25, range 1.0–3.0,
      `CVAR_ARCHIVE_ND`) + helper `Con_DrawScaledChar` (`cl_console.c`) ; doc `docs/CVARS.md`
- [x] **Constat** : tabbed-console déjà livrée (PR #19) — seule la police restait à régler
- [x] **CodeQL** : 4 itérations de clamps (haute/longueur/fermé [1,256]) ; query `uncontrolled-arithmetic`
      non satisfaite malgré valeurs bornées → 3 alertes **dismissées FP** (consigné `docs/security-triage.md`)
- [x] **Ménage** : branches locales mergées supprimées (`docs/cm360-dpi-tip`, `feature/tab-title-font`)
- [ ] **Restant utilisateur** : `git push origin main` (context+triage) ; supprimer remote `feature/tab-title-font`

### Session 10 — `cm360` + diagnostic écart DPI (2026-06-17) ← PR #25 (mergée), PR #26 (docs)
- [x] **Feature `cm360`** : commande convertisseur `sensitivity` ↔ cm/360 + cvar `m_dpi` (`cl_input.c`),
      doc `docs/CVARS.md` — **mergée (PR #25)**
- [x] **Diagnostic écart 2×** : réfutation systématique (mod / accel KDE / XWayland / display scale),
      instrumentation `mousecount` temporaire (retirée), sonde **evdev brute** → souris G305 réellement
      à ~1600 DPI (preset onboard, pas 800) → **aucun bug moteur**
- [x] **Note troubleshooting** (`docs/CVARS.md`) : `m_dpi` doit refléter le DPI matériel réel ; vérifier
      via comptes kernel — branche `docs/cm360-dpi-tip`, **PR #26 ouverte**
- [x] **Ménage** : 3 branches locales mergées supprimées ; 16 remotes mergées identifiées (suppression utilisateur)
- [ ] **Restant utilisateur** : merge PR #26 ; `git push origin --delete` des 16 branches mergées

### Session 9 — Harnais de test runtime headless (2026-06-17) ← branche `feature/agent-test-harness`
- [x] **Tier 0** : `assert`/`assert_cvar`, `quit <code>` (`Sys_Quit(int)`), `com_logTimestamps`
      (`common.c`, `unix_main.c`, `win_main.c`, `qcommon.h`)
- [x] **Tier 1** : `scripts/headless` (modes serveur/client/UI), runner `tests/integration/run.sh`
      + fixture install-free, **null renderer** `code/renderernull/tr_null.c` (module dlopen,
      `cl_renderer null`, `USE_RENDERER_NULL`), **`cl_noUI`** (client headless **sans asset**)
- [x] **Cibles + CI** : `make smoke` (serveur) + `make smoke-client` (client), tous deux install-free ;
      job CI `integration` lance les deux suites + détecte les FAIL (testé négatif)
- [x] **Mergé via PR #24**
- [ ] **Restant** : étoffer `cases/` au fil des features ; Tier 2/3 (introspection JSON, déterminisme) non commencés

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
      `.claude/agents/context-manager.md` ; dédup `.clinerules`/`.instructions.md`
- [x] Outillage Arch : bear, ccache, mold, cppcheck installés ; `compile_commands.json` généré
- [x] MCP Context7 ajouté (scope user) ; remotes `slim`/`upstream` push `DISABLE`
- [x] Base de connaissances : vault Obsidian (PARA-lite, `github.com/miccarrer/notes` privé)
      + symlink .context ; auto-mémoire Claude
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
- [x] Validé : `ci.yml` vert sur run réel (clang-format + builds Linux/macOS/Win + ASan/UBSan)

### M4 — Docs de référence ✅ FAIT (2026-06-14)
- [x] `docs/CVARS.md`, `docs/CREDITS.md`, `docs/legal/third-party.txt`, `docs/urt-features.md`
      + section Documentation dans README

### M5 — Features UrT (ex-1B/1C/1D) — EN COURS
- [x] Cvars serveur : sv_sayprefix/sv_tellprefix, sv_nofallDamage, sv_infiniteStamina
      (portés manuellement depuis slim, build OK, commit `aeac0131`)
- [x] Tellme / messagemode5 (`d158fc96`, `450d2f23`) : mode chat ; texte/`!cmd` → self-tell
      serveur (commandes bot invisibles) ; `/cmd` → commande CLIENT locale (`Cbuf`) + historique
      + rappel ↑/↓. Intègre l'équivalent du referee fix (gating `chat_cmdMode`). `tellme <text>`
      = variante bind.
- [x] **FAIT** Console à onglets (`855ae13b`) — réimplémentation fraîche (2026-06-15, testée en jeu,
      branche `feature/tabbed-console`). `consoles[]` All/General/Frag/Chat ; routage frag/chat via
      octets marqueurs (0x11/0x12/0x13) ; barre d'onglets visuelle (liseré rouge) ; switch souris
      (clic G/D) + shift+←/→ ; cvar `con_tabs` + cmds `con_nexttab`/`con_prevtab`. Itéré en 5 passes
      (infra → routage → UI/clic → position → finitions) avec test en jeu entre chaque.
- [ ] **DIFFÉRÉ** Demo UrT (`9579fc7e`) — port invasif (218 lignes `USE_URT_DEMO`), cherry-pick =
      5 conflits (notre demo-parsing modernisé par Quake3e). Faisable (guardé) mais nécessite
      tests `.urtdemo`/`.dm_` en jeu.
- [ ] (Optionnel) dmaHD (`213e0e5d`)

**Leçon M5** : les features petites/isolées (cvars serveur, tellme) se portent proprement
(port manuel + build + test ciblé). Les features structurelles/invasives (refactor console,
pipeline demo) sur le code divergé de slim nécessitent une réimplémentation fraîche + tests
runtime itératifs — non adaptées au port à l'aveugle. Différées jusqu'à dispo de tests en continu.

> ✅ **Hook clang-format corrigé** par l'utilisateur (`git clang-format -f`, via `bash -c`) :
> formate uniquement les lignes modifiées. L'outil Edit est de nouveau utilisable sur `.c`/`.h`.

### M6 — Release **v0.2.0** (modernisation) ← ✅ **PUBLIÉE** (2026-06-15)
- Cible révisée v1.0.0 → **v0.2.0** : console-onglets/demo différés → pas un 1.0. dmaHD hors périmètre.
- [x] CI verte ; CHANGELOG `[0.2.0]` ; tag `v0.2.0` poussé → release GitHub générée (provenance Sigstore)
- [x] Packaging release assaini ensuite : mode **dlopen** (client unique Vulkan + fallback OpenGL,
      `BINARIES.txt`) au lieu de 2 binaires statiques au nommage trompeur (PR #3)

### Post-release — Hygiène GitHub & sécurité (2026-06-15)
- [x] Licence : `COPYING.txt` (doublon) supprimé → fin du double onglet (PR #5)
- [x] CodeQL : suite `security-extended` + exclusion vendored (`.github/codeql/codeql-config.yml`)
      → **~1279 → ~172** alertes ; triage documenté **`docs/security-triage.md`** ; **8 dismissées**
      (FP vérifiés) ; ~164 backlog (patterns upstream, à trier au fil de l'eau)
- [x] Dependabot : alertes + security updates activées ; PR de bump mergées ; **grouping** (PR #11/#12)
- [x] cppcheck caché (build-dir + actions/cache) → ~7 min → secondes (PR #4)

### M7 — Qualité & durcissement (modernisation 2026) ← ✅ **MERGÉ sur main** (PR #1, 2026-06-15)
> Issu de l'audit `AUDIT.md`. 8 commits mergés (`1a6a244f`), CI verte sur main (8/8 jobs).
- [x] **Phase 1** Tests & fuzzing — `tests/unit/` (Unity, 8 tests q_math vérifiés) +
      `tests/fuzz/fuzz_info` (libFuzzer Info_*) + `support/stubs.c`, jobs CI `unit`/`fuzz-smoke`
- [x] **Phase 2** Durcissement CI — CodeQL, dependabot, **SHA-pinning** de toutes les actions,
      build provenance (release.yml). DIFFÉRÉ : job MSVC (non validable hors Windows)
- [x] **Phase 3** Build & versioning — `git describe`→`SVN_VERSION`, `-std=gnu99`, **durcissement
      Linux** (stack-protector-strong + full RELRO + FORTIFY ; PIE déjà actif → JIT non impacté ;
      vérifié build+readelf+init serveur)
- [x] **Phase 4** Env & polish — devcontainer/Dockerfile, CODE_OF_CONDUCT, badges README, ARCHITECTURE.md
- [ ] **Restant (validation externe)** : job MSVC + rebrand `.sln` (Windows) ; valider JIT en jeu
      sous durcissement (faible risque) ; optionnels (codecov, FUNDING, capture README)

### M8 / Feature #1 — Identity Switching ← ✅ **Implémenté** (2026-06-15, session 5)
- **Branche** : `feature/identity-switching` (changements non commités sur working tree)
- [x] Implémentation `code/client/cl_main.c` (+238 lignes) :
      `CL_SaveIdentity_f` / `CL_LoadIdentity_f` / `CL_ListIdentities_f` + tab completion,
      `CL_NameRotate()` (appelée depuis `CL_Connect_f`), cvars `cl_identity` / `cl_nameRotate`
      (enregistrées dans `CL_Init` + auto-load au démarrage), `Cmd_RemoveCommand` dans `CL_Shutdown`
- [x] Build vérifié (recompilation forcée `cl_main.c` → link OK, zéro warning/erreur)
- [x] Documentation : `docs/CVARS.md`, `docs/urt-features.md`, `ROADMAP.md` (M8), `CHANGELOG.md`
- [ ] **Reste** : commit + push + PR + test en jeu

---

## 📊 Métriques

| Indicateur | Valeur |
|------------|--------|
| Commits urbanterror-slim analysés | 66 |
| Fonctionnalités déjà intégrées | 13 |
| Fonctionnalités intégrées en M0 | 4 (sécurité + modversion + window margins) |
| Fonctionnalités manquantes restantes | 3 différées (console-onglets, demo UrT, dmaHD optionnel) |
| Milestones roadmap | 8 (M0–M7) |
| Milestones terminés | 6 (M0–M4 + M7) ; M5 quasi clos ; M6 (release) à venir |
