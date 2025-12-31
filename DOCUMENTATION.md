# Documentation Technique - Jeu de Taquin (15-Puzzle)

Ce document explique en détail le fonctionnement du code source `taquin.c`, sa structure, la logique de jeu implémentée, ainsi que l'interface utilisateur graphique réalisée avec la bibliothèque SDL2.

## 1. Structure Globale du Code

Le projet est contenu dans un fichier unique en C utilisant la bibliothèque **SDL2** pour la gestion de la fenêtre, du rendu graphique et des événements (clavier/souris).

### Bibliothèques et Constantes
- **Bibliothèques** :
  - `<SDL2/SDL.h>` : Pour l'interface graphique.
  - `<stdbool.h>`, `<stdio.h>`, `<stdlib.h>`, `<time.h>` : Pour les types booléens, les entrées/sorties, la gestion mémoire et l'aléatoire.
- **Macros (`#define`)** :
  - `GRID_SIZE 4` : Définit une grille de 4x4 (15 tuiles + 1 vide).
  - `TILE_SIZE 150` : La taille en pixels de chaque tuile.
  - `WINDOW_WIDTH`, `WINDOW_HEIGHT` : Calculés dynamiquement selon la taille de la grille.

### Structures de Données
- **`GameState`** : Cette structure stocke l'état complet du jeu.
  - `grid[GRID_SIZE][GRID_SIZE]` : Tableau 2D d'entiers représentant le plateau. `0` représente la case vide.
  - `empty_x`, `empty_y` : Coordonnées (x, y) de la case vide pour un accès rapide sans parcourir toute la grille.
- **`DIGIT_SEGMENTS`** : Un tableau constant (Lookup Table) définissant quels segments (haut, bas, gauche, droite, etc.) allumer pour dessiner les chiffres de 0 à 9, simulant un affichage 7 segments.

---

## 2. Analyse des Fonctions

### Fonctions Logiques (Backend)

#### `void init_game(GameState *game)`
- **Rôle** : Initialise le plateau de jeu à l'état "résolu".
- **Logique** : Remplit la grille avec les nombres de 1 à 15 dans l'ordre, et place le `0` (vide) en dernière position (bas-droite).

#### `void move_tile(GameState *game, int x, int y)`
- **Rôle** : Gère le déplacement d'une tuile.
- **Logique** :
  1. Vérifie si la tuile aux coordonnées `(x, y)` est adjacente à la case vide (en comparant avec `game->empty_x` et `game->empty_y`).
  2. Si valide, échange les valeurs dans la grille.
  3. Met à jour les coordonnées de la case vide.

#### `void shuffle_game(GameState *game)`
- **Rôle** : Mélange le plateau pour commencer une nouvelle partie.
- **Logique** : Au lieu de placer les tuiles au hasard (ce qui pourrait rendre le puzzle impossible à résoudre), l'algorithme effectue **1000 déplacements valides aléatoires**. Cela garantit que la configuration finale est toujours résoluble.

#### `bool check_win(GameState *game)`
- **Rôle** : Vérifie si le joueur a gagné.
- **Logique** : Parcourt toute la grille et vérifie que chaque case contient le nombre attendu (1, 2, 3... jusqu'à 15, suivi de 0). Retourne `true` si tout est en ordre.

### Fonctions Graphiques (Frontend / UI)

#### `void draw_number(SDL_Renderer *renderer, int number, int x, int y, int size)`
- **Rôle** : Dessine un nombre à l'écran sans utiliser de police de caractères (TTF).
- **Logique** :
  - Décompose le nombre en chiffres (ex: 12 -> 1 et 2).
  - Pour chaque chiffre, utilise le tableau `DIGIT_SEGMENTS` pour savoir quels rectangles (segments) dessiner.
  - Utilise `SDL_RenderFillRect` pour dessiner chaque segment. C'est une approche "bas niveau" créative pour éviter de charger des fichiers de police externes.

#### `int main(int argc, char *argv[])`
- **Rôle** : Point d'entrée, boucle principale et gestion des événements.
- **Structure** :
  1. **Initialisation** : Démarre SDL, crée la fenêtre et le "renderer" (moteur de rendu).
  2. **Initialisation Jeu** : Appelle `init_game` puis `shuffle_game`.
  3. **Boucle de Jeu (`while (running)`)** :
     - **Gestion des Événements (`SDL_PollEvent`)** :
       - `SDL_QUIT` : Ferme l'application.
       - `SDL_MOUSEBUTTONDOWN` : Calcule sur quelle tuile le joueur a cliqué (`x / TILE_SIZE`) et tente de la bouger.
       - `SDL_KEYDOWN` : Gère les flèches directionnelles pour déplacer les tuiles (ou `R` pour redémarrer).
     - **Rendu (`Draw`)** :
       - Efface l'écran (fond noir ou vert si gagné).
       - Parcourt la grille et dessine un rectangle coloré pour chaque tuile non vide.
       - Dessine le numéro sur la tuile via `draw_number`.
       - Affiche le tout avec `SDL_RenderPresent`.

---

## 3. Développement de la Logique et de l'Interface

### La Logique (Logic Development)
La logique repose sur une machine à états simple (`GameState`). Le choix crucial a été la méthode de mélange (`shuffle_game`).
- **Problème potentiel** : Si on remplit la grille totalement au hasard, 50% des configurations sont mathématiquement insolubles (parité des permutations).
- **Solution choisie** : Partir d'un état résolu et simuler des mouvements aléatoires inverses. Cela garantit mathématiquement la solvabilité.

### L'Interface Utilisateur (UI Development)
L'interface a été conçue pour être autonome (aucun asset image ou police externe requis).
- **Rendu vectoriel** : Tout est dessiné via des primitives géométriques (rectangles).
- **Feedback visuel** :
  - Les tuiles ont des couleurs alternées (paire/impaire) pour un aspect "damier" agréable.
  - L'écran devient vert (`RGB 0, 200, 0`) lorsque la victoire est détectée, offrant une récompense visuelle immédiate.
- **Contrôles** : Support hybride (Souris + Clavier) pour l'accessibilité et le confort de jeu.

Ce code est un exemple compact d'application interactive en C, séparant proprement les données (struct GameState) de la représentation visuelle (SDL calls).
