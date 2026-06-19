# Étude de faisabilité : moteur vs game VM pour les menus / HUD / `ui.qvm` custom

**Date** : 2026-06-19 · **Contexte** : la feature *Thèmes d'UI* (Feature #7) permet de re-styler le
chrome console et de **remapper des assets 2D** (menus/HUD) via `remapShader`. Question posée :
*peut-on aller plus loin et redéfinir le menu principal (groupe de boutons, disposition), voire
faire gérer les menus/HUD/scoreboard par le moteur ?*

## TL;DR

- **Menus / HUD / scoreboard sont dessinés par les VMs de jeu** (`ui.qvm`, `cgame.qvm`), pas par le
  moteur. Le moteur n'est qu'un **service de rendu**.
- **HUD / scoreboard** : reprise moteur **infaisable en pratique** — ce sont des *vues du gamestate*
  que seul le mod connaît (logique UrT fermée). Reprendre = forker le jeu.
- **Menus** : techniquement faisable via une **`ui.qvm` custom**, mais **non recommandé** —
  on perdrait tous les menus UrT et ça sort de la charte du projet.
- **Plafond réaliste retenu** : remap d'assets 2D (fonds + widgets rectangulaires).

## Comment c'est câblé (vérifié dans le code)

- Le moteur **appelle** les VMs : `VM_Call(cgvm, CG_DRAW_ACTIVE_FRAME)` (`cl_cgame.c:935`),
  `VM_Call(uivm, UI_REFRESH)` (`cl_scrn.c:562`). Les VMs **rappellent** le moteur pour dessiner via
  syscalls : `CG_R_DRAWSTRETCHPIC`/`CG_R_SETCOLOR`/`CG_R_REMAP_SHADER` (`cl_cgame.c:635-752`),
  `UI_R_*` (`cl_ui.c`).
- Le repo ne contient que les **en-têtes d'interface** : `code/ui/ui_public.h`,
  `code/cgame/cg_public.h`, `code/game/{g,bg}_public.h`. **Aucune implémentation** de menu/HUD —
  elle vit dans les `ui.qvm`/`cgame.qvm` **UrT (fermés, en pk3)**.

## Pourquoi le HUD / scoreboard est hors de portée

Ce ne sont pas « de la présentation » : ce sont des vues d'un **état de jeu** (vie, munitions,
équipe, scores, type de partie) que **seul le mod modélise**. Le moteur ne parse pas la sémantique
gamestate/entités d'UrT. Le reprendre impliquerait de réimplémenter une partie du gameplay UrT
(code fermé) → rétro-ingénierie + rupture de compat serveur + maintenance perpétuelle. **Infaisable
en pratique.**

## Le cas « menus » : `ui.qvm` custom

Techniquement possible mais il manque **deux** briques sur trois :

1. **Code de menu** — absent (en-têtes seuls). Il faudrait importer un module UI complet :
   ioquake3 **q3_ui** (mono-bloc, GPL) ou l'**UI Team Arena** (data-driven `.menu`, GPL). ~15–30k LOC.
2. **Chaîne d'outils QVM** — **absente** du repo (pas de `q3lcc`/`q3asm` ; les cibles `qvm` du
   Makefile concernent l'interpréteur VM du moteur, pas la compilation de game-VMs). Alternative :
   compiler en lib native (`uix86_64.so`), pas toujours autorisée côté client.
3. **Glue UrT** — l'interface (`ui_public.h`) est présente et stable → un module q3_ui *se
   chargerait*, mais afficherait un **menu Quake 3**, pas celui d'UrT.

### Coûts rédhibitoires

- **Perte de tous les menus UrT** (browser serveur + filtres, réglages UrT…). Une VM = **tout ou
  rien** : impossible de n'override que le style.
- Pour ne pas régresser : **réimplémenter les menus UrT** sur q3_ui → énorme + maintenance continue
  (UrT met à jour sa `ui.qvm`, la nôtre diverge).
- **Conflit de charte** : `urbanterror-optimized` est un **moteur** ; contrainte = `code/` aligné sur
  `ec-/Quake3e`, changements **non-code seulement** (CLAUDE.md). Livrer un module UI = autre produit.

### Effort vs valeur

Effort **très élevé** (import + toolchain + restyle + réimplémentation features UrT + maintenance) ;
valeur = menu thématisable **au prix des menus UrT**. **Négatif net** pour jouer réellement.

## Raison de fond (data-driven vs compilé)

L'UI **Team Arena** est *data-driven* : skin/layout dans des fichiers `.menu`, override par pk3
**sans recompiler**. Si UrT utilisait cette base, on re-skinnerait le menu par fichiers. UrT utilise
la base **q3_ui compilée** → re-skin par fichiers impossible, et **on ne peut pas changer ça** depuis
le moteur. C'est précisément ce qui bloque le « vrai » re-skin de menu.

## Recommandation

- **Ne pas** remplacer la `ui.qvm` dans ce projet.
- Customisation menu = **remap d'assets** (Feature #7 Phase 2/3) : fonds (`ui/assets/ut_menuback`,
  `backscreen`) + widgets **rectangulaires** (onglets, barres). Les **boutons** (texte placé par la
  VM) et les **polices VM** (`fonts/fontImage_*`, `R_REGISTERFONT`) restent hors de portée.
- Si l'envie d'un vrai menu custom persiste : **repo séparé « UI mod »** basé sur l'UI Team Arena
  data-driven — projet à part, et qui ne porterait toujours pas les menus UrT.

## Limites de la voie retenue (remap d'assets)

- Marche pour les **images rectangulaires** ; les **icônes en forme** (flèches, curseur) deviennent
  des aplats si remappées vers une couleur unie → leur faut de vrais assets dessinés.
- L'allowlist `remapShader` (`ui/`/`menu/`/`hud/`/`gfx/2d/`) couvre les assets de menu vus dans
  `shaderlist` (ex. `ui/assets/ut_menuback`) mais **pas** `fonts/` ni `gfx/crosshairs/`.
