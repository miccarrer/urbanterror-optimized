# Active Context — Urban Terror Optimized

## Dernière mise à jour
2026-06-16 — Session 8 : test en jeu de la feature #2 → **bug `fs_gamedir` trouvé & corrigé** + affinage identity (denylist vestiges Q3). Build OK.

## Session 8 : correctifs issus du test en jeu (download + identity)

**Branche** : `feature/fs-downloadpath` — 4 commits ajoutés (`6398949c`→`e6603912`), **non poussés**, working tree propre.

**Bug `fs_gamedir` (régression de la feature #2)** — `6398949c` `fix(fs)` :
`FS_Startup` enregistre `<root>/q3ut4/download` via `FS_AddGameDirectory`, qui **écrase la globale
`fs_gamedir`**. En jeu vanilla (pas de mod), rien ne la restaurait → `fs_gamedir` restait
`q3ut4/download`, et **toutes** les écritures (q3config, identités, démos, screenshots) atterrissaient
sous `q3ut4/download/`. Cassait aussi la feature identity (save dans `download/identities/`, load/règles
cherchaient dans `identities/`). Corrigé : save/restore de `fs_gamedir` autour de l'enregistrement.

**Affinage identity** (suite au test en jeu) :
- `3f93b6e6` `refactor(client)` : denyliste les **8 vestiges Q3** inertes dans UrT —
  `model`, `headmodel`, `team_model`, `team_headmodel`, `sex`, `color1`, `color2`, `handicap`.
  Le **vrai perso** = `racered`/`raceblue`/`racefree` (menu Player Setup) ; la couleur visible = `cg_rgb` ;
  fun items = `funred`/`funblue`. `model "sarge"` (stock) → fallback **perso féminin** par défaut.
- `c8dbe967` `refactor(client)` : `listidentities` n'affiche plus `model` (denylisté) → juste le `name`.
- `e6603912` `docs(cvars)` : doc « sauver **connecté en jeu** » (sinon userinfo incomplet) + note `race*` vs `model`.

**⚠️ Leçon clé** : `saveidentity` capture le userinfo **live** → les cvars d'apparence du mod ne sont
flaggés `CVAR_USERINFO` qu'**après chargement de la cgame** (donc connecté). Sauver au menu = profil
incomplet (seules les clés userinfo de base du moteur). **À faire utilisateur** : re-`saveidentity` les
profils existants en jeu (le `loadidentity` exécute le `.cfg` tel quel → vieilles lignes vestiges encore appliquées).

## Feature #2 : Dossier de download `q3ut4/download` + `fs_downloadpath` ✅ (build OK, testé en jeu)

**Branche** : `feature/fs-downloadpath` (commit initial `8bb05a5e` + fix `6398949c`, **non poussé**, working tree propre)

**Objectif** : ranger les paks téléchargés dans `<root>/q3ut4/download/` (pas mélangés au game dir →
backup/symlink) + cvar `fs_downloadpath` (racine configurable) pour partager un cache de download
entre installs à homepaths distincts (cas d'usage réel de l'utilisateur : 2 clients, homepaths séparés).

**Origine** : réimplémentation propre des commits slim `f92d24af` + `99f6edbb` (corrige fuite
`CopyString`, taille buffer `Com_sprintf`, incohérence write/read gamedir) + ajout racine configurable.
Couvre UDP + HTTP/cURL.

**Fichiers** : `files.c` (cvar + `FS_DownloadRoot` + helpers `FS_Download_*` via base-param + search dir
dans `FS_Startup`), `qcommon.h` (protos), `cl_main.c` (`CL_NextDownload` réécrit le localName + checksum),
`cl_parse.c` (UDP), `cl_curl.c` (cURL + dlmap). Docs : CVARS, urt-features, CHANGELOG, ROADMAP (#2), slim-comparison.

**Reste à faire** : push + PR + **test en jeu** (2 clients pour valider le partage).
**⚠️ Point de test n°1** : serveur **pure** — un pak en gamedir `q3ut4/download` est référencé comme
`q3ut4/download/foo` (`FS_ReferencedPakNames`). La pureté étant par **checksum**, ça devrait passer
(slim l'a en prod), mais à confirmer (risque de kick si un serveur valide les *noms*).

---

## Feature terminée : Identity Switching (#1) ✅ MERGÉE

**PR #20 mergée sur `main`** (`bfc4ff5f`). Branche `feature/identity-switching` supprimée.
Le rename `memory-bank/ → .context/` est arrivé sur main via cette PR. **Reste : test en jeu.**
Ménage effectué : 13 branches locales obsolètes supprimées ; ne restent que `main`.

**Objectif** : Permettre aux joueurs de switcher d'identité (nom, apparence, tags de clan) sans
toucher aux réglages de jeu (graphismes, réseau, etc.).

**Commandes** : `saveidentity <name>`, `loadidentity <name>`, `listidentities`,
`currentidentity`, `revertidentity`
**Cvars** : `cl_identity` (profil actif, lu au démarrage),
`cl_identityRules` (auto-identité par serveur : `pattern=profile; …`)
**Fichiers** : `identities/<name>.cfg` — profil **dérivé du userinfo live** (capture les cvars
spécifiques au mod), denylist pour les clés non-identité/sensibles (rate, snaps, cl_guid, password,
cl_anonymous, …)
**Compat** : ✅ 100% local (userinfo keys déjà transportées par les serveurs legacy)

**Évolution depuis la conception initiale** (cf. messages de commit) :
- `cl_nameRotate` **abandonné** (UrT identifie par `cl_guid`, pas par nom → aucun bénéfice
  anti-tracking, changeait le nom silencieusement).
- Profil dérivé du userinfo live au lieu d'une liste fixe Quake3-stock.
- Ajout `revertidentity` (undo 1 niveau, session) + `currentidentity` (drift) + `cl_identityRules`
  (auto-identité par serveur, remplacement honnête de nameRotate).
- `saveidentity` fixe le profil sauvé comme identité active ; `cl_anonymous` exclu (write-protected
  in-game par q3ut4).

**Implémentation** : tout dans `code/client/cl_main.c`, commits `41ca3bcc`→`e0afda38`.

**Reste à faire** : push + PR + test en jeu.
**Bug mineur connu** : `CL_Shutdown` ne fait `Cmd_RemoveCommand` que pour 3/5 commandes
(`revertidentity` + `currentidentity` manquent) — sans impact pratique, à corriger au prochain passage.

## Post-release : hygiène GitHub & sécurité (2026-06-15)

Après la publication de **v0.2.0** (release dlopen, cf. plus bas), série de durcissements repo :
- **Licence** : `COPYING.txt` (doublon byte-identique de `LICENSE`) supprimé → fin du double
  onglet GitHub (PR #5).
- **CodeQL** : config dédiée `.github/codeql/codeql-config.yml` — suite **`security-extended`**
  (au lieu de `security-and-quality`) + **exclusion du code vendored** → alertes **~1279 → ~172**.
  Triage documenté dans **`docs/security-triage.md`** (politique : pas de réécriture de masse du
  code upstream ; on classe Fix / Won't-fix / Backlog). **24 alertes dismissées** (vérifiées FP).
  **Surface réseau entièrement examinée** (`msg.c`/`net_chan.c`/`net_ip.c`/`sv_*`/`cl_main`) :
  **aucun vrai bug** (16 `uncontrolled-arithmetic` toutes FP — valeurs bornées, timing interne, ou
  l'arithmétique = le garde-fou ; fragment reassembly `net_chan.c` correctement borné). Le netcode
  ioquake3/Quake3e tient. ~148 backlog = code **non-réseau** (audio snd_mix, render tr_*, + vendored
  libjpeg/libvorbis que `paths-ignore` n'exclut pas pour le C compilé — limite à revoir).
- **Dependabot** : alertes + security updates **activées** ; toutes les PR de bump mergées ;
  **grouping** configuré (PR #11) → bumps d'actions futurs en une seule PR (déjà effectif, PR #12).
- **cppcheck** : caché (`--cppcheck-build-dir` + actions/cache) → ~7 min → secondes.

**État repo** : propre, aucune PR ouverte, CI verte.

## Milestone (clos) : M7 ✅ MERGÉ — release v0.2.0 publiée

**M7 — Qualité & durcissement** : déclenché par un audit demandé par l'utilisateur (verdict :
fondations M0–M4 solides). Rapport dans **`AUDIT.md`**, suivi `ROADMAP.md` (M7). Livré en
8 commits, **mergé sur `main` via PR #1** (`1a6a244f`), **CI verte sur main** (8/8 jobs, dont
cppcheck rendu informationnel). Branche `feature/modernization-m7`.

**M7 — 4 phases livrées et vérifiées** :
1. ✅ Tests & fuzzing — `tests/` (Unity 8 tests q_math + libFuzzer Info_*), jobs CI `unit`/`fuzz-smoke`.
2. ✅ Durcissement CI — CodeQL, dependabot, SHA-pinning toutes actions, build provenance.
3. ✅ Build & versioning — `git describe`→`SVN_VERSION` (vérifié binaire), `-std=gnu99`.
4. ✅ Env & polish — devcontainer/Dockerfile, CODE_OF_CONDUCT, badges README, ARCHITECTURE.md.

**Durcissement Linux FAIT (2026-06-15)** : stack-protector-strong + full RELRO + FORTIFY (PIE
déjà actif → JIT non impacté ; vérifié build/readelf/init serveur ; reste valider JIT en jeu, faible risque).
**Différé M7 (validation externe)** : job MSVC + rebrand `.sln` (Windows natif) ; optionnels (codecov, FUNDING).

## Prochaine cible recommandée : M6 — Release

Les fondations ont atteint le rendement décroissant → pivoter vers la **livraison**. M5 quasi
clos : cvars + tellme faits ; **referee fix absorbé par tellme** ; **console à onglets FAITE**
(2026-06-15, réimpl fraîche testée en jeu, branche `feature/tabbed-console` — workflow implémente→
teste→itère qui débloque les features « intestables à l'aveugle »). Restent différés : demo UrT
+ dmaHD (ne bloquent pas une release).

**Décisions prises (2026-06-15)** : release = **v0.2.0** (modernisation ; v1.0.0 réservé à
l'après console-onglets + demo) ; **dmaHD hors périmètre**. **v0.2.0 TAGUÉE & PUBLIÉE** (release
GitHub générée, provenance Sigstore OK).

**Correctif packaging release (branche `fix/release-dlopen-packaging`, non mergée)** : la
release v0.2.0 livrait 2 clients **statiques** (`USE_RENDERER_DLOPEN=0`) au nommage trompeur
(le binaire « par défaut » sans suffixe était l'OpenGL, le Vulkan recommandé était suffixé).
→ Basculé sur le mode **dlopen** (défaut moteur) : **un seul client** qui démarre en Vulkan et
bascule OpenGL à chaud (`\cl_renderer opengl` + `\vid_restart`), + les modules `_vulkan`/`_opengl`,
+ `BINARIES.txt` dans l'archive. `release.yml` simplifié (1 `make install`/plateforme). Vérifié
localement. **S'applique à la prochaine release** (v0.2.0 publiée reste inchangée). README : section « Running » ajoutée.

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
4. ✅ Configuration du Memory Bank (`.clinerules`, `.instructions.md`, `.agents/`, 6 fichiers `.context/`)

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
