#include <string>
#include "Board.h"
#include "Side.h"
#ifndef Player_H
#define Player_H

class Player {
public:
	Player(std::string name);
	std::string name() const;
	virtual bool isInteractive() const = 0;
	virtual int chooseMove(const Board & b, Side s) const = 0;
	virtual ~Player();
private:
	std::string m_name;
};

class HumanPlayer : public Player {
public:
	HumanPlayer(std::string name);
	virtual bool isInteractive() const { return true; }
	virtual int chooseMove(const Board& b, Side s) const;
	virtual ~HumanPlayer() {}
};

class BadPlayer : public Player {
public:
	BadPlayer(std::string name);
	virtual bool isInteractive() const {return false; }
	virtual int chooseMove(const Board& b, Side s) const;
	virtual ~BadPlayer() {}
};

class SmartPlayer : public Player {
public:
	SmartPlayer(std::string name);
	virtual bool isInteractive() const { return false; }
	virtual int chooseMove(const Board& b, Side s) const;
	virtual ~SmartPlayer() {}
private:
	bool smartSow(Board& b, Side s, int hole) const;
	int smartMove(const Board& b, Side currSide, int depth) const;
	int evaluate(const Board& b, Side s) const;
};

#endif

