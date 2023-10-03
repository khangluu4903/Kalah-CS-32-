#include "Board.h"
#include "Player.h"

#ifndef Game_H
#define Game_H

class Game {
public:
	Game(const Board& b, Player* south, Player* north);
	void display() const;
	void status(bool& over, bool& hasWinner, Side& winner) const;
	bool move(Side s);
	void play();
	int beans(Side s, int hole) const;
private:
	Board m_board;
	bool GameOver() const;
	void FinishingTheGame(Side s);
	Player* Nplayer;
	Player* Splayer;
};

#endif