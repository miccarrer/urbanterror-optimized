# Analyse Définitive : urbanterror-slim vs urbanterror-optimized

**Date d'analyse** : 2026-06-14 · **Total commits omg-urt analysés** : 66

> ⚠️ **Document historique (analyse initiale).** L'état « Notre code : Absent / Manquant »
> ci-dessous reflète le **2026-06-14** et n'est plus à jour. La **source de vérité courante**
> est [`ROADMAP.md`](../../ROADMAP.md) (M5) et [`.context/progress.md`](../../.context/progress.md).
>
> **Réconciliation au 2026-06-15** — depuis cette analyse :
> - ✅ **Faits** : Master server + `modversion`, sécurité downloads, window margins (M0) ;
>   cvars serveur `sv_sayprefix`/`sv_tellprefix`/`sv_nofallDamage`/`sv_infiniteStamina`, et
>   **Tellme / messagemode5** (M5).
> - ✅ **Resolu sans action** : *x86 JIT NaN fix* (`56f03bca`) déjà présent via l'upstream
>   Quake3e ; *referee fix* (`b834398f`) **absorbé** par la réimplémentation de Tellme
>   (gating `chat_cmdMode` dans `cl_keys.c`).
> - ⏸️ **Différés** (réimpl. fraîche + tests en jeu requis) : **console à onglets** (`2c70fdc0`),
>   **demo UrT** (`9579fc7e`). **dmaHD** (`213e0e5d`) : optionnel, décision en attente.

---

## 📊 Résumé Exécutif

| Catégorie | Déjà présent | Manquant | Partiel | Total |
|-----------|:---:|:---:|:---:|:---:|
| Fonctionnalités UrT | 13 | 11 | 5 | 29 |
| Branding/Build | 0 | 8 | 0 | 8 |
| Bug fixes | 10 | 1 | 5 | 16 |
| Autres (CI, readme) | 0 | 13 | 0 | 13 |

---

## 🔴 FONCTIONNALITÉS MANQUANTES (Recommandées pour intégration)

### 1. 🖥️ Console à onglets multiples (`2c70fdc0`, `03b932d1`)
- **Description** : Console avec plusieurs onglets (chat, game, debug) au lieu d'une seule console
- **Notre code** : `console_t con;` (console unique seulement)
- **Slim** : Système `NUM_CONSOLES` avec onglets et couleurs différenciées
- **Valeur** : ⭐⭐⭐ Haute - Fonctionnalité UrT très appréciée
- **Fichiers** : `code/client/cl_console.c`

### 2. 💬 Tellme (`d4f12aa7`, `7716e57a`, `002d179d`, `588c4cb1`)
- **Description** : Bouton "tell me" + commande tellme pour envoyer des messages d'équipe depuis la console, et permettre de taper des commandes console dans le chat
- **Notre code** : Absent
- **Valeur** : ⭐⭐⭐ Haute - Fonctionnalité UrT utilisée quotidiennement
- **Fichiers** : `code/client/cl_console.c`, `code/client/cl_keys.c`

### 3. 🎬 UrT Demo Support (`9579fc7e`, `ec15c2c6`)
- **Description** : Support du format démo UrT (`USE_URTDEMO`), 218+ lignes de code
- **Notre code** : Absent (`USE_URTDEMO` non défini)
- **Valeur** : ⭐⭐⭐ Haute - Essentiel pour rejouer les démos UrT 4.x
- **Fichiers** : `code/client/cl_main.c`, `code/client/client.h`, `code/qcommon/common.c`, `code/qcommon/q_shared.h`, `code/qcommon/qcommon.h`, `Makefile`

### 4. 🌐 Master Server UrT + modversion (`4bd6632f`)
- **Description** : Configuration des master servers UrT + ajout de `modversion` pour apparaître dans le browser de jeu UrT
- **Notre code** : `MASTER_SERVER_NAME` présent mais PAS de `modversion`/`sv_modversion`
- **Valeur** : ⭐⭐⭐ Haute - Permet d'être visible dans le server browser UrT
- **Fichiers** : `code/qcommon/common.c`, `code/qcommon/qcommon.h`, `code/server/sv_main.c`

### 5. 🔊 dmaHD Sound Engine (`213e0e5d`, `7df06294`, `bb82d0e5`)
- **Description** : Port du moteur audio dmaHD (haute qualité) depuis ioq3-for-UrT, 1524+ lignes
- **Notre code** : Pas de `snd_dmahd.c`
- **Valeur** : ⭐⭐ Moyenne - Amélioration qualité sonore, mais optionnel
- **Fichiers** : `code/client/snd_dmahd.c` (nouveau), `code/client/snd_dma.c`, `code/client/snd_dmahd.h`, `code/client/snd_local.h`, `code/client/snd_mem.c`, `code/client/snd_mix.c`

### 6. ⚙️ sv_sayprefix / sv_tellprefix (`dd52e95f`)
- **Description** : Cvars personnalisables pour les préfixes say/tell
- **Notre code** : Absent
- **Valeur** : ⭐⭐ Moyenne - Utile pour serveurs moddés
- **Fichiers** : `code/server/server.h`, `code/server/sv_ccmds.c`, `code/server/sv_init.c`, `code/server/sv_main.c`

### 7. ⚙️ sv_nofalldamage (`be301ebf`)
- **Description** : Cvar serveur pour désactiver les dégâts de chute ("no lemmings")
- **Notre code** : Absent
- **Valeur** : ⭐⭐ Moyenne - Fun pour serveurs d'entraînement
- **Fichiers** : `code/server/server.h`, `code/server/sv_game.c`, `code/server/sv_init.c`, `code/server/sv_main.c`

