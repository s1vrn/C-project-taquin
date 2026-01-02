# Documentation Technique - Jeu de Taquin (15-Puzzle)

Ce document explique en détail le fonctionnement du code source `taquin.c`, sa structure, la logique de jeu implémentée, ainsi que l'interface utilisateur graphique réalisée avec la bibliothèque **SDL3**.

## 1. Structure Globale du Code

Le projet est contenu dans un fichier unique en C utilisant la bibliothèque **SDL3** pour la gestion de la fenêtre, du rendu graphique et des événements (clavier/souris).

### Portabilité (Windows / Mac / Linux)
Le code a été adapté pour être compilable sur différents systèmes d'exploitation :
- **Bibliothèque SDL3** : Le code utilise désormais la dernière version de SDL.
- **Includes** : `#include <SDL3/SDL.h>`.
- **Scripts de construction mis à jour** : Les scripts lient désormais contre `SDL3`.

### Bibliothèques et Constantes
- **Bibliothèques** : `<SDL3/SDL.h>`, `<stdbool.h>`, `<stdio.h>`, `<stdlib.h>`, `<time.h>`.
- **Macros** : `MAX_GRID_SIZE 5`, `TILE_SIZE 150`, `GAP 5`, `UI_HEIGHT 60`.

---

## 2. Compilation et Exécution

**Prérequis** : Vous devez avoir **SDL3** installé sur votre système (et non SDL2).

### Sur macOS / Linux
Assurez-vous d'avoir SDL3 installé (par exemple via compilation source ou gestionnaire de paquets si disponible).
1. Ouvrez un terminal.
2. Lancez le script de construction :
   ```bash
   ./build.sh
   ```
3. Lancez le jeu :
   ```bash
   ./taquin
   ```

### Sur Windows
Assurez-vous d'avoir un environnement de développement C (comme MinGW) et les bibliothèques de développement **SDL3** configurées (DLLs dans le PATH).
1. Double-cliquez sur `build.bat` ou lancez-le depuis l'invite de commande.
2. Lancez `taquin.exe`.

---

## 3. Analyse des Fonctions & UI (Modifications SDL3)

### Changements Majeurs SDL3
- **Coordonnées Flottantes** : SDL3 utilise des `float` pour le rendu (`SDL_FRect` au lieu de `SDL_Rect`). Toutes les fonctions de dessin (`draw_number`, `draw_home_icon`, rendu des tuiles) ont été adaptées pour utiliser des floats, offrant une précision sub-pixel potentielle.
- **Initialisation Fenêtre** : `SDL_CreateWindow` ne prend plus de coordonnées X/Y explicites par défaut.
- **Gestion des Événements** : Utilisation des constantes `SDL_EVENT_QUIT`, `SDL_EVENT_KEY_DOWN`, etc.

### Menu & Navigation
- **Menu Principal** : Choix modulable de la taille (3, 4, 5).
- **Interface Jeu** : Grille + Barre de navigation (Maison).
- **Logique** : Machine à états (`STATE_MENU`, `STATE_PLAYING`) conservée et adaptée.
