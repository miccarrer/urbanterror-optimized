# Progress — Urban Terror Optimized

## État global du projet

**Phase actuelle** : EXÉCUTION — Phase 1A terminée, Phase 1B en cours de planification.

---

## ✅ Terminé

### Analyse & Planification (2026-06-14)
- [x] Analyse des 66 commits de `omg-urt/urbanterror-slim` vs notre projet
- [x] Identification de 11 fonctionnalités manquantes, 5 partielles, 13 déjà présentes
- [x] Génération de `ANALYSIS_REPORT.md`
- [x] Création de `ROADMAP.md` (6 phases détaillées)
- [x] Configuration du Memory Bank (`.clinerules` + 6 fichiers `memory-bank/`)

### Phase 1A — Fixes sécurité (2026-06-15)
- [x] x86 JIT NaN fix (`56f03bca`) — Vérification complète
- [x] FS_GamePak security check pour empêcher downloads UrT paks (`0504f377`)
- [x] CL_FirstDownload pour filtrer les serveurs moddés (`32499141`)
- [x] Master servers + modversion cvar (`4bd6632f`)
- [x] Build test: ✅ Succès
- [x] Commits: 3 atomiques avec conventional commits + Copilot co-author

---

## 🔄 En cours

*Préparation pour Phase 1B — Features UrT*

---

## 📋 À faire (Roadmap — voir `ROADMAP.md` pour le détail)

### Phase 1 — Intégration du code
- [x] **1A** — Fixes sécurité : JIT NaN (`56f03bca`), downloads (`0504f377`, `32499141`), master server (`4bd6632f`)
- [ ] **1B** — Features UrT : Console à onglets (`2c70fdc0`), Tellme (`d4f12aa7`), Demo UrT (`9579fc7e`)
- [ ] **1C** — Cvars serveur : sv_sayprefix (`dd52e95f`), sv_nofalldamage (`be301ebf`), sv_infiniteStamina (`bbb587d4`), referee fix (`b834398f`)
- [ ] **1D** — (Optionnel) dmaHD (`213e0e5d`)

### Phase 2 — Branding
- [ ] CMakeLists.txt : `CNAME` quake3e → urbanterror-optimized
- [ ] CI : Renommer artifacts, nettoyer jobs
- [ ] README.md, BUILD.md : Réécrire pour UrT
- [ ] Supprimer docs Q3A legacy
- [ ] LICENSE, tags Git, remote

### Phase 3 — Documentation
- [ ] Réorganiser `docs/` (legal/, analysis/, etc.)
- [ ] Créer CHANGELOG.md, CONTRIBUTING.md, SECURITY.md
- [ ] Créer CREDITS.md, CVARS.md, urt-features.md

### Phase 4 — Conventions dev
- [ ] `.editorconfig`, `.clang-format`, `.gitattributes`
- [ ] CONTRIBUTING.md (workflow, conventional commits)

### Phase 5 — CI/CD
- [ ] Séparer ci.yml / release.yml
- [ ] Lint clang-format, caching, release notes

### Phase 6 — Nettoyage final
- [ ] Build test Linux
- [ ] Tags Git, remote
- [ ] Vérification finale

---

## 📊 Métriques

| Indicateur | Valeur |
|------------|--------|
| Commits urbanterror-slim analysés | 66 |
| Fonctionnalités déjà intégrées | 13 |
| Fonctionnalités manquantes à intégrer | 11 |
| Fonctionnalités partielles | 5 |
| Phases roadmap | 6 |