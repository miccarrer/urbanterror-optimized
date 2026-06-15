# Project Brief — Urban Terror Optimized

## Qu'est-ce que ce projet ?

**Urban Terror Optimized** est un moteur de jeu (engine) dérivé de Quake III Arena, optimisé pour Urban Terror 4.x. C'est un fork de `ec-/Quake3e`, lui-même fork d'`ioquake3`.

## Objectif

Fournir un moteur client/serveur **moderne, performant et sécurisé** pour Urban Terror 4.x, en intégrant :
- Les optimisations du moteur Quake3e (renderers OpenGL/Vulkan, VM, memory allocator)
- Les fonctionnalités spécifiques Urban Terror (console à onglets, tellme, demo UrT, etc.)
- Des conventions de développement modernes (CI/CD, documentation, formatage)

## Origine et généalogie

```
id Software (Quake III Arena GPL)
  └── ioquake3 (ioq3)
       └── ec-/Quake3e (optimisations renderer, VM, etc.)
            ├── omg-urt/urbanterror-slim (features UrT)
            └── urbanterror-optimized (CE PROJET)
```

## Périmètre

- **Ce qui est inclus** : Code du moteur (engine) — client, serveur, renderers, VM, botlib, etc.
- **Ce qui n'est PAS inclus** : Assets de jeu (maps, modèles, sons), code QVM du mod UrT (game logic), fichiers `.pk3`

## License

GPL v2 — voir `LICENSE`

## Statut

Projet en cours de structuration. La roadmap complète est dans `ROADMAP.md`.