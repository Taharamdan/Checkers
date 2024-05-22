#include <iostream>
#include <SFML/Graphics.hpp>
#include "game.hpp"

int main(int argc, char const *argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <difficulty: easy | medium | hard>" << std::endl;
		return 1;
	}

	Difficulty difficulty;
	std::string difficulty_str = argv[1];

	if (difficulty_str == "easy")
	{
		difficulty = EASY;
	}
	else if (difficulty_str == "medium")
	{
		difficulty = MEDIUM;
	}
	else if (difficulty_str == "hard")
	{
		difficulty = HARD;
	}
	else
	{
		std::cerr << "Unknown difficulty: " << argv[1] << ". Use easy, medium, or hard." << std::endl;
		return 1;
	}

	Game my_game(difficulty);
	my_game.start();
	my_game.play();

	return 0;
}
