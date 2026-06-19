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
      `.claude/agents/context-manager.md` ; dédup `.clinerules`/`.instructions.md`
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
- [x] **Tellme / messagemode5** — réimplémenté proprement (commits `d158fc96`, `450d2f23`) :
      mode chat où le texte/`!cmd` part en self-tell serveur (commandes bot invisibles), et
      `/cmd` exécute une **commande client** localement (`Cbuf`, pas d'envoi serveur) + historique
      + rappel ↑/↓. Intègre l'équivalent du referee fix `b834398f` (gating via `chat_cmdMode`).
      `tellme <text>` = variante bind (self-tell direct).
- [x] **Console à onglets** (`855ae13b`) — **FAIT** (réimplémentation fraîche, 2026-06-15, testée
      en jeu). Console unique → `consoles[]` (All/General/Frag/Chat) ; routage des lignes frag/chat
      via les octets marqueurs du mod ; barre d'onglets visuelle (liseré rouge) ; bascule
      souris (clic G/D) + shift+←/→ ; cvar `con_tabs` + commandes `con_nexttab`/`con_prevtab`.
- [ ] **Demo UrT** (`9579fc7e`, `ec15c2c6`) — **DIFFÉRÉ**. Port invasif (218 lignes `USE_URT_DEMO`
      dans le pipeline demo ; cherry-pick = 5 conflits car notre demo-parsing a été modernisé
      par Quake3e — `DEMOEXT`/`com_protocol`). Faisable (guardé → défaut `.dm_` inchangé) mais
      nécessite des tests `.urtdemo`/`.dm_` en jeu. À reprendre avec validation runtime.

### Cvars serveur optionnelles
- [x] `sv_sayprefix` / `sv_tellprefix` (`dd52e95f`) — porté manuellement, build OK
- [x] `sv_nofallDamage` (`be301ebf`) — porté manuellement, build OK
- [x] `sv_infiniteStamina` (`bbb587d4`) — porté manuellement, build OK
- [x] Referee fix (`b834398f`) — `code/client/cl_keys.c` — **absorbé par la réimplémentation de
      tellme** (gating `chat_cmdMode`, voir `cl_keys.c:511-553`). Aucune action séparée requise.

### Optionnel
- [x] dmaHD (`213e0e5d`, ~1524 lignes) — **DIFFÉRÉ / hors périmètre** (décision utilisateur
      2026-06-15). Masse de code optionnelle, faible priorité vs livrer ; réévaluable si
      demande joueurs.

---

## M6 — Release **v0.2.0** (modernisation)

**Risque : faible.** Cible révisée → **v0.2.0** (décision 2026-06-15) : base solide + premières
features, mais console-onglets/demo encore différés → pas encore un 1.0. `v1.0.0` reste le jalon
final une fois ces features livrées.

- [x] Build test complet toutes plateformes via CI (vert sur `main` après merge M7)
- [x] `CHANGELOG.md` : entrée `[0.2.0] - 2026-06-15` consolidée
- [ ] Validation finale (`git status` propre, README à jour, build OK) puis **tag `v0.2.0`** +
      `git push origin v0.2.0` → déclenche `release.yml` (build multi-plateforme + provenance)
- [ ] (futur) `v1.0.0` après console-onglets + demo UrT

---

## M7 — Qualité & durcissement (modernisation 2026)

*Issu de l'audit du 2026-06-15 (voir [`AUDIT.md`](AUDIT.md)). Combler les lacunes vers un
projet « le plus moderne possible » : tests, sécurité CI, build, env reproductible.*
**Risque : moyen** (Phases 1–2 hors `code/` ; Phase 3 touche le Makefile → tests build requis).

### Phase 1 — Tests & fuzzing 🔴 ✅ (2026-06-15)
- [x] `tests/unit/` : harness **Unity** vendored + `tests/Makefile` séparé (ASan/UBSan défaut)
- [x] Tests unitaires `q_math.c` (8 tests, vérifiés) ; `support/stubs.c` pour `q_shared.c`
- [x] `tests/fuzz/fuzz_info` : **libFuzzer** sur `Info_*` (q_shared.c) + corpus seed
- [x] CI : jobs `unit` + `fuzz-smoke` dans `ci.yml`
- [ ] *(extension future)* `cvar.c`/`md4`/`md5`, fuzz `COM_Parse`/`msg.c`

### Phase 2 — Durcissement CI / sécurité 🟠 ✅ (2026-06-15, sauf MSVC)
- [x] `.github/workflows/codeql.yml` (c-cpp, PR + hebdo)
- [x] `.github/dependabot.yml` (github-actions)
- [x] Épinglage des actions sur **SHA** (`ci.yml` + `release.yml` + `codeql.yml`)
- [x] Build provenance (`actions/attest-build-provenance`) dans `release.yml`
- [ ] **DIFFÉRÉ** Job **MSVC** — non validable hors Windows (à coupler au rebrand `.sln`)
- [ ] *(optionnel)* coverage codecov

### Phase 3 — Build & versioning 🟠 ✅ (2026-06-15, durcissement différé)
- [x] Versioning dynamique `git describe --match 'v[0-9]*'` → `-DSVN_VERSION` (hook
      `q_shared.h:30-32`, **0 modif code**) — vérifié dans le binaire
- [x] `-std=gnu99` explicite — build complet clean validé
- [x] Durcissement Linux : `-fstack-protector-strong`, full RELRO (`-z relro,-z now`),
      `-D_FORTIFY_SOURCE=2` (release). PIE déjà actif (défaut toolchain) → JIT non impacté.
      Vérifié : build clean + readelf (RELRO/FORTIFY/canary) + init serveur OK. Reste : valider
      le JIT en jeu (mod QVM) — faible risque vu que PIE était déjà actif.
- [ ] *(optionnel)* rebranding `quake3e.sln` → `urbanterror-optimized.*`

### Phase 4 — Env reproductible & polish 🟡/🟢 ✅ (2026-06-15)
- [x] `.devcontainer/` + `Dockerfile` (toolchain one-command, miroir CI)
- [x] `CODE_OF_CONDUCT.md` (Contributor Covenant 2.1)
- [x] README : badges + liens CoC/ARCHITECTURE/AUDIT
- [x] `ARCHITECTURE.md` (promotion `.context/systemPatterns.md`)
- [ ] *(optionnel)* `.github/FUNDING.yml` ; capture/GIF README

---

## M8 — Features post-v0.2.0 (nouvelles features joueurs)

*Features issues du brainstorming `docs/FEATURE_IDEAS.md` (155 idées).*
**Risque : faible** (features client-side, zéro impact protocole).

### Feature #1 — Identity Switching ✅ (2026-06-15)
- **Branche** : `feature/identity-switching`
- **Cas d'usage** : un joueur veut se connecter sous différentes identités (nom, apparence,
  binds textuels, tags de clan) sans tout reconfigurer à chaque fois. Mode "incognito".
