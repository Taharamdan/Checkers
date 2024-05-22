#include "board.hpp"
#include <iostream>
#include <cstdlib> // Pour std::rand()

Board::Board()
{ // pawn vector initialization
	pawn_vector.reserve(24);
	float new_x, new_y;
	OwningPlayer new_player;
	std::shared_ptr<Pawn> new_ptr;
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (i % 2 == j % 2)
			{
				if (j < 3 || j > 4)
				{
					new_x = border_size + i * field_size + 5;
					new_y = border_size + (7 - j) * field_size + 5;
					if (j < 3)
						new_player = HUMAN;
					else if (j > 4)
						new_player = COMPUTER;
					new_ptr = std::make_shared<Pawn>(i, j, new_x, new_y, new_player);
					field[i][j] = new_ptr;
					pawn_vector.push_back(std::weak_ptr<Pawn>(new_ptr));
					getVector(new_player).push_back(std::weak_ptr<Pawn>(new_ptr));
				}
			}
		}
	}
}

Board::Board(const Board &copied)
{
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (copied.field[i][j])
			{ // Crée un nouveau pion en copiant le pion de 'copied'
				std::shared_ptr<Pawn> new_ptr = std::shared_ptr<Pawn>(new Pawn(*copied.field[i][j]));
				// Assigne le nouveau pion à la case correspondante du nouveau plateau
				field[i][j] = new_ptr;
				// Ajoute le pion au vecteur des pions globaux
				pawn_vector.push_back(std::weak_ptr<Pawn>(new_ptr));
				// Ajoute le pion au vecteur des pions du joueur approprié
				getVector(new_ptr->owner).push_back(std::weak_ptr<Pawn>(new_ptr));
			}
		}
	}
}
// retourne une référence à un vecteur qui contient les pions d'un joueur specifié
std::vector<std::weak_ptr<Pawn>> &Board::getVector(OwningPlayer player)
{
	if (player == HUMAN)
		return player_pawns[0];
	else
		return player_pawns[1];
}

// obtenir un pointeur vers un pion à des coordonnées spécifiques sur le plateau de jeu
std::shared_ptr<Pawn> Board::getPawn(const sf::Vector2i &coords)
{
	if (field[coords.x][coords.y] != nullptr) //est ce qu'il y a un pion à cette position ?
		return field[coords.x][coords.y];//
	else
		return nullptr;
}
// placer un pion sur le plateau de jeu aux coordonnées spécifiées
int Board::setPawn(const sf::Vector2i &coords, const std::shared_ptr<Pawn> &new_ptr)
{
	// Vérifie si les coordonnées sont valides
	if (coords.x < 0 || coords.x >= 8 || coords.y < 0 || coords.y >= 8)
	{
		return -1; // Code d'erreur pour coordonnées invalides
	}

	field[coords.x][coords.y] = new_ptr;
	return 0; // Indique que l'opération a réussi
}


std::shared_ptr<Pawn> Board::movePawn(sf::Vector2i start, sf::Vector2i finish, MoveType type)
{
	if (auto pawn = getPawn(start)) // Si getPawn retourne un pointeur valide, le pion existe et peut être déplacé.
	{
		if (type == BEAT)
		{
			int dx = finish.x - start.x;
			int dy = finish.y - start.y;
			int stepX = dx / abs(dx); // les directions de déplacement soit 1 ou -1
			int stepY = dy / abs(dy);
			for (int i = 1; i < abs(dx); ++i) // Itère sur toutes les positions intermédiaires entre la position de départ et la position d'arrivée
			{
				sf::Vector2i current_pos = start + sf::Vector2i(i * stepX, i * stepY); // Calcule la position actuelle en ajoutant le déplacement
				auto current_pawn = getPawn(current_pos);							   // Obtient le pion à la position actuelle.
				if (current_pawn && current_pawn->owner == otherPlayer(pawn->owner))   // Vérifie si un pion adverse est présent à la position actuelle.
				{
					getPawn(current_pos).reset();  // Capture le pion adverse
					setPawn(current_pos, nullptr); // Retire le pion capturé du plateau
					break;						   // Sort de la boucle après avoir capturé un pion
				}
			}
		}
		setPawn(start, nullptr);	// Enlève le pion de sa position de départ
		setPawn(finish, pawn);		// Place le pion à sa position d'arrivée
		pawn->coordinates = finish; // mettre à jour les coordonnées du pion

		// Promotion en roi
		if (pawn->owner == HUMAN && finish.y == 7) // y c'est la ligne
		{
			pawn->level = king;
		}
		else if (pawn->owner == COMPUTER && finish.y == 0)
		{
			pawn->level = king;
		}

		resolveBeating(pawn->owner); // Vérifie et met à jour l'état des captures possibles
		return pawn;				 // Retourne le pion déplacé
	}
	return nullptr; // Retourne nullptr si aucun pion n'a été trouvé à la position de départ
}

