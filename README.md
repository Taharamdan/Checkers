**README**
Le jeu de dames est un jeu de plateau pour deux joueurs, chacun disposant de pièces de couleur opposée placées sur un damier. Le but est de capturer les pièces adverses en sautant par-dessus elles en diagonale, tout en évitant d'être capturé soi-même. Le joueur qui capture toutes les pièces de son adversaire ou qui le met dans l'incapacité de jouer gagne la partie.


Ce projet consiste en un jeu de dames développé en C++ utilisant la bibliothèque SFML (Simple and Fast Multimedia Library) pour la partie graphique. Le jeu comporte deux fichiers principaux :

1. **game.cpp**
2. **board.cpp**

### game.cpp

Ce fichier contient la logique principale du jeu, y compris la gestion de la fenêtre, la capture des événements, le déplacement des pions, et l'algorithme d'IA (algorithme Alpha Beta ) utilisé pour prendre des décisions pour l'ordinateur. Voici un aperçu des principales fonctions :

- **Game::Game(Difficulty level)** : Constructeur de la classe `Game` qui initialise les textures et les sprites du jeu en fonction du niveau de difficulté spécifié.
- **Game::start()** : Démarre le jeu en créant la fenêtre SFML et en affichant le contenu initial.
- **Game::manualMove(OwningPlayer player)** : Permet au joueur humain de faire un mouvement manuel en cliquant sur les cases de la fenêtre.
- **Game::play()** : Gère le déroulement du jeu en alternant entre les tours des joueurs humain et de l'ordinateur, jusqu'à ce qu'un joueur gagne ou que le jeu se termine.
- **Game::computerMove()** : Permet à l'ordinateur de faire un mouvement en utilisant l'algorithme minimax avec élagage alpha-bêta.
- **Game::getMove(OwningPlayer player)** : Obtient le mouvement à effectuer par le joueur spécifié, qu'il soit humain ou ordinateur.
- **Game::pollEvents(sf::Vector2i &mouse_position)** : Capture les événements de la fenêtre, tels que la fermeture de la fenêtre ou les clics de souris.
- **Game::view()** : Dessine l'état actuel du jeu à l'écran, y compris le plateau et les pions.
- **Game::executeMove(sf::Vector2i &start, sf::Vector2i &finish, MoveType type)** : Anime le déplacement d'un pion sur le plateau.

### board.cpp

Ce fichier contient la définition de la classe `Board`, qui représente le plateau de jeu et gère les opérations liées aux pions. Voici un aperçu des principales fonctions :

- **Board::Board()** : Constructeur qui initialise le plateau de jeu avec les pions placés aux positions initiales.
- **Board::Board(const Board &copied)** : Constructeur de copie qui crée une copie profonde du plateau.
- **Board::getVector(OwningPlayer player)** : Retourne une référence au vecteur contenant les pions d'un joueur spécifié.
- **Board::getPawn(const sf::Vector2i &coords)** : Obtient un pointeur vers un pion aux coordonnées spécifiées sur le plateau de jeu.
- **Board::setPawn(const sf::Vector2i &coords, const std::shared_ptr<Pawn> &new_ptr)** : Place un pion sur le plateau de jeu aux coordonnées spécifiées.
- **Board::movePawn(sf::Vector2i start, sf::Vector2i finish, MoveType type)** : Déplace un pion sur le plateau de jeu en vérifiant la validité du mouvement.
- **Board::checkMove(sf::Vector2i &start, sf::Vector2i &finish)** : Vérifie la validité d'un mouvement en fonction des règles du jeu.
- **Board::getBeatPossible(OwningPlayer player)** : Retourne une référence au booléen indiquant si des captures sont possibles pour le joueur spécifié.
- **Board::resolveBeating(OwningPlayer player)** : Met à jour l'état des captures possibles pour le joueur spécifié après un mouvement.
- **Board::getAvailibleMoves(OwningPlayer player)** : Obtient tous les mouvements disponibles pour un joueur spécifié.

Ces deux fichiers forment le noyau du jeu de dames développé dans ce projet.


### main.cpp

- Interface graphique utilisant la bibliothèque SFML pour une expérience de jeu immersive.
- Trois niveaux de difficulté : facile, moyen et difficile.
- Instructions claires pour compiler et exécuter le jeu avec différents niveaux de difficulté.