### 8. ⚙️ sv_infiniteStamina (`bbb587d4`)
- **Description** : Cvar serveur pour stamina infinie
- **Notre code** : Absent
- **Valeur** : ⭐⭐ Moyenne - Fun pour serveurs d'entraînement
- **Fichiers** : `code/server/server.h`, `code/server/sv_client.c`, `code/server/sv_init.c`, `code/server/sv_main.c`

### 9. 📥 Sécurité téléchargements (`0504f377`, `32499141`)
- **Description** : Fix liens malformés + ne jamais télécharger les paks UrT + fix downloads sur serveurs moddés (extraire uniquement le nom de map)
- **Notre code** : `FS_ComparePaks` présent mais pas le filtre "urbanterror paks"
- **Valeur** : ⭐⭐⭐ Haute - Sécurité + fiabilité download
- **Fichiers** : `code/client/cl_main.c`, `code/qcommon/files.c`

### 9bis. 📁 Dossier de download `q3ut4/download` (`f92d24af`, `99f6edbb`) — ✅ INTÉGRÉ (2026-06-16)
- **Description** : range les paks téléchargés dans `q3ut4/download/` au lieu de la racine du game dir
- **Notre code** : ✅ réimplémenté proprement sur `feature/fs-downloadpath` (corrige fuite
  `CopyString`, taille de buffer, incohérence write/read de slim) + ajout cvar `fs_downloadpath`
  (racine configurable/partageable). UDP + cURL.
- **Valeur** : ⭐⭐ Moyenne - hygiène fichiers + cache partagé multi-install
- **Fichiers** : `code/qcommon/files.c`, `code/client/cl_main.c`, `code/client/cl_parse.c`, `code/client/cl_curl.c`

### 10. 🔧 Fix referee commands (`b834398f`)
- **Description** : Fix pour commandes referee
- **Notre code** : Absent
- **Valeur** : ⭐⭐ Moyenne
- **Fichiers** : `code/client/cl_keys.c`

### 11. 🐛 x86 JIT NaN fix (`56f03bca`)
- **Description** : Fix comparaison JIT pour gérer les opérandes NaN
- **Notre code** : Absent
- **Valeur** : ⭐⭐ Moyenne - Fix subtil, affecte la VM
- **Fichiers** : `code/qcommon/vm_x86.c`

---

## 🟡 FONCTIONNALITÉS PARTIELLEMENT PRÉSENTES

| Commit | Description | Ce qui manque |
|--------|-------------|---------------|
| `642545fa` | Port master UrT (27900) | Vérifier notre valeur actuelle |
| `e230227f` | Fix browser list (modversion) | Pas de modversion |
| `05574288` | cl_allowdownload en CVAR_ROM | Vérifier flag actuel |
| `3cd007a4` | USE_URTDEMO + USE_Q3KEY | USE_URTDEMO manquant |
| `4547c7e2` | r_texturemode par défaut | Vérifier valeur actuelle |

---

## 🟢 DÉJÀ PRÉSENT DANS NOTRE PROJET (13 fonctionnalités)

1. ✅ MAX_RELIABLE_COMMANDS = 128 (`f4938846`)
2. ✅ qkey fix (`39a9574c`)
3. ✅ MAX_LOCATIONS (`1369dc9d`)
4. ✅ condump actif (`63aaace0`)
5. ✅ con_autoclear (`45df16ca`)
6. ✅ r_ext_compressed_textures (`fd3cac59`)
7. ✅ .mtrx shader loading (`7f6cf9d4`)
8. ✅ CalcSpecular epsilon fix (`27aefe17`)
9. ✅ r_parallaxMapOffset (`6c26eff4`)
10. ✅ teleport command (`205679d1`)
11. ✅ spoof/forcecvar/sendclientcommand (`20ade64f`)
12. ✅ kicknum/kickall/kickbots (`e0b6c526`)
13. ✅ Authorize server disabled (`7a1eaa22`)

---

## ⚪ NON APPLICABLE / BRANDING (21 commits)

Ces commits concernent le branding (nom binaire, icône, README, workflows CI, fichiers VS project). Ils ne sont pas pertinents pour l'optimisation du moteur.

---

## 📋 RECOMMANDATION D'INTÉGRATION

### Priorité 1 - Intégrer (valeur immédiate)
1. **Master Server + modversion** (`4bd6632f`) - Visibilité server browser
2. **Sécurité downloads** (`0504f377`, `32499141`) - Sécurité + fiabilité
3. **x86 JIT NaN fix** (`56f03bca`) - Correction de bug VM

### Priorité 2 - Intégrer (fonctionnalités UrT importantes)
4. **Console à onglets** (`2c70fdc0`, `03b932d1`) - UI importante
5. **Tellme** (`d4f12aa7`, `7716e57a`, `002d179d`, `588c4cb1`) - Feature UrT quotidienne
6. **UrT Demo support** (`9579fc7e`, `ec15c2c6`) - Essential pour démos UrT

### Priorité 3 - En option
7. **dmaHD** (`213e0e5d`) - Grosse masse de code (1524 lignes)
8. **sv_sayprefix/tellprefix** (`dd52e95f`)
9. **sv_nofalldamage** (`be301ebf`)
10. **sv_infiniteStamina** (`bbb587d4`)
11. **referee fix** (`b834398f`)