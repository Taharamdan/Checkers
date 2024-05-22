#ifndef PAWN_HPP
#define PAWN_HPP

#include <SFML/Graphics.hpp>

enum OwningPlayer
{
	NOBODY,
	HUMAN,
	COMPUTER,
};

OwningPlayer otherPlayer(OwningPlayer current_player);

enum PawnLevel
{
	normal,
	king,
};

class Pawn
{
public:
	Pawn(int, int, float, float, OwningPlayer);
	OwningPlayer owner;
	PawnLevel level = normal;
	float x; // pixel
	float y;
	sf::Vector2i coordinates;
};

#endif