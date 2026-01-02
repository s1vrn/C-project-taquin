# Jeu de Taquin (15-Puzzle) - SDL3 Version

Bienvenue sur le dépôt du projet Jeu de Taquin, une implémentation moderne en C utilisant la bibliothèque **SDL3**.

## 🎮 Fonctionnalités

*   **Grille Dynamique** : Choisissez votre difficulté au démarrage (3x3, 4x4 ou 5x5).
*   **Interface Graphique** : Rendu vectoriel propre et fluide (aucun asset externe nécessaire).
*   **Navigation** : Menu principal et bouton "Accueil" en jeu pour changer de difficulté à tout moment.
*   **Cross-Platform** : Compatible **macOS**, **Linux** et **Windows**.

## 🛠 Prérequis

Ce projet nécessite la bibliothèque **SDL3**.
*   **macOS** : `brew install sdl3` (si disponible) ou installation manuelle.
*   **Windows** : Avoir les DLLs SDL3 et un compilateur MinGW/GCC.
*   **Linux** : Installer SDL3 via votre gestionnaire de paquets ou les sources.

## 🚀 Compilation et Exécution

### macOS / Linux
Utilisez le script fourni :
```bash
./build.sh
./taquin
```

### Windows
Double-cliquez sur `build.bat` ou lancez :
```cmd
build.bat
taquin.exe
```

## 📚 Documentation

Pour une explication détaillée du code source, de la structure du projet et de la logique interne, consultez le fichier [DOCUMENTATION.md](./DOCUMENTATION.md).

## 👨‍💻 Auteurs

Projet développé en C avec SDL3.
