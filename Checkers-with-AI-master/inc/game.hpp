#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

#include "pawn.hpp"
#include "board.hpp"

const int plus_infty = 10000;
const int minus_infty = -10000;

enum Difficulty
{
	EASY,
	MEDIUM,
	HARD
};

class Game
{
public:
	Game(Difficulty level);
	void start();
	void play();
	void view();
	int getMove(OwningPlayer player);
	int manualMove(OwningPlayer player);
	int computerMove();
	void executeMove(sf::Vector2i &start, sf::Vector2i &finish, MoveType type);
	bool pollEvents(sf::Vector2i &mouse_position);
	int alphabeta(Board &, Move &, int depth, OwningPlayer, int alpha, int beta);

	sf::RenderWindow window;
	sf::Texture textures[5];
	sf::Sprite sprites[5];
	sf::Image icon;

	OwningPlayer players[2] = {HUMAN, COMPUTER};
	Board game_board;
	OwningPlayer active_player = HUMAN;

private:
	Difficulty difficulty;
	int getSearchDepth() const;
};
#endif