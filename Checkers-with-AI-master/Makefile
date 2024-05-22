CXX = g++

CPPFLAGS = -Wall -pedantic -std=c++17 -I ./inc
SFMLFLAGS = -lsfml-graphics -lsfml-window -lsfml-system
OBJECTS = main.o game.o pawn.o board.o

graf: $(OBJECTS)
	$(CXX) $(OBJECTS) -o checkers $(SFMLFLAGS) $(CPPFLAGS) 

main.o: inc/game.hpp
game.o: inc/game.hpp inc/pawn.hpp inc/board.hpp
pawn.o: inc/pawn.hpp
board.o: inc/board.hpp inc/pawn.hpp

.PHONY: clean
clean:
	rm -f $(OBJECTS) checkers

%.o : ./src/%.cpp
	$(CXX) -c $(CPPFLAGS) $<