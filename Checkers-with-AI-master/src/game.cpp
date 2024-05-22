#include "game.hpp"
#include <iostream>
#include <algorithm>
#include <cstdlib> // Pour std::rand()

void delay(int miliseconds)
{
	// Crée une horloge SFML pour mesurer le temps écoulé
	sf::Clock clock;
	// Redémarre l'horloge à zéro
	clock.restart();
	// Boucle infinie qui tourne jusqu'à ce que le délai soit atteint
	while (1)
	{
		// Si le temps écoulé en millisecondes dépasse le délai spécifié, sortir de la boucle
		if (clock.getElapsedTime().asMilliseconds() > miliseconds)
			break;
	}
}

Game::Game(Difficulty level) : difficulty(level)
{
	// Initialisation de l'icône de la fenêtre
	icon.loadFromFile("./graphics/red.png");

	// Initialisation des textures
	textures[0].loadFromFile("./graphics/board.jpg");	   // Texture du plateau de jeu
	textures[1].loadFromFile("./graphics/black.png");	   // Texture pour les pions noirs
	textures[2].loadFromFile("./graphics/red.png");		   // Texture pour les pions rouges
	textures[3].loadFromFile("./graphics/black_king.png"); // Texture pour les rois noirs
	textures[4].loadFromFile("./graphics/red_king.png");   // Texture pour les rois rouges

	// Initialisation des sprites
	sprites[0].setTexture(textures[0]); // Associe la texture du plateau au premier sprite
	for (int i = 1; i < 5; ++i)
	{
		sprites[i].setTexture(textures[i]); // Associe les textures des pions et des rois aux sprites correspondants
		sprites[i].setScale(0.6, 0.6);		// Ajuste l'échelle des sprites à 60% de leur taille originale
	}
}

void Game::start()
{
	window.create(sf::VideoMode(board_size, board_size), "Checkers Game");	 // Crée une fenêtre SFML avec les dimensions spécifiées par board_size
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr()); // Définit l'icône de la fenêtre du jeu
	view();																	 // Met à jour l'affichage pour dessiner le contenu initial de la fenêtre
}

int Game::manualMove(OwningPlayer player)
{
	// Vecteurs pour stocker la position de la souris, et les positions de départ et d'arrivée du pion
	sf::Vector2i mouse_position, start, finish;
	// Pointeur vers le vecteur actuellement mis à jour (soit start, soit finish)
	sf::Vector2i *updated_vector;
	// Pointeur intelligent vers le pion actuellement actif
	std::shared_ptr<Pawn> active_pawn = nullptr; // Initialisé à nullptr
	// Indicateur de l'état du bouton de la souris
	bool mouse_pressed = false;

	// Boucle principale du jeu, continue tant que la fenêtre est ouverte
	while (window.isOpen())
	{
		// Met à jour la position de la souris et vérifie si le bouton de la souris est pressé
		mouse_pressed = pollEvents(mouse_position);
		if (mouse_pressed)
		{
			// Vérifie si la position de la souris est dans les limites du plateau
			if (mouse_position.x > border_size && mouse_position.x < board_size - border_size &&
				mouse_position.y > border_size && mouse_position.y < board_size - border_size)
			{
				// Si aucun pion n'est actif, met à jour start, sinon met à jour finish
				if (!active_pawn) // Vérifie si active_pawn est nullptr
				{
					updated_vector = &start; // updated_vector pointe vers start
				}
				else
				{
					updated_vector = &finish; // updated_vector pointe vers finish
				}

				// Calcule les coordonnées du pion en fonction de la position de la souris
				updated_vector->x = (mouse_position.x - border_size) / field_size;
				updated_vector->y = (mouse_position.y - border_size) / field_size;
				updated_vector->y = 7 - updated_vector->y; // Inverse la coordonnée y pour correspondre à l'orientation du plateau

				// Si un pion est actif, vérifie et exécute le mouvement
				if (active_pawn)
				{
					// Vérifie que le pion appartient au joueur actuel
					if (active_pawn->owner == player)
					{
						// Vérifie si le mouvement est valide
						MoveType result = game_board.checkMove(start, finish);
						if (result != INVALID)
						{
							// Exécute le mouvement
							executeMove(start, finish, result);
							return 0; // Retourne 0 pour indiquer le succès du mouvement
						}
					}
					// Réinitialise le pion actif
					active_pawn = nullptr;
				}
				else
				{
					// Récupère le pion à la position de départ et le désigne comme le pion actif
					active_pawn = game_board.getPawn(start);
				}
			}
		}
	}
	// Retourne 1 pour indiquer que le mouvement n'a pas été effectué (par exemple, si la fenêtre est fermée)
	return 1;
}

void Game::play()
{
	Move computer_move;
	OwningPlayer winner = NOBODY;
	while (winner == NOBODY)
	{
		if (getMove(active_player)) // si la sortie est non null -> break
			break;
		// std::cerr << alphabeta(game_board, computer_move, 2, COMPUTER, minus_infty, plus_infty);
		active_player = otherPlayer(active_player);
		winner = game_board.checkWin(active_player);
	}
	if (winner == HUMAN)
		std::cout << " \n You Win! \n";
	else if (winner == COMPUTER)
		std::cout << " \n You Lose :( \n";
}