// permet de déplacer un pion en utilisant un objet Move au lieu de passer trois paramètres séparés
std::shared_ptr<Pawn> Board::movePawn(const Move &move)
{
	return movePawn(move.start, move.finish, move.type);// c'est une foction a un avantage de flexibilité et facicle à lire 
}

// vérifie la validité d'un mouvement en fonction des règles du jeu
MoveType Board::checkMove(sf::Vector2i &start, sf::Vector2i &finish)
{
	MoveType result = INVALID; // Initialisation du résultat du mouvement à INVALID

	// Vérifie que la destination est dans les limites du plateau
	if (finish.x >= 0 && finish.x <= 7 && finish.y >= 0 && finish.y <= 7)
	{
		// Obtient le pion à la position de départ
		if (std::shared_ptr<Pawn> pawn = getPawn(start))
		{
			// Vérifie si le pion est un roi
			if (pawn->level == king)
			{
				int dx = finish.x - start.x;
				int dy = finish.y - start.y;

				// Vérifie que le mouvement est diagonal(Un mouvement est diagonal si le pion se déplace le même nombre de cases en x et en y.)
				if (abs(dx) == abs(dy))
				{
					int stepX = dx / abs(dx); // Direction du mouvement en x
					int stepY = dy / abs(dy); // Direction du mouvement en y
					bool path_clear = true;	  // Indicateur de chemin libre
					int enemy_pawns = 0;	  // Compteur de pions ennemis sur le chemin

					// Parcourt chaque case entre la position de départ et d'arrivée
					for (int i = 1; i < abs(dx); ++i)
					{
						auto current_pos = start + sf::Vector2i(i * stepX, i * stepY);
						auto current_pawn = getPawn(current_pos);
						if (current_pawn != nullptr)
						{
							// Si une pièce alliée est trouvée sur le chemin, le mouvement est invalide
							if (current_pawn->owner == pawn->owner)
							{
								path_clear = false;
								break;
							}
							// Si une pièce ennemie est trouvée, elle est comptée
							else if (current_pawn->owner == otherPlayer(pawn->owner))
							{
								enemy_pawns++;
							}
						}
					}

					// Vérifie le type de mouvement en fonction des pions ennemis rencontrés
					if (path_clear)
					{
						if (enemy_pawns == 0)
						{
							// Si aucune pièce ennemie n'est trouvée sur le chemin, le mouvement est normal
							if (getPawn(finish) == nullptr)
							{
								result = NORMAL;
							}
						}
						else if (enemy_pawns == 1)
						{
							// Si une seule pièce ennemie est trouvée sur le chemin, le mouvement est une capture
							if (getPawn(finish) == nullptr)
							{
								result = BEAT;
							}
						}
					}
				}
			}
			else // Vérifie les mouvements pour un pion normal
			{
				int direction = (pawn->owner == COMPUTER) ? -1 : 1; // Direction du mouvement en fonction du propriétaire

				// Vérifie les mouvements normaux (d'une case en avant en diagonale)
				if (finish.y == start.y + direction)
				{
					if (finish.x == start.x + 1 || finish.x == start.x - 1)
					{
						// Vérifie si la case de destination est vide
						if (!getPawn(finish))
						{
							result = NORMAL;
						}
					}
				}
				// Vérifie les mouvements de capture (deux cases en avant en diagonale)
				else if (finish.y == start.y + 2 * direction)
				{
					if (finish.x == start.x + 2 || finish.x == start.x - 2)
					{
						// Vérifie si la case de destination est vide
						if (!getPawn(finish))
						{
							// Calcule la position du pion capturé
							sf::Vector2i beaten_pawn(start.x + (finish.x - start.x) / 2, start.y + direction);
							auto beaten_pawn_ptr = getPawn(beaten_pawn);
							if (beaten_pawn_ptr && beaten_pawn_ptr->owner == otherPlayer(pawn->owner))
							{
								result = BEAT;
							}
						}
					}
				}
			}
		}
	}

	return result; // Retourne le résultat du type de mouvement
}

// Retourne une référence au booléen indiquant si des captures sont possibles pour le joueur spécifié
bool &Board::getBeatPossible(OwningPlayer player) // une référence permet à l'appelant de modifier directement la valeur dans le tableau beat_possible.
{
	// Si le joueur est l'ordinateur, retourne la référence à beat_possible[1]
	if (player == COMPUTER)
		return beat_possible[1];
	// Sinon, retourne la référence à beat_possible[0] pour le joueur humain
	return beat_possible[0];
}

