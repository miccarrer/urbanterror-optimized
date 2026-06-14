# Active Context — Urban Terror Optimized

## Dernière mise à jour
2026-06-14 — Session 3 : fondations/outillage + réorganisation de la roadmap

## Milestone actuel : M3 — CI/CD (M1 et M2 terminés)

M1 (identité/hygiène) et M2 (gouvernance/qualité) **terminés et poussés**. Prochaine étape :
**M3 — CI/CD** (scinder `build.yml` → `ci.yml` + `release.yml` ; clang-format, cppcheck,
clang-tidy, build ASan/UBSan, cache ; artifacts `urbanterror-optimized-*` + notes auto).

---

### Historique récent
## (ancien) M1 — Identité & hygiène des fichiers

Roadmap réorganisée **« fondations d'abord »** (M0–M6, voir `ROADMAP.md`). M0 posé.

**M1 — partie locale terminée et commitée** (6 commits) : abandon CMake, scripts/ (+ fix chemin),
README UrT, LICENSE, purge docs Q3A legacy, restructuration `docs/` (legal/, analysis/, BUILD).

**M1 — actions GitHub** :
1. ✅ `gh repo rename urbanterror-optimized` (fait — repo public renommé)
2. ✅ `git remote set-url origin …/urbanterror-optimized.git` (fait)
3. ✅ Tag `v0.1.0` créé et poussé ; 26 tags Quake3e + tag `latest` purgés (local + remote)
4. ✅ `git push origin main` fait ; branche `master` legacy supprimée, défaut GitHub = `main`
Le chemin local du repo ne change pas → symlink/MOC vault inchangés. **M1 terminé.**

> Contrainte structurante : `code/` reste **aligné upstream Quake3e** (cherry-pick préservé) ;
> on ne réorganise que le **non-code** (racine, `docs/`, `scripts/`, CI).

## Ce qui a été fait lors de la dernière session (2026-06-14)

### Session 1 — Planification initiale (agent précédent)
1. ✅ Analyse complète des 66 commits de `omg-urt/urbanterror-slim`
2. ✅ Génération de `ANALYSIS_REPORT.md` (11 fonctionnalités manquantes identifiées)
3. ✅ Création de `ROADMAP.md` (6 phases détaillées)
4. ✅ Configuration du Memory Bank (`.clinerules`, `.instructions.md`, `.agents/`, 6 fichiers `memory-bank/`)

### Session 2 — Phase 1A + nettoyage (agents Copilot + Claude)
1. ✅ Phase 1A implémentée et commitée (branding, build config, sécurité, window margins, modversion)
2. ✅ CLAUDE.md créé pour Claude Code
3. ✅ Git remotes reconfigurés : `origin` → `miccarrer/Quake3e`, `upstream` → `ec-/Quake3e`
4. ✅ Feature branch mergée en FF dans `main`, worktree supprimé, branches nettoyées
5. ✅ `main` pushé sur `origin`
6. ⚠️ Phase 1B (console tabs, tellme) : WIP incomplet discardé — à recommencer proprement

## Décisions en attente / points ouverts

- ~~**JIT NaN fix** (`56f03bca`)~~ : ✅ Résolu (2026-06-14) — déjà présent via Quake3e upstream (`56f03bca` + `cdb374ec` dans l'historique de `main`). Voir `progress.md`.
- **dmaHD** : Intégrer ou non ? (1524 lignes de code audio, complexité moyenne — M5 du ROADMAP)
- **Décision validée** : abandon de CMake (build = Makefile + MSVC) — à exécuter en M1.
- **Décision validée** : rename repo GitHub + tags → `v0.1.0` à faire en M1 (action externe).

## Prochaine étape recommandée

**M1 — Identité & hygiène des fichiers** (voir `ROADMAP.md`). Démarrer par : suppression de
CMake, réécriture README, nettoyage/restructuration `docs/`, puis actions GitHub
(rename + `v0.1.0`). Faible risque, aucun fichier de `code/` touché.
