# Active Context — Urban Terror Optimized

## Dernière mise à jour
2026-06-15 — Session 4 : audit fondations + M7 (modernisation) **mergé sur main** (PR #1)

## Milestone actuel : M7 ✅ MERGÉ — prochaine cible : M6 (release)

**M7 — Qualité & durcissement** : déclenché par un audit demandé par l'utilisateur (verdict :
fondations M0–M4 solides). Rapport dans **`AUDIT.md`**, suivi `ROADMAP.md` (M7). Livré en
8 commits, **mergé sur `main` via PR #1** (`1a6a244f`), **CI verte sur main** (8/8 jobs, dont
cppcheck rendu informationnel). Branche `feature/modernization-m7`.

**M7 — 4 phases livrées et vérifiées** :
1. ✅ Tests & fuzzing — `tests/` (Unity 8 tests q_math + libFuzzer Info_*), jobs CI `unit`/`fuzz-smoke`.
2. ✅ Durcissement CI — CodeQL, dependabot, SHA-pinning toutes actions, build provenance.
3. ✅ Build & versioning — `git describe`→`SVN_VERSION` (vérifié binaire), `-std=gnu99`.
4. ✅ Env & polish — devcontainer/Dockerfile, CODE_OF_CONDUCT, badges README, ARCHITECTURE.md.

**Différé M7 (validation externe requise)** : job MSVC + rebrand `.sln` (Windows natif),
flags durcissement PIE/RELRO (test JIT VM en jeu), optionnels (codecov, FUNDING).

## Prochaine cible recommandée : M6 — Release

Les fondations ont atteint le rendement décroissant → pivoter vers la **livraison**. M5 quasi
clos : cvars + tellme faits ; **referee fix absorbé par tellme** (réconcilié 2026-06-15) ;
console-à-onglets / demo / dmaHD restent différés (réimpl + tests en jeu) et **ne bloquent pas**
une release.

**Décisions prises (2026-06-15)** : prochaine release = **v0.2.0** (modernisation ; v1.0.0
réservé à l'après console-onglets + demo) ; **dmaHD hors périmètre** (différé). CHANGELOG daté
`[0.2.0] - 2026-06-15`. **Reste à faire (action utilisateur, push m'est refusé)** :
`git tag v0.2.0 && git push origin v0.2.0` → déclenche `release.yml` (build multi-plateforme +
provenance Sigstore). Après tag, la bannière de version affichera `v0.2.0` (versioning git-describe).

**Notes techniques clés** (cf. AUDIT.md) :
- `git describe` retombe sur le tag legacy `latest` → filtrer `--match 'v[0-9]*'` (= `v0.1.0-…`),
  non-destructif, pas besoin de supprimer le tag.
- `-std=gnu99` (pas `c99`) : le code utilise extensions GNU.
- `tests/` hors-arbre → zéro impact sur l'alignement upstream / cherry-pick.

---

## (clos) Milestone précédent : M5 — Features UrT (PARTIEL ; fondations M0–M4 terminées)

**Fondations terminées (M0–M4)** : CI vert, docs, identité, conventions, outillage.

**M5 — fait** :
- Cvars serveur (`sv_sayprefix`/`sv_tellprefix`, `sv_nofallDamage`, `sv_infiniteStamina`) —
  poussé (`aeac0131`).
- Tellme / `messagemode5` — `/cmd` = commande client + self-tell `!`/texte + historique +
  rappel ↑/↓. **5 commits locaux non poussés** (`d158fc96`→`18b869d0`) — push bloqué par deny-rule.

**M5 — DIFFÉRÉ** (gros ports structurels/invasifs sur code slim divergé, intestables à l'aveugle,
nécessitent réimpl fraîche + tests runtime itératifs) :
- Console à onglets (`855ae13b`) — cherry-pick non viable (10 conflits, non guardé).
- Demo UrT (`9579fc7e`) — cherry-pick = 5 conflits (demo-parsing modernisé) ; guardé, faisable
  avec tests `.urtdemo`/`.dm_`.
- dmaHD (`213e0e5d`) — optionnel, décision en attente.

**État** : consolidation. À faire par l'utilisateur : `git push origin main` (5 commits tellme).
Reprise des features différées quand le test en jeu en continu est possible.

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
