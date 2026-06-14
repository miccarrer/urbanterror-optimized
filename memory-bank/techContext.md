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
- **Makefile** (principal) — Support Linux, macOS, MinGW, Raspberry Pi, PowerPC
- **CMakeLists.txt** (alternatif) — Maintenu mais `CNAME` encore à `"quake3e"` (Phase 2 pending)
- **MSVC** (`code/win32/msvc2017/quake3e.sln`) — Windows natif

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
| `origin` | `git@github.com:miccarrer/Quake3e.git` | activé | Notre repo (push/pull par défaut) |
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
