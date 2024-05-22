#ifndef BOARD_HPP
#define BOARD_HPP

#include "pawn.hpp"

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

const float board_size = 800;
const float field_size = 77.5;
const float border_size = 91;

enum MoveType
{
	INVALID,
	NORMAL,
	BEAT,
};

struct Move
{
	Move() = default;
	Move(sf::Vector2i start_, sf::Vector2i finish_, MoveType type_) : start(start_), finish(finish_), type(type_){};
	sf::Vector2i start;
	sf::Vector2i finish;
	MoveType type;
};


class Board
{
public:
	Board();
	//on a pas besoin d'un destructeur car on utilise des pionteurs intéllegents 
	Board(const Board &copied);

	// Tableau 2D représentant le plateau de jeu, où chaque case peut contenir un pointeur vers un pion (nullptr si la case est vide).
	std::shared_ptr<Pawn> field[8][8] = {nullptr};
	// Vecteur contenant des pointeurs faibles vers tous les pions sur le plateau, permettant de gérer et de parcourir facilement tous les pions.
	std::vector<std::weak_ptr<Pawn>> pawn_vector;
	// Tableaux de vecteurs contenant des pointeurs faibles vers les pions de chaque joueur(indice 0 pour HUMAN, indice 1 pour COMPUTER)
	std::vector<std::weak_ptr<Pawn>> player_pawns[2];

	// Indicateur pour chaque joueur indiquant si une capture est possible (indice 0 pour HUMAN, indice 1 pour COMPUTER).
	bool beat_possible[2] = {false, false};
	// Retourne une référence à l'indicateur de capture possible pour le joueur spécifié.
	bool &getBeatPossible(OwningPlayer player);
	void resolveBeating(OwningPlayer player);

	// Retourne une référence au vecteur de pions appartenant au joueur spécifié.
	std::vector<std::weak_ptr<Pawn>> &getVector(OwningPlayer player);
	// Retourne un pointeur partagé vers le pion situé aux coordonnées spécifiées.
	std::shared_ptr<Pawn> getPawn(const sf::Vector2i &coords);
	// Place un nouveau pion aux coordonnées spécifiées et retourne 0 si réussi, -1 sinon.
	int setPawn(const sf::Vector2i &coords, const std::shared_ptr<Pawn> &new_ptr);
	// Déplace un pion des coordonnées de départ aux coordonnées d'arrivée avec un type de mouvement spécifié.
	std::shared_ptr<Pawn> movePawn(sf::Vector2i start, sf::Vector2i finish, MoveType type);
	// Déplace un pion en utilisant un objet Move contenant les coordonnées de départ, d'arrivée et le type de mouvement.
	std::shared_ptr<Pawn> movePawn(const Move &move);
	// détermine le type de mouvement entre les positions de départ et d'arrivée fournies
	MoveType checkMove(sf::Vector2i &start, sf::Vector2i &finish);
	// Cette méthode retourne tous les mouvements possibles pour un joueur donné
	std::vector<Move> *getAvailibleMoves(OwningPlayer player);
	// Cette méthode retourne tous les mouvements possibles pour un pion spécifique appartenant au joueur donné.
	std::vector<Move> *getAvailibleMoves(OwningPlayer player, const std::shared_ptr<Pawn> pawn);
	// Calcule et retourne le score pour le joueur spécifié.
	int getScore(OwningPlayer player);
	// Vérifie et retourne le statut de victoire pour le joueur spécifié.
	OwningPlayer checkWin(OwningPlayer player);
	int evaluate();
};
#endif