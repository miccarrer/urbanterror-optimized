# Active Context — Urban Terror Optimized

## Dernière mise à jour
2026-06-17 — Session 12 : **scripting cfg — Lot 1** (branche `feature/cfg-scripting`, en cours). Premier lot d'une série de 3 visant à améliorer console + scripting `.cfg` (idées de `docs/FEATURE_IDEAS.md`). Livré : `alias`/`unalias`/`unaliasall` (séquences nommées, persistées dans `q3config.cfg`, garde anti-récursion), `if <cvar> <op> <value> <cmd>` (exec conditionnel), `cvarlock`/`cvarunlock` (verrou local runtime), `time <cmd>` (profiling µs). `make smoke` vert (nouveau cas `tests/integration/cases/scripting.cfg`). Build client+serveur OK. Lots 2–3 (console UX render-side) à venir.

## Session 12 : scripting cfg — Lot 1 (`alias`, `if`, `cvarlock`, `time`)

**Branche** : `feature/cfg-scripting` (depuis `main`). **Non poussée**, working tree à committer.

**Contexte** : l'utilisateur veut améliorer l'usage de la console et le scripting `.cfg`. Plan
établi (3 lots séquencés, découpés par **testabilité** : Lot 1 = scripting 100 % assertable
headless ; Lots 2–3 = console UX render-side, vérif visuelle). L'utilisateur a choisi un **lot
ciblé** pour démarrer → Lot 1 livré cette session, Lots 2–3 planifiés.

**État du code vérifié avant impl** : `vstr` (`cmd.c:428`) et `wait <N>` (`cmd.c:51`) existaient
déjà ; `alias`, `if`, cvar-lock, `time` absents. Historique console déjà persistant (`history.c`).

