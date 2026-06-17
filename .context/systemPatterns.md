# System Patterns — Urban Terror Optimized

## Architecture du code

Le projet suit l'architecture classique Quake III Arena / ioquake3 :

```
code/
├── qcommon/        # Code commun client/serveur (VM, network, files, cvars, commands)
├── client/         # Code client (console, input, sound, parse, UI, download)
├── server/         # Code serveur (game, network, commands, snapshots)
├── renderer/       # Renderer OpenGL 1.x optimisé (modulaire, chargeable via dlopen)
├── renderer2/      # Renderer OpenGL2/3.x (ioq3 original, non maintenu)
├── renderervk/     # Renderer Vulkan (moderne, recommandé)
├── renderercommon/ # Code commun aux renderers
├── sdl/            # Backend SDL2 (video, audio, input)
├── botlib/         # Bibliothèque de bots (AAS, AI)
├── game/           # Interfaces partagées (g_public.h, bg_public.h)
├── cgame/          # Interface cgame (cg_public.h)
├── ui/             # Interface UI
├── unix/           # Code spécifique Unix/Linux
├── win32/          # Code spécifique Windows (+ solution MSVC)
├── asm/            # Assembleur optimisé (mixing audio, x86/x64)
├── libcurl/        # libcurl embarquée (downloads)
├── libjpeg/        # libjpeg embarquée
├── libogg/         # libogg embarquée
├── libvorbis/      # libvorbis embarquée
└── libsdl/         # SDL2 embarquée (Windows)
```

## Patterns clés

### Virtual Machines (VM)
Le jeu charge la logique via des VMs compilées (QVM bytecode ou DLL native).
- `code/qcommon/vm.c` — Gestionnaire VM principal
- `code/qcommon/vm_x86.c` — Compilateur JIT x86
- `code/qcommon/vm_aarch64.c` — Compilateur JIT ARM64
- `code/qcommon/vm_powerpc.c` — Compilateur JIT PowerPC

### Renderers modulaires
Les renderers peuvent être :
- **Statiques** (`USE_RENDERER_DLOPEN=0`) : liés directement dans le binaire
- **Dynamiques** (`USE_RENDERER_DLOPEN=1`, défaut) : chargés via `dlopen`, switchables à chaud via `\cl_renderer`

### Cvars & Commands
- Cvars : Variables de configuration (client/serveur)
- Commands : Fonctions appelables depuis la console
- Le système est dans `code/qcommon/cvar.c` et `code/qcommon/cmd.c`

## Test & vérification headless (session 9)

Le moteur se pilote **headless et install-free** pour vérifier le comportement runtime — point
d'entrée : `make smoke` (serveur), `make smoke-client` (client via null renderer + `cl_noUI`),
ou `scripts/headless` ad hoc. Asserts auto-vérifiants (`assert`, `assert_cvar`, `assert_cvar_flag`,
`assert_command`, `assert_file`) → code de sortie. Null renderer `code/renderernull/`. Suites en CI
(`integration`, `integration-asan`). Détails : `tests/integration/README.md`, `docs/FEATURE_IDEAS.md`
§ Autonomie de l'agent.

## Convention de travail — propositions proactives d'outillage

L'agent est **encouragé à proposer proactivement** de nouvelles solutions améliorant son autonomie,
le harnais de test, les workflows CI et l'outillage dev — pas seulement à exécuter la tâche demandée.
À surfacer aux moments naturels (après une livraison, face à une friction, devant un manque
tests/CI). Proposer concrètement (accroches + effort estimé), recommander une priorité, laisser le
choix à l'utilisateur ; ne pas spammer ni dérailler la tâche en cours, et respecter une dépriorisation.

## Conventions de codage

### Style C (Quake3/ioq3)
- **Indentation** : Tabs
- **Braces** : Opening brace on same line as control statement; `}` aligned with control statement
- **Naming** : `PascalCase` pour types/structs, `camelCase` ou `snake_case` pour variables
- **Header guards** : `#ifndef` classique (pas de `#pragma once`)
- **Commentaires** : `//` pour inline, `/* */` pour blocs

### Exemple de style
```c
void SomeFunction( int arg ) {
	int localVar;

	if ( arg > 0 ) {
		localVar = arg * 2;
	} else {
		localVar = 0;
	}

	return localVar;
}
```

## Build system

### Makefile (principal)
- Options clés : `CNAME`, `DNAME`, `ARCH`, `USE_SDL`, `USE_VULKAN`, `USE_OPENGL`, `RENDERER_DEFAULT`, `BUILD_SERVER`, `BUILD_CLIENT`, `USE_RENDERER_DLOPEN`
- Cible : `make install DESTDIR=<path>` ou `make` (sortie dans `build/`)
- Le Makefile supporte Linux, macOS, MinGW (Windows), Raspberry Pi, PowerPC

### CMake — SUPPRIMÉ (M1)
- `CMakeLists.txt` et `cmake_modules/` ont été retirés. Build = Makefile + MSVC.
- IDE/clangd via `compile_commands.json` (`bear -- make`). Récupérable dans l'historique git si besoin.

### MSVC (Windows)
- Solution : `code/win32/msvc2017/quake3e.sln`
- Projects : `quake3e.vcxproj`, `quake3e-ded.vcxproj`, `renderer.vcxproj`, `renderervk.vcxproj`

## Cvars ajoutées (M0 / ex-Phase 1A)

### Window margins (css-like windowed mode positioning)
- `r_windowMarginTop` / `r_windowMarginBottom` / `r_windowMarginLeft` / `r_windowMarginRight`
- Valeur `-1` = auto (centré), `>=0` = marge fixe en pixels
- Déclarées dans `code/client/cl_main.c`, `code/client/client.h`
- Logique dans `code/sdl/sdl_glimp.c`

### Modversion (master server)
- `modversion` / `sv_modversion` — identifiant du moteur pour le master server UrT
- Ajouté dans `code/qcommon/common.c`, `code/qcommon/qcommon.h`, `code/server/sv_main.c`

## CI/CD
- `.github/workflows/build.yml` : Workflow GitHub Actions hérité de Quake3e
- Builds : Windows (MSYS2, MSVC), Ubuntu (x86, x86_64, ARM64), macOS (x86_64, ARM64)
- Artifacts uploadés + release auto sur tag `latest`
- **Phase 5** : À refactorer en `ci.yml` + `release.yml`, renommer artifacts vers `urbanterror-optimized-*`