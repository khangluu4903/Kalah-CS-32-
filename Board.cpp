#include "Board.h"

Board::Board(int nHoles, int nInitialBeansPerHole){
	if (nHoles < 0) {
		// if hole input is invalid, I set hole to 1
		m_holes = 1;
		m_totalBeans = 2 * nInitialBeansPerHole;
		N.push_back(0);
		N.push_back(nInitialBeansPerHole);
		S.push_back(0);
		S.push_back(nInitialBeansPerHole);
	}
	else if (nInitialBeansPerHole < 0) {
		// If the number of beans per hole is negative, I set it to 0
		m_holes = nHoles;
		m_totalBeans = 0;
		for (int i = 0; i <= m_holes; i++) {
			N.push_back(0);
			S.push_back(0);
		}
	}
	else {
		// If the holes and beans inputs are correct, I initialize the pot and then push in 
		// the number of beans at each hole.
		m_holes = nHoles;
		m_totalBeans = 2 * (nHoles * nInitialBeansPerHole);
		N.push_back(0);
		S.push_back(0);
		for (int i = 1; i != m_holes + 1; i++) {
			N.push_back(nInitialBeansPerHole);
			S.push_back(nInitialBeansPerHole);
		}
	}

}
int Board::holes() const { return m_holes; }

int Board::beans(Side s, int hole) const {
	if (hole <= m_holes) {
		if (s == 0) {
			return N[hole];
		}
		if (s == 1) {
			if (hole == 0) {
				return S[0];
			}
			else {
				return S[S.size() - hole];
			}
		}
	}
	return -1;
}
int Board::beansInPlay(Side s) const {
	int Total = 0;
	if (s == 0) {
		for (int i = 1; i <= m_holes; i++) {
			Total += N[i];
		}
	}
	if (s == 1) {
		for (int j = 1; j <= m_holes; j++) {
			Total += S[j];
		}
	}
	return Total;
}

int Board::totalBeans() const { 
	int Total = 0;
	for (int i = 0; i <= m_holes; i++) {
		Total += N[i];
	}
	for (int j = 0; j <= m_holes; j++) {
		Total += S[j];
	}
	return Total;
}

bool Board::sow(Side s, int hole, Side& endSide, int& endHole) { 
	// check that game isn't trying to sow a pot
	if (hole > m_holes || hole == 0) { return false; }
	if (s == 0 && N[hole] == 0) { return false; }
	if (s == 1 && S[S.size() - hole] == 0) { return false; }
	if (s == 0) {			// if side is NORTH
		int temp = N[hole];
		Side lastSide;
		int lastHole = 0;
		N[hole] = 0;
		while (temp != 0) {
			for (int i = hole - 1; i != -1; i--) {
				// sow counter-clockwise, reducing the amount of beans stored in temp by 1 at each hole and N's pot
				if (temp == 0) {
					break;
				}
				N[i]++;
				lastSide = NORTH;
				lastHole = i;
				temp--;
			}
			while (temp != 0) {
				// Now that the sowing finished on the N's side, I sowed on the South, skipping the pot, and sow the North again if there are leftover beans
				for (int j = S.size() - 1; j != 0; j--) {
					if (temp == 0) {
						break;
					}
					S[j]++;
					lastSide = SOUTH;
					lastHole = j;
					temp--;
				}
				if (temp == 0) {
					break;
				}
				for (int j = N.size() - 1; j >= 0; j--) {
					if (temp == 0) {
						break;
					}
					N[j]++;
					lastSide = NORTH;
					lastHole = j;
					temp--;
				}
			}
		}
		endSide = lastSide;
		if (lastSide == 0) {
			if (lastHole == 0) {
				endHole = 0;
			}
			else {
				endHole = lastHole;			
			}
		}
		if (lastSide == 1) {
			if (lastHole == 0) {
				endHole = 0;
			}
			else {
				endHole = S.size() - lastHole;
			}
		}
		return true; 
	}
	if (s == 1) {		// While side is South
		int temp = S[S.size() - hole];
		Side lastSide;
		int lastHole = 0;
		S[S.size() - hole] = 0;
		while (temp != 0) {
			// I start sowing the hole to the right and drop 1 beans, including the pot
			for (int i = S.size() - hole - 1; i >= 0; i--) {
				if (temp == 0) {
					break;
				}
				S[i]++;
				lastSide = SOUTH;
				lastHole = i;
				temp--;
			}
			while (temp != 0) {
				for (int j = N.size() - 1; j != 0; j--) {
					// Now that the sowing finished on the S's side, I sowed on the NORTH, skipping the pot, and sow the South again if there are leftover beans
					if (temp == 0) {
						break;
					}
					N[j]++;
					lastSide = NORTH;
					lastHole = j;
					temp--;
				}
				if (temp == 0) {
					break;
				}
				for (int j = S.size() - 1; j >= 0; j--) {
					if (temp == 0) {
						break;
					}
					S[j]++;
					lastSide = SOUTH;
					lastHole = j;
					temp--;
				}

			}
		}
		endSide = lastSide;
		if (lastSide == 0) {
			if (lastHole == 0) {
				endHole = 0;
			}
			else {
				endHole = lastHole;
			}
		}
		if (lastSide == 1) {
			if (lastHole == 0) {
				endHole = 0;
			}
			else {
				endHole = S.size() - lastHole;
			}
		}
		return true; 
	}
	return false;
}

bool Board::moveToPot(Side s, int hole, Side potOwner) { 
	// if statements located desired Side and potOwner for relocation of beans
	if (hole == 0) {
		return false;
	}
	if (s == 0 && potOwner == 0) {
		N[0] = N[0] + N[hole];
		N[hole] = 0;
		return true; 
	}
	if (s == 0 && potOwner == 1) {
		S[0] = S[0] + N[hole];
		N[hole] = 0;
		return true;
	}
	if (s == 1 && potOwner == 1) {
		S[0] = S[0] + S[S.size() - hole];
		S[S.size() - hole] = 0;
		return true;
	}
	if (s == 1 && potOwner == 0) {
		N[0] = N[0] + S[S.size() - hole];
		S[S.size() - hole] = 0;
		return true;
	}
	return false;
}

bool Board::setBeans(Side s, int hole, int beans) {
	if (s ==  NORTH && beans >= 0 && hole <= m_holes) {
		N[hole] = beans;
		return true;
	}
	if (s == SOUTH && beans >= 0 && hole <= m_holes) {
		if (hole == 0) {
			S[0] = beans;
		}
		else {
			S[S.size() - hole] = beans;
		}
		return true;
	}
	return false;
}
