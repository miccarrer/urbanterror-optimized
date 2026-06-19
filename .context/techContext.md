# Tech Context — Urban Terror Optimized

## Stack technique

### Langage principal
- **C** (C89/C99 selon les fichiers, style Quake3/ioq3)

### Dépendances externes
| Dépendance | Rôle | Embarquée ? |
|------------|------|:---:|
| SDL2 | Video, audio, input (optionnel) | Non (système) sauf Windows |
| OpenGL | Renderer classique | Non (système) |
| Vulkan | Renderer moderne | Non (système) |
| libcurl | Téléchargements HTTP | Oui (`code/libcurl/`) |
| libjpeg | Compression JPEG (screenshots) | Oui (`code/libjpeg/`) |
| libogg + libvorbis | Décodeur audio Ogg Vorbis | Oui (`code/libogg/`, `code/libvorbis/`) |

### Build systems
- **Makefile** (canonique) — Support Linux, macOS, MinGW, Raspberry Pi, PowerPC
- **MSVC** (`code/win32/msvc2017/`) — Windows natif
- CMake : **supprimé en M1** (build = Makefile + MSVC ; IDE via `compile_commands.json`/bear)

### CI/CD
- **GitHub Actions** (`.github/workflows/build.yml`)
- Plateformes : Windows (MSYS2, MSVC), Ubuntu (x86/x86_64/ARM64), macOS (x86_64/ARM64)

## Environnement de développement

### Outils requis (Linux)
```bash
sudo apt install make gcc libcurl4-openssl-dev mesa-common-dev \
  libxxf86dga-dev libxrandr-dev libxxf86vm-dev libasound-dev libsdl2-dev
```

### Build minimal (Linux)
```bash
make                        # Build dans build/
make install DESTDIR=bin    # Sortie dans bin/
```

### Build avec Vulkan statique
```bash
make BUILD_SERVER=0 USE_RENDERER_DLOPEN=0 RENDERER_DEFAULT=vulkan
```

### Test rapide du build
```bash
# Syntaxe bash (ne pas utiliser directement dans fish — préfixer avec bash -c '...')
make clean && make -j$(nproc) 2>&1 | tail -5
```

## Lancement local du client (machine de dev)

Le jeu n'est **pas** installé : le client custom tourne depuis `build/` mais réutilise les
**assets de l'install officielle**. Deux lanceurs dans `~/.local/bin/` :

- **`urbanterror-optimized`** → notre client (`build/release-linux-x86_64/urbanterror-optimized.x64`).
- **`urbanterror-official`** → le client UrT 4.3 d'origine (référence/comparaison).

Chemins (FS) du lanceur `urbanterror-optimized` :

| Rôle | Chemin | Usage |
|------|--------|-------|
| `fs_basepath` | `/home/michael/Jeux/UrbanTerror43` | assets officiels (pk3, maps) — **lecture seule** |
| `fs_homepath` | `~/.local/share/urbanterror-optimized` | **écriture** : config, demos, screenshots, identities, themes |
| `fs_downloadpath` | `~/.local/share/urbanterror-shared` | paks téléchargés (partagés ; `cfg`/`qkey` y sont symlinkés) |
| `fs_game` | `q3ut4` | mod dir (tout est sous `<path>/q3ut4/`) |

**Fichiers/dossiers importants** (sous `<fs_homepath>/q3ut4/`) :
- `q3config.cfg` — config persistée (cvars `archive`, alias) ; `autoexec.cfg` — exécuté au boot.
- `identities/<nom>.cfg` — profils d'identité (`loadidentity`/`saveidentity`).
- **`themes/<nom>.cfg`** — thèmes d'UI (`theme`/`themesave`/`themelist`). C'est ici que `themesave`
  écrit et que `theme` lit. **Les thèmes d'exemple du repo (`docs/themes/`) doivent être copiés ici**
  pour être visibles en jeu :
  ```bash
  cp docs/themes/*.cfg ~/.local/share/urbanterror-optimized/q3ut4/themes/
  ```
- `q3history` — historique console persistant ; `servercache.dat` — cache navigateur de serveurs.

> Note : `theme`/`identity` cherchent dans **tous** les search paths (`fs_homepath`, `fs_basepath`,
> pk3) ; en pratique on dépose les fichiers dans le `q3ut4/` du `fs_homepath` (inscriptible).

## Environnement Shell

**L'utilisateur utilise `fish` comme shell par défaut.**
- fish n'est PAS POSIX-compliant : pas de `$()`, `&&` fonctionne différemment, les globs (`*`) sont différents
- **Préfixer les commandes complexes par `bash -c '...'`** pour garantir la compatibilité
- Si une commande bloque, c'est probablement un problème de syntaxe fish
- Les commandes simples (`make`, `git status`, etc.) fonctionnent sans adaptation

## Plateformes cibles

| Plateforme | Architecture | Statut build |
|------------|-------------|:---:|
| Linux | x86_64 | ✅ Principal |
| Linux | x86 (i386) | ✅ CI |
| Linux | ARM64 | ✅ CI |
| Windows | x86_64 (MSYS2/MinGW) | ✅ CI |
| Windows | x86 (MSYS2/MinGW) | ✅ CI |
| Windows | x64/x86/ARM64 (MSVC) | ✅ CI |
| macOS | x86_64 (Intel) | ✅ CI |
| macOS | aarch64 (Apple Silicon) | ✅ CI |

## Git — Remotes

| Remote | URL (fetch) | Push | Rôle |
|--------|-----|------|------|
| `origin` | `git@github.com:miccarrer/urbanterror-optimized.git` | activé | Notre repo (push/pull par défaut) |
| `upstream` | `https://github.com/ec-/Quake3e.git` | `DISABLE` | Quake3e upstream (lecture seule — cherry-pick de fixes) |
| `slim` | `https://github.com/omg-urt/urbanterror-slim.git` | `DISABLE` | Référence UrT features (lecture seule) |

> Le push est explicitement désactivé sur `upstream` et `slim` (`git remote set-url --push <remote> DISABLE`) pour éviter tout push accidentel vers les repos amont.

- Branche principale : `main`
- Convention de commit : Conventional Commits (`feat:`, `fix:`, `docs:`, `refactor:`, `chore:`, `ci:`)
- Tags actuels : dates Quake3e (`2021-03-28`, etc.) — à remplacer par `v1.0.0` (Phase 6)
- Le repo GitHub s'appelle encore `Quake3e` — à renommer `urbanterror-optimized` (Phase 2)

## Outils de qualité (planifiés, Phase 4)
- `.editorconfig` — Indentation cohérente
- `.clang-format` — Formatage C automatique
- `.gitattributes` — Normalisation fins de ligne