int Game::computerMove()
{
	Move computer_move;
	sf::Clock clock;
	clock.restart();
	int depth = getSearchDepth();
	alphabeta(game_board, computer_move, depth, COMPUTER, minus_infty, plus_infty);
	// std::cerr << clock.getElapsedTime().asMilliseconds();
	executeMove(computer_move.start, computer_move.finish, computer_move.type);
	return 0;
}

int Game::getMove(OwningPlayer player)
{
	// Vérifie et met à jour l'état des captures possibles pour le joueur actif
	game_board.resolveBeating(player);

	// Vérifie si le joueur actif est l'ordinateur
	if (player == COMPUTER)
	{
		// Exécute le mouvement de l'ordinateur en utilisant l'algorithme d'Alpha-Beta
		return computerMove();
	}
	else
	{
		// Permet à l'humain de faire un mouvement manuel
		return manualMove(HUMAN);
	}
}

bool Game::pollEvents(sf::Vector2i &mouse_position)
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			window.close();
			return false;
		}
		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				mouse_position.x = event.mouseButton.x;
				mouse_position.y = event.mouseButton.y;
				return true;
			}
		}
	}
	return false;
}

// dessin l'état actuel du jeu à l'écran: le dessin du plateau et des pions.
void Game::view()
{
	window.clear();
	// draw the board
	window.draw(sprites[0]);
	int sprite_number;
	// draw the pawns
	for (const auto &pawn_ptr : game_board.pawn_vector)
	{
		if (auto drawn_pawn = pawn_ptr.lock())
		{
			if (drawn_pawn->owner == HUMAN)
				sprite_number = (drawn_pawn->level == king) ? 4 : 2; // 4 pour red_king.png, 2 pour red.png
			else
				sprite_number = (drawn_pawn->level == king) ? 3 : 1;		  // 3 pour black_king.png, 1 pour black.png
			sprites[sprite_number].setPosition(drawn_pawn->x, drawn_pawn->y); // Positionne le sprite à la position du pion
			window.draw(sprites[sprite_number]);
		}
	}
	window.display();
}
// gère l'animation de déplacement d'un pion
void Game::executeMove(sf::Vector2i &start, sf::Vector2i &finish, MoveType type)
{
	if (auto pawn = game_board.movePawn(start, finish, type))
	{
		float distance_x = ((finish.x - start.x) * field_size) / 10;
		float distance_y = ((finish - start).y * field_size) / 10;
		for (int i = 0; i < 10; ++i)
		{
			pawn->x += distance_x;
			pawn->y -= distance_y;
			delay(20);
			view();
		}
		view();
	}
}
int Game::alphabeta(Board &current_board, Move &best_move, int depth, OwningPlayer player, int alpha, int beta)
{
	int value;

	if (depth == 0)
	{
		value = current_board.evaluate(); // Utilise la nouvelle fonction d'évaluation
		return value;
	}

	std::vector<Move> *possible_moves = current_board.getAvailibleMoves(player);

	if (possible_moves->empty())
	{
		delete possible_moves;
		return (player == COMPUTER) ? minus_infty : plus_infty;
	}

	std::vector<int> best_moves_indices;
	int best_value = (player == COMPUTER) ? minus_infty : plus_infty;

	for (unsigned int i = 0; i < possible_moves->size(); ++i)
	{
		Board new_board = current_board;
		new_board.movePawn(possible_moves->at(i));

		if (player == COMPUTER)
		{
			value = alphabeta(new_board, best_move, depth - 1, HUMAN, alpha, beta);
			if (value > best_value)
			{
				best_value = value;
				best_moves_indices.clear();
				best_moves_indices.push_back(i);
			}
			else if (value == best_value)
			{
				best_moves_indices.push_back(i);
			}
			alpha = std::max(alpha, value);
		}
		else
		{
			value = alphabeta(new_board, best_move, depth - 1, COMPUTER, alpha, beta);
			if (value < best_value)
			{
				best_value = value;
				best_moves_indices.clear();
				best_moves_indices.push_back(i);
			}
			else if (value == best_value)
			{
				best_moves_indices.push_back(i);
			}
			beta = std::min(beta, value);
		}

		if (alpha >= beta)
		{
			break;
		}
	}

	if (depth == getSearchDepth() && !best_moves_indices.empty())
	{
		int random_index = best_moves_indices[std::rand() % best_moves_indices.size()];
		best_move = possible_moves->at(random_index);
	}

	delete possible_moves;

	return (player == COMPUTER) ? alpha : beta;
}

int Game::getSearchDepth() const
{
	switch (difficulty)
	{
	case EASY:
		return 2;
	case MEDIUM:
		return 4;
	case HARD:
		return 6;
	default:
		return 4;
	}
}
