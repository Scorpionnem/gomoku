# Gomoku

Implémentation du Gomoku (variante 42) en C++20, avec une interface graphique SDL2 et une IA basée sur l’alpha-beta.

Le plateau est un goban **19×19**. Le but est d’aligner **cinq pierres** ou de capturer **cinq paires** adverses (10 pierres). L’IA doit répondre en **moins de 500 ms**.

## Règles

- **Alignement** : cinq pierres consécutives (horizontal, vertical ou diagonal). Un alignement n’est pas gagnant tant que l’adversaire peut encore le casser en capturant une des pierres de la ligne.
- **Captures** : poser une pierre qui encadre exactement deux pierres adverses (`X O O X`) les retire du plateau. Cinq paires capturées = victoire.
- **Double-trois** : un coup qui crée deux « trois libres » (alignements de trois ouverts des deux côtés) est interdit. Les cases illégales sont affichées en rouge.



## Prérequis

- Compilateur C++20 (`c++`/`g++`)
- `make`
- **SDL2**
- **SDL2_ttf**
- OpenGL (`-lGL`)

Sur les machines 42, SDL2 est souvent déjà installé via `suprapack` / Cosmos dans `~/.local`. SDL2_ttf ne l’est pas toujours : le script `script.sh` le compile et l’installe dans `~/.local` **sans sudo**.

```bash
# SDL2 (si besoin)
suprapack install libsdl2

# SDL2_ttf
./script.sh
```

Ajoute ces variables à `~/.zshrc` si elles n’y sont pas déjà :

```bash
export PATH="$HOME/.local/bin:$PATH"
export LD_LIBRARY_PATH="$HOME/.local/lib:$LD_LIBRARY_PATH"
export PKG_CONFIG_PATH="$HOME/.local/lib/pkgconfig:$PKG_CONFIG_PATH"
```

Le Makefile utilise `sdl2-config` pour les flags SDL2, et lie `-lSDL2_ttf -lGL`.

## Compilation

À lancer **depuis la racine du dépôt** (les textures et la police sont chargées avec des chemins relatifs `assets/…`).

```bash
make          # compile le binaire Gomoku
make clean    # supprime les objets (.build/)
make fclean   # clean + binaire
make re       # recompilation complète
```

Le binaire produit s’appelle `Gomoku`.

## Lancement

```bash
./Gomoku                 # ouvre le menu
./Gomoku human ai        # joueur 1 humain, joueur 2 IA (saute le menu)
./Gomoku ai human
./Gomoku human human
./Gomoku ai ai
```

Les arguments sont `human` ou `ai`, dans l’ordre joueur 1 puis joueur 2. Toute autre valeur fait quitter le programme.

**Échap** ou fermeture de la fenêtre : quitter.

## Interface



### Menu

Pour chaque joueur :

- **Human / AI**
- **Debug** : heatmap des scores IA + stats (profondeur, nœuds, etc.)
- **Hint** : contour bleu sur le meilleur coup calculé par l’IA
- **Couleur** des pierres (noir, blanc, rouge, bleu, vert, jaune)

Puis **Start**.

### Partie

- Clic gauche sur une intersection pour jouer (tour humain).
- Panneaux gauche / droite : joueur actif, temps du dernier coup IA, captures, boutons Debug / Hint.
- Écran de fin : **Play Again** ou **Main Menu**.

En **IA vs IA**, le premier coup est tiré au hasard sur le plateau pour varier les parties.

## Architecture

```
Gomoku
├── Makefile
├── script.sh              # installe SDL2_ttf dans ~/.local
├── assets/                # plateau, pions, hotbar, police, splash
├── inc/
│   ├── Gomoku.hpp         # boucle de jeu, UI, états
│   ├── Board.hpp          # plateau, captures, hash Zobrist
│   ├── Move.hpp           # coups légaux / double-trois
│   ├── AI.hpp             # recherche
│   ├── Heuristic.hpp      # évaluation statique
│   ├── Chrono.hpp         # timeout 500 ms
│   └── platform/          # Window + Input (SDL2)
└── src/                   # implémentations correspondantes
```

Le programme passe par trois états : **MENU** → **GAME** → **GAME_DONE**.

- `Gomoku` gère la fenêtre, les clics, le tour et l’affichage.
- `Board` stocke les 361 cases, l’historique (pour `undo` pendant la recherche), les captures et un hash Zobrist.
- `Move` génère les coups proches des pierres déjà posées, filtre le double-trois, et marque les cases illégales.
- `AI` choisit le coup ; `Heuristic` note une position.



## IA

La recherche doit tenir dans **0,495 s** (`AI::TIME_LIMIT`).

1. **Candidats** : uniquement les cases dans un rayon de 2 autour des pierres existantes (sinon le centre).
2. **Ordonnancement** : captures et alignements d’abord, pour que l’élagage alpha-beta coupe plus tôt.
3. **Iterative deepening** : profondeur 1, puis 2, … jusqu’à 10, en gardant le meilleur coup de la dernière profondeur **terminée**.
4. **Alpha-beta** avec table de transposition (Zobrist, ~1 M d’entrées).
5. **Élagage de largeur** : au plus 6 coups examinés par nœud (`MAX_CANDIDATES`).
6. **Évaluation** : alignements (longueur × extrémités ouvertes), captures déjà faites, menaces de capture. Les pierres vulnérables à une capture voient leur score d’alignement réduit.

Le mode Hint / Debug lance la même recherche côté humain pour afficher le meilleur coup et la heatmap des scores.

## Dépendances d’assets

Le programme charge au démarrage (échec si un fichier manque) :


| Fichier                                 | Usage                                                |
| --------------------------------------- | ---------------------------------------------------- |
| `assets/gomoku_board.bmp`               | Plateau                                              |
| `assets/white_piece.bmp`                | Texture des pions (teintée selon la couleur choisie) |
| `assets/hotbar.bmp`                     | Bandeau sous le plateau                              |
| `assets/Minecraft.ttf`                  | Police UI                                            |
| `assets/mewen.bmp` / `assets/sacha.bmp` | Fond du menu (tirage aléatoire)                      |


