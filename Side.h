#include <limits>
#ifndef Side_H
#define Side_H
using namespace std;

enum Side { NORTH, SOUTH };

const int NSIDES = 2;
const int POT = 0;
const int inf = numeric_limits<int>::max();
inline
Side opponent(Side s)
{
	return Side(NSIDES - 1 - s);
}

#endif