**Livré (Lot 1)** :
- **`alias`/`unalias`/`unaliasall`** (`cmd.c`) — liste chaînée `cmd_alias_t`, dispatch dans
  `Cmd_ExecuteString` **après** le match commande (pas de shadow d'un builtin) et **avant**
  `Cvar_Command`. Expansion via `Cbuf_InsertText` (le corps tourne avant la suite du buffer).
  **Garde anti-récursion** : `cmd_aliasRunaway` (budget 1024/passe, reset en tête de
  `Cbuf_Execute`) → `alias a "a"` n'gèle plus (testé). **Persistance** : `Cmd_WriteAliases`
  appelé dans `Com_WriteConfigToFile` (`common.c`) ; un changement d'alias arme
  `cvar_modifiedFlags |= CVAR_ARCHIVE` pour forcer la réécriture de `q3config.cfg`.
- **`if <cvar> <op> <value> <cmd>`** (`cmd.c`) — le 1er opérande est un **nom de cvar** (sa
  valeur est comparée ; choix délibéré vs littéraux, c'est ce dont les configs ont besoin).
  Réutilise le comparateur des asserts : `Com_AssertCompare` renommé **`Com_Compare`** (rendu
  **public**, message d'erreur neutralisé, proto dans `qcommon.h`). Limité aux conditions.
- **`cvarlock`/`cvarunlock`** (`cvar.c`) — nouveau flag **`CVAR_USER_LOCKED` = 0x40000**
  (`q_shared.h`, bit libre). Check ajouté en tête du gate de flags de `Cvar_Set2` (avant
  ROM/INIT/…) : si verrouillé et `!force`, refuse. Runtime-only (jamais archivé/réseau ; les
  writes moteur `force=qtrue` passent). `Cvar_Get` fait `flags |= ` → le bit survit.
- **`time <cmd>`** (`cmd.c`) — `Sys_Microseconds()` autour de `Cmd_ExecuteString`, affiche ms.μs.

**Tests** : `tests/integration/cases/scripting.cfg` (ramassé auto par le runner) — couvre
enregistrement des 7 commandes, alias define/expand/redéfinition/multi-statement/shadow-refus/
unalias, if vrai/faux/string, cvarlock blocage→unlock, time smoke. **`make smoke` : 2/2 PASS**.

**Pièges rencontrés** :
- `if` v1 comparait des **littéraux** (comme `assert`) → `if cond == 5` comparait la chaîne
  « cond ». Corrigé : 1er opérande = nom de cvar (`Cvar_VariableString`).
- Test manuel d'alias multi-statement via `+alias x "a; b"` en ligne de commande : le `;` était
  tronqué — **artefact de quoting shell**, pas un bug (dans un vrai `.cfg` les guillemets tiennent ;
  prouvé par le cas `multi` du test d'intégration et la persistance `alias greet "echo hello world"`).

**Docs mises à jour** : `docs/CVARS.md` (§ « Cfg scripting »), `ROADMAP.md` (M8 Feature #6 +
rattrapage #3–#5 + table d'ordre corrigée v0.2.0/M7/M8).

**Reste à faire** : commit + push + PR. Puis Lot 2 (console UX : `con_height`/opacité, notify
paramétrable) et Lot 3 (recherche scrollback, smart condump).

---

## Session 11 : police des onglets de console (`con_tabScale`)

**Branche** : `feature/tab-title-font` (PR #27 **mergée**, `7dc8e018`).

**Contexte** : l'utilisateur demandait « que manque-t-il pour finir tabbed-console ? ». Réponse :
**rien** — la console à onglets est livrée et mergée depuis **PR #19** (`895d66d1`). La branche
`feature/tabbed-console` restante était stale (réutilisée pour un chore de rename). Seul besoin réel :
agrandir un peu la police des **titres d'onglets**.

**Livré** :
- `Con_DrawScaledChar( x, y, w, h, ch )` dans `cl_console.c` — dessine un glyphe à une taille pixel
  arbitraire (réplique `SCR_DrawSmallChar` paramétrée).
- cvar **`con_tabScale`** (défaut **1.25**, `CVAR_ARCHIVE_ND`, `Cvar_CheckRange` 1.0–3.0) : taille des
  titres d'onglets en multiple de la police du corps. Ajustable à chaud (testé en jeu, l'utilisateur a
  choisi 1.25). Les boîtes d'onglets s'élargissent et les glyphes se centrent verticalement.
- Doc `docs/CVARS.md` § « tabbed console » (con_tabScale, con_tabs, con_nexttab/con_prevtab).

**Saga CodeQL (leçon)** : la cvar `con_tabScale` (valeur « non contrôlée » pour CodeQL) alimentait
l'arithmétique de layout → 5 alertes `cpp/uncontrolled-arithmetic` (high). 4 itérations de durcissement :
(1) clamp `cw`/`chh` borne haute → 5→3 ; (2) clamp longueur titre `nlen≤32` + boucle bornée ; (3) clamp
`cw`/`chh` **fermé [1,256]** (borne basse manquait → `cw` pouvait sembler négatif dans une multiplication).
**Aucune n'a satisfait CodeQL** sur les produits `(nlen+2)*cw` / `(k+1)*cw` (query notoirement bruyante,
cf. triage doc). Les valeurs étant **prouvablement bornées** (aucun overflow réel), les **3 alertes ont été
dismissées en false-positive** (UI GitHub par l'utilisateur ; le dismiss via `gh api` a été **refusé par le
classifier** d'auto-mode comme action de suppression de sécurité non autorisée — demander l'accord explicite).
Consigné dans `docs/security-triage.md` § « Dismissed on PRs ». Les clamps défensifs sont **gardés** (code sain).

**Reste à faire utilisateur** : `git push origin main` (ce commit context+triage) ; supprimer la branche
remote mergée `feature/tab-title-font` (`git push origin --delete feature/tab-title-font`).

---

## Session 10 : `cm360` + diagnostic écart DPI

**Branche** : `docs/cm360-dpi-tip` (PR #26, docs-only). Feature `cm360` déjà sur `main` (PR #25 mergée).

**Feature livrée** : commande `cm360` (`cl_input.c`) — convertit `sensitivity` ↔ **cm/360** (distance physique pour un tour complet). `cm360` affiche, `cm360 <valeur>` règle `sensitivity`. Nouveau cvar `m_dpi` (défaut 800, `CVAR_ARCHIVE_ND`). Formule : `cm/360 = 360 × 2.54 / (m_dpi × sensitivity × m_yaw)`. Doc `docs/CVARS.md`.

**Le bug signalé** : `cm360` indiquait 51.95 cm/360 (sens 1, dpi 800, m_yaw 0.022) mais le ressenti in-game était ~26 cm — un facteur **2×** propre.

**Diagnostic (méthode → conclusion)** :
- Hypothèses **réfutées** une à une : mod (`cgameSensitivity=1` confirmé en jeu actif), accel KDE (Flat/neutre, off → pas d'effet), XWayland (idem en Wayland natif), **display scale** (`kscreen-doctor` : `Scale: 1` sur les 2 écrans, natif 2560×1440 → pas de HiDPI).
- Instrumentation temporaire (commande `mousecount` dans `cl_input.c`, **retirée depuis**) : un trait de 25.7 cm (3 cartes bancaires) → **16528 comptes** côté moteur = 643 comptes/cm ≈ **1620 DPI**.
- **Lecture brute evdev** (`/dev/input/event18`, sonde Python, avant libinput/SDL) : ~15700–16900 comptes pour 25.7 cm → **~1600 DPI matériel réel**. SDL transmet fidèlement (pas de doublement).
- `ratbagctl` : presets G305 = 400/800/**1600**/3200/200 ; `res 1: 800 (active)` selon ratbagd **mais** profil `Profile 0: (disabled)` → ratbagd ne pilote pas réellement la souris. Le bouton DPI-shift avait basculé le preset onboard sur 1600.

**Verdict** : aucun bug moteur/SDL/cm360. `m_dpi 800` mentait sur le vrai DPI (1600). L'utilisateur joue en réalité à 1600 DPI / ~26 cm/360 depuis des années (mémoire musculaire calibrée dessus).

**Résolution utilisateur** : a mis **`m_dpi 1600`** (cm360 lit juste désormais) + `ratbagctl … profile 0 enable`. Recommandation donnée : garder 1600 partout (y compris CS2 — pas de souci, raw input), ne pas « corriger » à 800 sauf à vouloir tout recalibrer.

**Leçon documentée (PR #26)** : `cm360` n'est juste que si `m_dpi` reflète le **DPI matériel réel**. Les configurateurs (Piper/libratbag) peuvent afficher un preset non appliqué ; vérifier via comptes kernel bruts. → note troubleshooting dans `docs/CVARS.md`.

**Reste à faire utilisateur** : merge PR #26 ; supprimer les 16 branches remote mergées (`git push origin --delete …`, commande fournie).

---

## Session 9 : harnais de test runtime headless (autonomie agent / CI)

**Branche** : `feature/agent-test-harness` (depuis `feature/fs-downloadpath`) — **8 commits, non poussés**, working tree propre.

**Objectif** : fermer la boucle de feedback runtime — pouvoir build → lancer headless →
piloter par script → asserter via code de sortie, **sans intervention humaine**.

**Capacités livrées (à connaître pour les prochaines sessions)** :

- **Tier 0 — moteur scriptable** (`common.c`, `unix_main.c`, `win_main.c`, `qcommon.h`) :
  - `quit <code>` propage un vrai code de sortie (`Sys_Quit(int)`). ⚠️ `Sys_Exit` a
    `assert(code==0)` hors-NDEBUG → code ≠ 0 propre seulement en build **release** (le `make` par défaut).
  - commandes `assert <a> <op> <b>` / `assert_cvar <name> <op> <value>` (ops num. `== != < <= > >=`,
    chaînes `eq`/`ne`) → impriment `ASSERT PASS`/`FAIL`, un FAIL arme le code de sortie.
  - + `assert_cvar_flag <name> <FLAG>` (flags `CVAR_*`), `assert_command <name>` (`Cmd_Exists`),
    `assert_file <path>` (existe dans le **chemin d'écriture** `fs_homepath/fs_gamedir` — download/démo).
  - cvar `com_logTimestamps 0` → en-tête de log sans date (golden diffable).
- **Tier 0.5 — durcissement CI** : job `integration-asan` rejoue la suite serveur sous
  `-fsanitize=address,undefined` (`ASAN_OPTIONS=detect_leaks=0`) → chaque test devient un check
  mémoire/UB. La cible `release` propage désormais `EXTRA_LDFLAGS`.
- **Tier 1 — headless + runner** :
  - `scripts/headless` : lance un binaire headless (SDL `dummy`, homepath temp isolé, `net_enabled 0`).
    Modes : **serveur** (défaut), **client** (`URT_CLIENT=1`), **client+UI réelle** (`URT_UI=1`, needs install).
  - `tests/integration/` : runner `run.sh [--client]`, fixture minimal `fixtures/q3ut4/default.cfg`
    (boote **sans pk3 ni install**), cas `cases/*.cfg` (serveur) + `cases/client/*.cfg` (client).
  - **null renderer** `code/renderernull/tr_null.c` — module dlopen `..._null_<arch>.so`,
    `+set cl_renderer null`, aucune fenêtre/contexte GL. Additif (zéro fichier upstream touché),
    gardé par `USE_RENDERER_NULL` (on par défaut).
  - `cl_noUI 1` (`cl_main.c`, `CVAR_INIT`) : saute la VM UI (`uivm` NULL déjà toléré partout) →
    **client boote 100 % headless sans aucun asset**.
- **Cibles & CI** : `make smoke` (serveur) + `make smoke-client` (client + null renderer, sans
  GL/Vulkan) — tous deux **install-free**. Job CI `integration` lance les deux suites.

**Comment je m'en sers (mémo)** :
```bash
make smoke              # asserts qcommon via serveur dédié (CI)
make smoke-client       # asserts client (binds/cvars/console) via client headless (CI)
scripts/headless +assert 1 == 1 +quit                      # serveur ad hoc
URT_CLIENT=1 scripts/headless +assert_cvar cl_noUI == 1 +quit   # client install-free
URT_CLIENT=1 URT_UI=1 URT_BASEPATH=/home/michael/Jeux/UrbanTerror43 \
  scripts/headless ... +quit                               # client + vraies VM UI/cgame (local)
```

**Reste à faire** : push + PR éventuelle. Étoffer `cases/` au fil des features (chaque future
feature joueur/dev arrive avec son `.cfg` d'intégration). Tier 2/3 (introspection JSON, déterminisme)
non commencés — voir `docs/FEATURE_IDEAS.md` § Autonomie de l'agent.

---

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