// Met à jour l'état des captures possibles pour le joueur spécifié après un mouvement
void Board::resolveBeating(OwningPlayer player)
{
	// Réinitialise l'indicateur de capture possible pour le joueur spécifié à false
	getBeatPossible(player) = false;

	// Obtient tous les mouvements disponibles pour le joueur spécifié
	std::vector<Move> *move_vector = getAvailibleMoves(player);

	// Parcourt chaque mouvement disponible
	for (auto tested_move : *move_vector)
	{
		// Si un mouvement de type BEAT est trouvé, met à jour l'indicateur de capture possible à true
		if (tested_move.type == BEAT)
		{
			getBeatPossible(player) = true;
		}
	}

	// Libère la mémoire allouée pour le vecteur des mouvements disponibles
	delete move_vector;
}

std::vector<Move> *Board::getAvailibleMoves(OwningPlayer player)
{
	std::vector<Move> *move_vector = new std::vector<Move>;
	for (auto pawn_ptr : getVector(player))
	{
		if (auto pawn = pawn_ptr.lock()) // Convertit le weak_ptr en shared_ptr
		{
			auto new_moves = getAvailibleMoves(player, pawn);
			if (!new_moves->empty())
				move_vector->insert(move_vector->end(), new_moves->begin(), new_moves->end());
			delete new_moves;
		}
	}
	return move_vector;
}

std::vector<Move> *Board::getAvailibleMoves(OwningPlayer player, const std::shared_ptr<Pawn> pawn)
{
	std::vector<Move> *move_vector = new std::vector<Move>;
	sf::Vector2i start, finish;
	int direction = (player == COMPUTER) ? -1 : 1;

	if (pawn)
	{
		start = pawn->coordinates;

		// Mouvements pour pions normaux
		for (int dy : {1, 2})
		{
			for (int dx : {-1, 1})
			{
				finish = start + sf::Vector2i(dx * dy, dy * direction);
				MoveType result = checkMove(start, finish);
				if (result != INVALID)
				{
					Move new_move = Move(start, finish, result);
					move_vector->push_back(new_move);
				}
			}
		}

		// Mouvements pour les rois
		if (pawn->level == king)
		{
			for (int dy : {-1, 1})
			{
				for (int dx : {-1, 1})
				{
					// Déplacements normaux et captures en diagonale
					for (int dist = 1; dist < 8; ++dist)
					{
						finish = start + sf::Vector2i(dx * dist, dy * dist);
						MoveType result = checkMove(start, finish);
						if (result != INVALID)
						{
							Move new_move = Move(start, finish, result);
							move_vector->push_back(new_move);

							// Si un mouvement de capture est possible, arrêtez de vérifier plus loin dans cette direction
							if (result == BEAT)
							{
								break;
							}
						}
						else
						{
							// Arrêtez de vérifier plus loin si le mouvement est invalide
							break;
						}
					}
				}
			}
		}
	}
	return move_vector;
}

int Board::getScore(OwningPlayer player)
{
	int score = 0;
	for (auto pawn_weak : getVector(player))
	{
		if (auto pawn = pawn_weak.lock())
		{
			std::vector<Move> *move_vector = new std::vector<Move>;
			int x = pawn->coordinates.x;
			int y = pawn->coordinates.y;
			score += 10;
			if (player == HUMAN)
			{
				if (y == 2 || y == 3)
					score += 1;
				else if (y == 4 || y == 5)
					score += 3;
				else if (y == 6 || y == 7)
					score += 5;
			}
			else
			{
				if (y == 5 || y == 4)
					score += 1;
				else if (y == 3 || y == 2)
					score += 3;
				else if (y == 1 || y == 0)
					score += 5;
			}
			if ((x == 0 || x == 7) && (y == 0 || y == 7))
				score += 2;
			else if ((x == 1 || x == 6) && (y == 1 || y == 6))
				score += 1;

			move_vector = getAvailibleMoves(player, pawn);
			if (!move_vector->empty())
			{
				for (auto tested_move : *move_vector)
				{
					if (tested_move.type == BEAT)
						score += 30;
				}
			}
			delete move_vector;
		}
	}
	return score;
}

OwningPlayer Board::checkWin(OwningPlayer player)
{
	resolveBeating(player);
	OwningPlayer winner = NOBODY;
	std::vector<Move> *availible_moves;

	availible_moves = getAvailibleMoves(player);
	if (availible_moves->empty())
	{
		winner = otherPlayer(player);
	}
	else
	{
		int pawn_count = 0;
		for (auto checked_pawn : getVector(player))
		{
			if (!checked_pawn.expired()) // Vérifie si le pion est toujours valide (non détruit).
				++pawn_count;
		}
		if (pawn_count == 0)
		{
			winner = otherPlayer(player);
		}
	}
	delete availible_moves;
	return winner;
}
int Board::evaluate()
{
	int score = getScore(COMPUTER) - getScore(HUMAN);
	int random_factor = rand() % 10; // Ajoute un facteur aléatoire entre 0 et 9
	return score + random_factor;
}
