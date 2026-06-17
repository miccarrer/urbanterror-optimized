# 💡 Feature Ideas — Urban Terror Optimized

> Backlog d'idées, **filtré par ce que ce moteur (client + serveur dédié) peut réellement
> livrer**. Ce dépôt n'est **pas** le mod : tout le HUD en jeu vit dans le cgame de q3ut4.
>
> **Focus actuel : le joueur et le développeur.** Les features serveur sont mises en pause
> (conservées en [Annexe S](#annexe-s--serveur-en-pause)). Chaque fiche cite une **accroche
> code vérifiée** quand elle en a une.

## Règle de compatibilité (contrainte absolue)

Compatibilité binaire avec les clients/serveurs legacy : **pas** de changement du protocole
réseau (`entityState_t` / `playerState_t` / `msg_t`), **pas** de nouveaux opcodes non
négociés, **pas** de modification des indices de configstrings utilisés par le mod.

**Règle d'or** : tout ce qui passe par **userinfo**, **servercommands** (texte),
**configstrings non-utilisées**, **cvars** ou **commandes console** est « gratuit » côté
compat — les legacy ignorent ce qu'ils ne connaissent pas. Tableau en
[Annexe B](#annexe-b--références-de-compatibilité).

## 🎯 Périmètre — lire avant tout

| Tag | Signification | Livrable ici ? |
|-----|---------------|:--------------:|
| `[ENGINE]` | Code client du moteur (console, démo, input, réseau, fichiers) | ✅ |
| `[AUDIO]` | Mixer moteur (`snd_dma.c`/`snd_mix.c`) | ✅ |
| `[RENDERER]` | Renderers `renderervk`/`renderer` | ✅ (avec réserves) |
| `[DEV/CI]` | Outillage développeur / CI | ✅ |
| `[SERVER]` | Serveur dédié (`sv_*`) | ⏸️ en pause — [Annexe S](#annexe-s--serveur-en-pause) |
| `[MOD]` | **Nécessite le cgame/ui de q3ut4** — hors moteur | ❌ — [Annexe A](#annexe-a--idées-mod-dépendantes-hors-périmètre-moteur) |

> **⚠️ Le piège `[MOD]`.** `code/cgame`, `code/game`, `code/ui` existent dans le dépôt mais
> ce sont les **QVM Quake3 stock** : les joueurs UrT exécutent les QVM de **q3ut4** (chargés
> depuis les pk3). Donc tout le **HUD en jeu** — crosshair, scoreboard, minimap, hitmarker,
> indicateurs de dégâts, deathcam, weapon wheel, sous-titres, daltonisme HUD… — vit dans le
> cgame **du mod**, que ce projet ne ship pas. Ces idées sont en Annexe A.

## ✅ Déjà livré (ne pas re-proposer)

- **Identity switching** (`saveidentity`/`loadidentity`/règles par serveur) — voir `docs/CVARS.md`.
- **Console à onglets** (All/General/Frag/Chat).
- **Dossier de download `q3ut4/download/` + `fs_downloadpath`**.
- **Cvars serveur** : `sv_sayprefix`/`sv_tellprefix`, `sv_nofallDamage`, `sv_infiniteStamina`.
- **tellme / `messagemode5`**.

---

## 📊 Shortlist priorisée (joueur + développeur)

| Feature | Tag | Difficulté | Impact | Statut |
|---------|-----|------------|--------|--------|
| **`quit <code>` → vrai code de sortie** | `[DEV/CI]` | très facile | élevé | 🆕 |
| **`assert` / `assert_cvar` (scripts auto-vérifiants)** | `[DEV/CI]` | très facile | élevé | 🆕 |
| **Mode log déterministe (`com_logTimestamps`)** | `[DEV/CI]` | très facile | moyen | 🆕 |
| **Run headless + `make smoke`** | `[DEV/CI]` | moyenne | élevé | 🆕 |
| **Introspection JSON (`cvarlist --json`, …)** | `[DEV/CI]` | facile | élevé | 🆕 |
| Convertisseur de sensi (`cm360`, conversion cross-game) | `[ENGINE]` | très facile | élevé | 🆕 |
| Profils de binds (export/import/hot-swap) | `[ENGINE]` | facile | élevé | 🆕 |
| Profils de config (`cfg_profile`, hot-swap) | `[ENGINE]` | facile | élevé | 🆕 |
| Presets graphiques (batch de cvars) | `[ENGINE]` | très facile | moyen | 🆕 |
| Recherche / filtre / historique persistant console | `[ENGINE]` | facile | moyen | — |
| Alias & exec conditionnel | `[ENGINE]` | facile | moyen | — |
| Personnalisation du notify (lignes/durée/filtre) | `[ENGINE]` | facile | moyen | 🆕 |
| Auto-démo intelligent + pruning | `[ENGINE]` | facile | moyen | — |
| Métadonnées de démo + `demolist` | `[ENGINE]` | facile | élevé | 🆕 |
| Bookmarks de démo (playback) | `[ENGINE]` | moyenne | moyen | 🆕 |
| Server browser : favoris, historique, auto-reconnect | `[ENGINE]` | facile | élevé | — |
| Auto-join quand un slot se libère | `[ENGINE]` | facile | moyen | 🆕 |
| `net_stats` (ping/loss/jitter en console) | `[ENGINE]` | facile | moyen | 🆕 |
| Sandbox des downloads | `[ENGINE]` | facile | élevé (sécu) | — |
| Screenshot manager (PNG, burst, rangement par date) | `[RENDERER]` | facile | moyen | — |
| Normalisation / compresseur audio + volumes séparés | `[AUDIO]` | moyenne | moyen | — |
| Hot-swap du périphérique audio | `[AUDIO]` | moyenne | moyen | 🆕 |
| **Simulateur réseau unifié** (`net_simulate`) | `[DEV/CI]` | facile | élevé | 🆕 |
| **Crash report enrichi** (backtrace + cvars + console) | `[DEV/CI]` | facile | élevé | 🆕 |
| Profiler frame (timing breakdown) | `[DEV/CI]` | facile | élevé | — |
| Doc auto des cvars (Markdown) | `[DEV/CI]` | très facile | moyen | — |
| Watch de cvars + cvar groups étendus | `[DEV/CI]` | facile | moyen | — |
| Logger de trafic réseau (pcap) | `[DEV/CI]` | facile | élevé | — |

🆕 = nouvelle proposition de cette passe.

---

## 🎮 Joueur — Input & binds `[ENGINE]`

> Terrain 100 % moteur : le binding et la souris vivent dans `cl_keys.c` / `cl_input.c`,
> **aucune dépendance au cgame**. C'est le gisement le plus sous-exploité pour des features
> à fort impact joueur et faible coût.

### Convertisseur de sensibilité (`cm360`) — 🆕
Commande qui calcule `sensitivity` à partir d'une distance cm/360 (+ DPI), et conversion
depuis d'autres jeux (« je joue à X cm/360 sur CS, donne-moi l'équiv. UrT »). Pur calcul.
- **Accroche** : `sensitivity`, `m_pitch`/`m_yaw` (`cl_input.c:988`, `1008`) ; `cl_yawspeed`.
- **Cvars** : `m_dpi` (info) ; commandes `cm360 <cm>`, `sens_from <jeu> <valeur>`.
- **Compat** : ✅ purement local.

### Profils de binds (export / import / hot-swap) — 🆕
`bindprofile save <nom>` / `load <nom>` : sauvegarde un set complet de binds dans un fichier,
bascule à chaud (config compétitive vs fun, layouts par mode de jeu).
- **Accroche** : `Key_WriteBindings` (`keys.h:38`), `Key_GetBinding` (`keys.h:40`).
- **Compat** : ✅ local.

### Presets de visée à bascule — 🆕
Mémorise N réglages souris complets (`sensitivity`, `cl_mouseAccel*`, `m_pitch/m_yaw`) et
bascule en une touche (ex. sensi normale ↔ sensi sniper).
- **Accroche** : `cl_mouseAccel`/`cl_mouseAccelStyle`/`cl_mouseAccelOffset` (`cl_input.c:990-1001`).
- **Cvars** : `m_aimPreset`, commande `aimpreset <n>`.

### Bind avancé : tap / hold / double-tap — 🆕
Détecte un appui court vs maintenu, ou un double-tap, pour déclencher des commandes
différentes (ex. tap = +use, hold = autre action) — sans toucher au gameplay réseau.
- **Accroche** : traitement des events clavier dans `cl_keys.c` (timestamps de `Key_Event`).
- **Cvars** : `cl_tapTime`, `cl_doubleTapTime` ; commande `bindtap`/`bindhold`.

---

## 🎮 Joueur — Console, configs & commandes `[ENGINE]`

> Cœur de cible : `cl_console.c`, `cmd.c`, `cvar.c`, `files.c`.

### Profils de config (hot-swap) — 🆕
`cfg_profile save/load/list <nom>` : snapshot d'un sous-ensemble de cvars marquées, bascule
à chaud entre profils (LAN/online, comp/practice). Complète les presets graphiques.
- **Accroche** : itère les cvars (`Cvar_*`), réutilise l'I/O de `Cvar_WriteVariables`.
- **Cvars** : `cfg_profileDir`.

### Presets graphiques (batch de cvars) — 🆕
`gfxpreset low|balanced|comp|max` : applique un lot cohérent de cvars renderer/son en une
commande. Faisable 100 % moteur (simple batch via `Cbuf_AddText`).
- **Accroche** : `Cmd_AddCommand` + fichiers `presets/*.cfg`.

### Personnalisation du notify à l'écran — 🆕
Le notify (messages qui s'affichent hors console) est **dessiné par le moteur**, pas le cgame :
nombre de lignes, durée, position, filtre par tag/couleur, masquage du spam.
- **Accroche** : `Con_DrawNotify` (`cl_console.c:789`), `con_notifytime` (`cl_console.c:495`).
- **Cvars** : `con_notifyLines`, `con_notifyFilter`, `con_notifyX/Y`.

### Recherche / filtre dans la console
Recherche en direct dans le scrollback avec surlignage, filtrage par texte/couleur.
- **Accroche** : le texte vit dans `con->text[]` (couleur par char) — `cl_console.c`.
- **Cvars** : `con_searchHighlight`, `con_searchCaseSensitive`.

### Historique de console persistant
Sauvegarde de l'historique des commandes entre sessions.
- **Accroche** : `Con_SaveField` existe en mémoire — ajouter I/O fichier.
- **Cvars** : `con_historySave`, `con_historySize` ; fichier `console_history.txt`.

### Système d'alias
`alias name "cmd1; cmd2"` façon Source/GoldSrc, local (non envoyé au serveur).
- **Accroche** : wrapper autour de `Cmd_AddCommand` avec stockage de chaîne — `cmd.c`.

### Exec conditionnel
`exec_if <cvar> <op> <value> <command>` pour configs adaptatives (ex. par CPU/arch).
- **Accroche** : `Cvar_VariableValue` + `Cmd_ExecuteString` — `cmd.c`.

### Cvar lock (lecture seule locale)
Verrouille une cvar contre les modifs accidentelles (configs compétitives).
- **Accroche** : flag `CVAR_LOCKED` + check dans `Cvar_Set` — `cvar.c`.

### Hauteur & transparence de console
Hauteur configurable (hardcodée à 0.5) + opacité du fond.
- **Accroche** : `con->finalFrac = 0.5` → `con_height->value` dans `Con_RunConsole`.

### Autocomplétion enrichie (fuzzy + descriptions)
Fuzzy match + affichage des descriptions de cvars pendant la complétion.
- **Accroche** : `Field_CompleteCommand` (`common.c`) ; `Cvar_SetDescription` existe déjà.

### Smart condump (filtré & formaté)
Export console avec filtres (texte/couleur) et formats (texte/HTML coloré/Markdown).
- **Accroche** : étend `Con_Dump_f` — `cl_console.c`.

### Autres (console/config)
Timestamps & tags de source `[SERVER]`/`[CHAT]` · word-wrap par mots · `time <command>`
(profiling de commande) · `cfg_diff`/`cfg_merge` · snapshot/rollback de cvars · multi-paste
clipboard (`Sys_GetClipboardData` existe déjà, `cl_keys.c:176`).

---

## 🎬 Joueur — Démo, capture & média `[ENGINE]` / `[RENDERER]`

### Métadonnées de démo + `demolist` — 🆕
À l'enregistrement, écrit un sidecar `.json` (map, date, durée, serveur, joueurs présents).
Commande `demolist` qui liste/trie les démos par date/map en lisant ces sidecars.
- **Accroche** : `CL_StopRecord_f` / `clc.demorecording` (`cl_main.c:302`) ; `json.h` existe.
- **Compat** : ✅ purement local.

### Bookmarks de démo — 🆕
Pendant la lecture, poser des marqueurs (touche) ; sauter de marqueur en marqueur. Utile
pour le montage et la revue d'actions.
- **Accroche** : `clc.demoMessageSequence` suit la position (`cl_main.c`) ; sidecar `.bookmarks`.
- **Cvars** : commande `demomark` / `demojump`.

### Auto-démo intelligent + pruning
Auto-start/stop par round/match, conservation des N dernières, purge des démos vides.
- **Accroche** : `cl_autoRecordDemo` (on/off) existe déjà — ajouter triggers + pruning.
- **Cvars** : `cl_autoDemoKeep`, `cl_autoDemoMinSize`, `cl_autoDemoTrigger`.
- ⚠️ Triggers « round/match » fiables peuvent dépendre d'events du mod.

### Demo → vidéo / batch convert
`demo2avi` / `demo2png`, mode headless pour CI.
- **Accroche** : `cl_avi.c` supporte déjà le piping ffmpeg.
- **Cvars** : presets `cl_aviPreset` (youtube/twitch/archive).

### Screenshot manager
PNG en plus du JPEG/TGA, organisation par date/map, burst mode.
- **Accroche** : `R_ScreenShot_f` (`tr_init.c:1053`), `R_ScreenshotFilename` (`tr_init.c:963`).
- **Cvars** : `cl_screenshotFormat`, `cl_screenshotQuality`.
- ⚠️ Le mode « clean » (sans HUD) suppose `cg_draw2D` = cgame ; le rangement/format est moteur.

---

## 🌐 Joueur — Réseau, browser & perf `[ENGINE]`

### Server browser : favoris, historique, auto-reconnect
Favoris, tags, tri par historique, auto-reconnect avec backoff.
- **Accroche** : `cls.favoriteServers[]` (`cl_main.c:4773`), `cl_pinglist[]` (`cl_main.c:132`),
  `CL_ServerStatus` (`cl_main.c:148`).
- **Cvars** : `cl_autoReconnect`, `cl_autoReconnectDelay`. **Compat** : ✅ local.

### Auto-join quand un slot se libère — 🆕
Sur serveur plein, re-tente la connexion avec backoff jusqu'à ce qu'un slot s'ouvre
(via `getinfo`/`getstatus` déjà utilisés), puis connecte automatiquement.
- **Accroche** : `cl_pinglist[]` / `CL_ServerStatusResponse` (`cl_main.c:149`).
- **Cvars** : `cl_autoJoinRetry`, `cl_autoJoinTimeout`.

### `net_stats` — diagnostic réseau en console — 🆕
Affiche ping/perte/jitter/débit en console (le moteur a déjà toute la donnée du netchan).
Évite le lagomètre HUD (qui est cgame) tout en donnant le diagnostic au joueur.
- **Accroche** : compteurs `clc.netchan` / délais déjà calculés ; pas de protocole touché.
- **Cvars** : commande `net_stats`, optionnel `cl_netStatsLog`. **Compat** : ✅ passif.

### Sandbox des downloads
Validation (checksum, taille max, type) avant écriture disque ; quarantaine.
- **Accroche** : étend `CL_FirstDownload` (déjà présent pour filtrer les serveurs moddés).
- **Cvars** : `cl_downloadMaxSize`, `cl_downloadWhitelist`. **Compat** : ✅ local (sécu).

### FPS limiter adaptatif
`com_maxfps` existe déjà ; ajouter un cap adaptatif (sleep vs busy-wait) pour réduire GPU/CPU.
- **Accroche** : `com_maxfps`/`com_maxfpsUnfocused` (`common.c:76`).
- **Cvars** : `r_maxFPSMode` (sleep/busy).

### Profils de connexion — 🆕
Presets `rate`/`cl_packetdup`/`snaps` par qualité de lien (fibre / 4G / wifi instable),
bascule rapide.
- **Accroche** : `cl_packetdup` (`cl_input.c:982`), `rate`/`snaps` existants.
- **Cvars** : `cl_netProfile`.

### Protection contre configs malveillantes
Whitelist des commandes autorisées dans les servercommands reçus.
- **Accroche** : handler de servercommands client. **Cvars** : `cl_execWhitelist`.

---

## 🔊 Joueur — Audio `[AUDIO]` (mixer moteur)

### Normalisation / compresseur dynamique + volumes séparés
Compresse les sons forts, amplifie les faibles ; volumes par catégorie (armes/pas/voix/ambiance).
- **Accroche** : mixer moteur `snd_mix.c` / `snd_dma.c`.
- **Cvars** : `s_compressor`, `s_compressorThreshold`, `s_volumeWeapons`, `s_volumeFootsteps`…

### Hot-swap du périphérique audio — 🆕
Changer de sortie audio (casque ↔ HP) sans redémarrer le jeu via le backend SDL.
- **Accroche** : backend audio SDL (`code/sdl/`) ; réinit du device sans full restart.
- **Cvars** : commande `s_listdevices` / `s_setdevice`.

### TTS des messages chat
Lecture vocale du chat (accessibilité / specs AFK).
- **Accroche** : le moteur voit déjà les servercommands de chat → faisable moteur (espeak/flite).
- **Cvars** : `cg_tts`, `cg_ttsFilter`.

### Codecs (modulaire)
FLAC / WAV 24-bit via le système `snd_codec` modulaire (extension propre, périmètre maîtrisé).

---

## 🖥️ Renderer `[RENDERER]` (périmètre réduit)

> Volontairement limité : la plupart des gros effets (HDR, FSR, dynamic resolution, texture
> streaming…) ne se voient qu'en jeu via le rendu piloté par le cgame, et représentent des
> chantiers renderer disproportionnés pour ce fork. **Hors scope** pour l'instant.

### Cache de shaders SPIR-V sur disque
Évite la recompilation des pipelines au lancement (spécifique `renderervk`).
- **Accroche** : pipeline de création de shaders dans `renderervk`. **Cvars** : `r_shaderCache`.

### Live shader reloading (dev)
Recharge les shaders à chaud pendant le dev sans relancer le jeu.
- **Accroche** : chemin de chargement renderer ; watch fichier. **Tag réel** : `[DEV/CI]`.

---

## 🤖 Autonomie de l'agent & test runtime `[DEV/CI]`

> Objectif : permettre à un agent (ou la CI) de **fermer la boucle de feedback runtime** —
> aujourd'hui on peut compiler et lancer les tests unitaires (`tests/unit`, Unity) mais le
> comportement du moteur en exécution n'est pas vérifiable sans intervention humaine. La
> chaîne visée : `make → lancer headless → piloter par script → lire un résultat machine →
> assert via code de sortie`. Implémenté par tiers, du moins cher au plus cher.

### Tier 0 — oracle scriptable ✅ livré

- **`quit <code>`** → vrai code de sortie (`Sys_Quit(int)`). ⚠️ propre seulement en build
  release (`Sys_Exit` a `assert(code==0)` hors-NDEBUG).
- **Asserts auto-vérifiants** (impriment `PASS`/`FAIL`, un FAIL arme le code de sortie) :
  - `assert <a> <op> <b>` — ops num. `== != < <= > >=`, chaînes `eq`/`ne`.
  - `assert_cvar <name> <op> <value>` — compare la valeur d'une cvar.
  - `assert_cvar_flag <name> <FLAG>` — vérifie qu'une cvar porte un flag `CVAR_*`
    (ARCHIVE/USERINFO/SERVERINFO/SYSTEMINFO/INIT/LATCH/ROM/CHEAT/TEMP/…). Idéal pour vérifier
    la **justesse d'enregistrement** (ex. cvar d'identité bien `CVAR_USERINFO`).
  - `assert_command <name>` — vérifie qu'une commande console est enregistrée.
  - `assert_file <path>` — vérifie qu'un fichier existe dans le **chemin d'écriture**
    (`fs_homepath/fs_gamedir`). Pile pour les features qui écrivent : download, démo, screenshot.
- **`com_logTimestamps 0`** — log sans date (sortie « golden » diffable au byte près).

### Tier 0.5 — durcissement CI ✅ livré

- **Smoke sous ASan/UBSan** : job CI `integration-asan` rejoue la suite serveur compilée avec
  `-fsanitize=address,undefined` → **chaque test fonctionnel devient un test mémoire/UB** sur les
  chemins exercés (`ASAN_OPTIONS=detect_leaks=0`, la cible `release` propage désormais
  `EXTRA_LDFLAGS`).

### Idées de suite (harnais)

- **Scripting cfg conditionnel** : `exec_if <cvar> <op> <value> <cmd>` (ou `if`) — utile pour des
  cas adaptatifs (skip si une cvar absente, brancher selon l'arch). **Recommandation** : se limiter
  aux **conditions** (cheap, faible risque dans le cbuf) ; garder boucles/orchestration complexe
  dans `run.sh` (le shell a déjà de vraies boucles/conditions) plutôt que d'étendre le cbuf en
  langage de script (risque overflow/boucles infinies, maintenance). Voir aussi `vstr`.
- **`wait <frames>` / `waitms`** — débloque les asserts **asynchrones** (download/connexion terminés).
- **`assert_log <substr>`** — asserter qu'une chaîne est apparue dans la console (scan buffer).

### Tier 1 — headless + runner (le vrai déblocage)

**Run headless** ✅ livré — **serveur et client, tous deux install-free + en CI**
Wrapper `scripts/headless` (env SDL `dummy` + cvars `net_enabled 0`/`com_logTimestamps 0` +
homepath temporaire isolé) en deux modes, bootant sur le fixture minimal (zéro pk3) :
- **serveur** (défaut) : serveur dédié → tout le non-rendu (cvars, cmd, fs, parse, alias…).
  `make smoke`.
- **client** (`URT_CLIENT=1`) : binaire client + **null renderer** (`code/renderernull/`,
  `cl_renderer null`, aucune fenêtre/contexte GL) + son off + **VM UI sautée** (`cl_noUI`,
  les usages `uivm` étant déjà gardés) → **le client boote headless sans aucun asset**,
  testant les sous-systèmes client (cvars, binds, console, démo I/O). `make smoke-client`.
- **client + UI/cgame réels** (`URT_UI=1`) : charge les QVM du mod, donc nécessite une vraie
  install dans `URT_BASEPATH` (hors CI) — pour tester le code client proche du gameplay.

**Runner d'intégration `tests/integration/*.cfg` + `make smoke`** ✅ livré
Scripts `.cfg` auto-vérifiés via `assert` + code de sortie, bootés sur un fixture minimal
(`tests/integration/fixtures/q3ut4/default.cfg`, pas d'install requise). `make smoke` = build
serveur + suite ; câblé en CI (job `integration`). Oracle « ai-je cassé quelque chose ».

### Tier 2 — sortie lisible par machine

**Introspection JSON** 🆕 : `cvarlist --json`, `cmdlist --json`, `cvardump <name>` (`json.h`
existe). Permet d'asserter sur l'état exact du moteur au lieu de regexer du texte humain.
**Log structuré JSON** (`com_logJson`) : chaque ligne `{level, subsystem, msg}` au même sink.
(Le `cvar_dumpmd` du backlog sert double : doc + introspection.)

### Tier 3 — déterminisme (replay regression)

**RNG seedable + `fixedtime`** (déjà présent, `common.c:3894`) → replay de démo déterministe,
golden-frame regression couplé au runner d'intégration.

---

## 🔧 Développeurs & debug `[DEV/CI]`

### Simulateur réseau unifié (`net_simulate`) — 🆕
Unifie les bouts existants (`net_dropsim`, `cl_packetdelay`/`sv_packetdelay`) en **un seul
profil de dégradation** : perte %, latence + jitter, duplication, réordonnancement. Indispensable
pour tester l'interpolation/prédiction sans matériel réseau dégradé.
- **Accroche** : `net_dropsim` (`net_ip.c:1678`), `cl_packetdelay` (`common.c:3910`),
  file d'attente `NET_QueuePacket` (`net_chan.c:697`).
- **Cvars** : `net_simulate "loss=5 lag=80 jitter=20 dup=1 reorder=2"`.

### Crash report enrichi — 🆕
Étend le handler de signaux (déjà backtrace symbolique) pour écrire un `crashlog` avec :
build hash, OS/CPU, dump des cvars non-défaut, N dernières lignes de console, backtrace.
- **Accroche** : `signal_handler` + `backtrace`/`backtrace_symbols_fd` (`linux_signals.c:54-58`).
- **Cvars** : `com_crashLog` (chemin). **Compat** : ✅ local, énorme gain support.

### Profiler frame (timing breakdown)
Temps par section (renderer / VM / réseau / son / FS) par frame.
- **Accroche** : wrappers de timer autour des sections de `Com_Frame`.
- **Cvars** : `com_profiler` (off/console/CSV/overlay).

### Doc auto des cvars
`cvar_dumpmd` → Markdown de toutes les cvars (description, défaut, flags). Alimente `docs/CVARS.md`.
- **Accroche** : `Cvar_SetDescription` existe déjà.

### Watch de cvars
`cvar_watch <name>` : log à chaque changement (ancienne/nouvelle valeur).
- **Accroche** : callback/watchlist dans `Cvar_Set`.

### Cvar groups étendus
Étendre `cvarGroup_t` (déjà `CVG_RENDERER`) avec `CVG_GAMEPLAY`/`CVG_NETWORK`/`CVG_AUDIO`…
- **Accroche** : `cvarGroup_t` (`q_shared.h:973`).

### Logger de trafic réseau
Dump du trafic (timestamps), format pcap-compatible pour Wireshark.
- **Accroche** : wrapper dans `NET_*` / netchan. **Cvars** : `cl_netLog`, `sv_netLog`. **Compat** : ✅ passif.

### Live config/cvar watch (inotify) — 🆕
Surveille un `.cfg` et le re-exec à chaque sauvegarde — itération rapide sur les configs/binds
pendant le dev, sans `exec` manuel.
- **Accroche** : `FS_*` + watch fichier (inotify Linux) ; `Cbuf_AddText`.
- **Cvars** : `dev_watchCfg`.

### Tracking mémoire
`mem_stats`/`mem_dump` : breakdown hunk/zone par tag, détection de fuites au shutdown.
- **Accroche** : étend `Hunk_Log`/`Z_LogHeap`.

### Inspecteurs protocole
Configstring browser (`cs_dump`/`cs_watch`, `SV_GetConfigstring` existe) · snapshot delta
visualizer (`snap_diff`/`snap_watch`) · VM bytecode disassembler · engine/trap API doc
generator (tables de dispatch `sv_game.c`/`cl_cgame.c`).

### CI / qualité
Benchmark de régression de perf (`timedemo` existe) · replay déterministe depuis démo pour
tests de non-régression · fuzzer protocole étendu (`tests/fuzz/`) · frame capture.

---

## 🏗️ Chantiers lourds (parkés — fort intérêt, coût élevé)

> Gardés pour mémoire : faisables côté moteur mais **difficiles**. À n'attaquer qu'une fois
> les quick-wins joueur/dev livrés.

- **VoIP Opus** `[ENGINE]` — opcodes `svc_voipOpus`/`clc_voipOpus` **déjà réservés**
  (`qcommon.h:344-362`, garde `USE_VOIP`) ; libopus + capture micro SDL + transport sur le
  channel existant. Compat ✅ (legacy ignorent les packets VoIP). **Difficulté : difficile.**
- **Replay buffer (shadow recording)** `[ENGINE]` — bufferise les N dernières secondes en
  mémoire (messages réseau), dump à la demande (style ShadowPlay). **Difficile** (gestion
  mémoire du flux). Cvars `cl_replayBuffer`, commande `cl_replaySave`.
- **HRTF binaural** `[AUDIO]` — spatialisation 3D casque ; nécessite un backend audio avancé.

> ❌ **Retirés (hors périmètre / risque compat)** : compression LZ4/zstd des snapshots
> (négociation de protocole requise → casse les legacy) · HDR/tone-mapping, FSR upscaling,
> dynamic resolution, texture streaming/LOD (chantiers renderer disproportionnés, surtout
> visibles via cgame) · capture AVI « avec HUD » (le HUD est rendu par le cgame).

---

## Annexe S — Serveur (en pause)

> Travail serveur **mis de côté** le temps de se concentrer sur joueur + dev. Conservé intact
> pour reprise ultérieure. Tout est « gratuit » côté compat (local serveur / servercommands).

| Feature | Difficulté | Impact |
|---------|------------|--------|
| Audit log RCON (`sv_rconLog`) | très facile | élevé (sécu) |
| Anti-bruteforce RCON (`rateLimit_t`/`leakyBucket_t`, `server.h:130`) | facile | élevé (sécu) |
| Status / playerlist JSON (`json.h`, `SV_Status_f`) | facile | élevé |
| Bans temporaires + ban par GUID (`serverBans[]`) | moyenne | élevé |
| Gestion AFK automatique (`lastUsercmd` dans `SV_ClientThink`) | facile | moyen |
| Map vote / RTV natif (servercommands texte) | moyenne | élevé |
| Discord webhook (libcurl vendored) | moyenne | élevé |
| Géofencing par TLD (`sv_filter.c`, `client->tld`) | facile | moyen |
| Cron / commandes programmées (`SV_Frame` + `Cbuf_AddText`) | facile | moyen |
| Spectator delay anti-ghosting (`snapFrames[]`) | moyenne | moyen |
| Validation stricte des userinfo | facile | moyen |

Divers serveur (à étoffer) : limite de ping dynamique · mute/silence par GUID · whitelist de
checksums de mods · ready-up compétitif · rotation des logs · export stats match (JSON/CSV) ·
metrics Prometheus. **Borderline (coop mod requise)** : auto-équilibrage des équipes (userinfo `team`).

---

## Annexe A — idées `[MOD]`-dépendantes (hors périmètre moteur)

> Ces idées vivent dans le **cgame/ui de q3ut4**, que ce projet ne ship pas. Non implémentables
> côté moteur sans coopération du mod (ou un cgame de remplacement, qui casserait la compat).

**HUD & visée** : indicateurs de dégâts directionnels · scoreboard enrichi (accuracy, streaks) ·
crosshair dynamique + hitmarker · compteur de munitions flottant · killstreak tracker ·
minimap/radar · FOV par arme · weapon wheel radial · indicateur de spawn protection ·
import/export de crosshairs.

**Caméra & spectateur** : free-cam amélioré · deathcam/killer-cam · spectator info overlay ·
watermark/stream overlay · smooth camera paths pour démos.

**Accessibilité (HUD)** : mode daltonien · sous-titres des sons · échelles de UI ·
high-contrast · indicateurs d'équipe par formes.

**Social/gameplay HUD** : clans/tags dans le scoreboard · toasts de notification · training mode ·
strafe-jump helper · hit sounds différenciés par zone.

**Borderline (coopération mod requise)** : pause/timeout (gel `level.time`) · ready-up
obligatoire · POV auto en tournoi · custom skins client-side.

---

## Annexe B — références de compatibilité

| Élément | Modifiable ? | Note |
|---------|:-----------:|------|
| `entityState_t` / `playerState_t` | ❌ | Casserait clients/serveurs |
| Opcodes réseau (`svc_*`, `clc_*`) | ❌ | Casserait le protocole (sauf opcodes déjà réservés, ex. VoIP) |
| `msg_t` / `netchan_t` | ❌ | Casserait le transport |
| Indices de configstrings utilisés | ❌ | Casserait le mod |
| **Userinfo keys** | ✅ | Clés inconnues = ignorées par les legacy |
| **ServerCommands** (texte) | ✅ | Commandes inconnues = ignorées |
| **Configstrings non-utilisées** | ✅ | Zones disponibles |
| **Cvars / commandes console** | ✅ | Inconnues = ignorées |

### Zones de code par type de feature

| Type | Fichiers |
|------|----------|
| Console | `code/client/cl_console.c` |
| Commandes / cvars / fichiers | `code/qcommon/cmd.c`, `cvar.c`, `files.c`, `common.c` |
| Input & binds | `code/client/cl_input.c`, `cl_keys.c`, `code/sdl/sdl_input.c` |
| Démo / capture | `code/client/cl_main.c`, `cl_parse.c`, `cl_avi.c` |
| Audio | `code/client/snd_dma.c`, `snd_mix.c`, `code/sdl/` |
| Réseau client / browser | `code/client/cl_main.c`, `code/qcommon/net_chan.c`, `net_ip.c` |
| Crash / signaux | `code/unix/linux_signals.c` |
| Renderer | `code/renderervk/`, `code/renderer/`, `code/renderercommon/` |
| Serveur (en pause) | `code/server/sv_ccmds.c`, `sv_client.c`, `sv_filter.c`, `sv_main.c` |

---

*Document vivant. Périmètre validé contre le code (juin 2026). Focus joueur + dev ; serveur en
pause (Annexe S). Avant d'attaquer une idée, revérifier l'accroche code (le moteur évolue) et
privilégier les chemins « gratuits » (Annexe B).*
