#include <vector>
#include "Side.h"
#ifndef Board_H
#define Board_H

class Board {
public:
	Board(int nHoles, int nInitialBeansPerHole);
	int holes() const;
	int beans(Side s, int hole) const;
	int beansInPlay(Side s) const;
	int totalBeans() const;
	bool sow(Side s, int hole, Side & endSide, int& endHole);
	bool moveToPot(Side s, int hole, Side potOwner);
	bool setBeans(Side s, int hole, int beans);
private:
	std::vector<int> N;
	std::vector<int> S;
	int m_holes;
	int m_totalBeans;
};


#endif