- **Commandes** : `saveidentity <name>`, `loadidentity <name>`, `listidentities`,
  `currentidentity`, `revertidentity`
- **Cvars** : `cl_identity` (nom du profil actif, lu au démarrage),
  `cl_identityRules` (auto-identité par serveur : `pattern=profile; …`)
- **Fichiers** : `identities/<name>.cfg` — profil dérivé du userinfo live (capture les cvars
  spécifiques au mod), denylist pour les clés non-identité/sensibles
- **Auto-identité** : `cl_identityRules` charge le bon profil à la connexion selon l'adresse du
  serveur (tag de clan sur serveurs clan, nom casual ailleurs) ; `revertidentity` annule
- **Compat** : ✅ 100% local (userinfo keys déjà transportées par les serveurs legacy)
- **Statut** : ✅ Implémenté, build OK. Voir [CVARS.md](docs/CVARS.md)

### Feature #2 — Dossier de download `q3ut4/download` + `fs_downloadpath` ✅ (2026-06-16)
- **Branche** : `feature/fs-downloadpath`
- **Cas d'usage** : ne pas mélanger les paks téléchargés avec le reste de `q3ut4` (backup/symlink),
  et partager un cache de download entre plusieurs installs ayant des homepaths distincts.
- **Comportement** : downloads (UDP + HTTP/cURL, `\dlmap`/`\download`) écrits dans
  `<root>/q3ut4/download/`, enregistré au search path (chargé après `FS_Restart`).
- **Cvar** : `fs_downloadpath` (init-only ; vide → `fs_homepath`) = racine du dossier `download/`.
- **Origine** : réimplémentation propre des commits slim `f92d24af` + `99f6edbb` (corrige fuite
  `CopyString`, taille de buffer, incohérence write/read) + ajout de la racine configurable.
- **Pure** : validation par checksum → insensible au sous-dossier (à confirmer en jeu).
- **Statut** : ✅ Implémenté, build OK (client+serveur). Voir [CVARS.md](docs/CVARS.md)

### Features #3–#5 — livrées depuis v0.2.0 (rattrapage)
Détails complets dans `.context/activeContext.md` (sessions 9–11).
- **#3 Harnais de test headless** (session 9, PR #24) — `assert`/`assert_cvar`, `quit <code>`,
  `scripts/headless`, `tests/integration/`, null renderer, `make smoke`/`smoke-client`.
- **#4 `cm360`** (session 10, PR #25/#26) — convertisseur `sensitivity` ↔ cm/360, cvar `m_dpi`.
- **#5 Console à onglets + `con_tabScale`** (PR #19 / #27) — onglets All/General/Frag/Chat,
  taille des titres réglable.

### Feature #6 — Scripting cfg & console (Lot 1 mergé, PR #28)
*Issu de [`docs/FEATURE_IDEAS.md`](docs/FEATURE_IDEAS.md) §« Console, configs & commandes ».
Lot 1 d'une série de 3 (Lot 2 : console UX légère `con_height`/notify ; Lot 3 : recherche
scrollback + smart condump). Voir le plan de session.*
- **Lot 1 — cœur scripting cfg** ✅ mergé dans `main` (PR #28, `e0614bf2`), `make smoke` vert :
  - `alias` / `unalias` / `unaliasall` — séquences de commandes nommées, persistées dans
    `q3config.cfg`, garde anti-récursion (1024 expansions/passe), refus de masquer une commande.
  - `if <cvar> <op> <value> <cmd>` — exec conditionnel ; réutilise le comparateur des asserts
    (`Com_Compare`, désormais public). Limité aux conditions (pas de boucles) → cbuf sûr.
  - `cvarlock` / `cvarunlock` — verrou local (flag runtime `CVAR_USER_LOCKED`, ni archivé ni réseau).
  - `time <cmd>` — profiling d'une commande (µs).
  - **Fichiers** : `cmd.c` (alias/if/time + dispatch), `cvar.c` (lock), `common.c`/`qcommon.h`
    (`Com_Compare` public + `Cmd_WriteAliases`), `q_shared.h` (flag). Test :
    `tests/integration/cases/scripting.cfg`. Doc : [CVARS.md](docs/CVARS.md).
- **Lot 2 — console UX (render-side)** ✅ implémenté, `make smoke-client` vert :
  - `con_height` (fraction d'écran couverte, 0.1–1.0) — remplace le `0.5` hardcodé de `Con_RunConsole`.
  - `con_opacity` (alpha du fond, 0–1) — appliqué aux deux chemins de `Con_DrawSolidConsole`
    (shader par défaut **et** `cl_conColor`).
  - `con_notifyLines` (0–8) + `con_notifyY` (offset px) — `Con_DrawNotify` ; `NUM_CON_TIMES` porté
    de 4 à 8 (taille du ring buffer `times[]`). `con_notifytime` (durée) existait déjà.
  - **Fichier** : `cl_console.c` (seul). Test : `tests/integration/cases/client/console-ux.cfg`.
    Doc : [CVARS.md](docs/CVARS.md) § « console appearance & notify ».
- **Lot 3** : à venir (recherche scrollback + smart condump + `con_notifyFilter`).

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
| ✅ | M6 — Release **v0.2.0** (publiée 2026-06-15 ; v1.0.0 = jalon futur) | Court | Faible |
| ✅ | M7 — Qualité & durcissement | Long | Moyen |
| → | M8 — Features post-v0.2.0 (joueur/dev) | Continu | Faible |

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
