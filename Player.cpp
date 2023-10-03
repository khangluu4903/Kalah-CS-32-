#include "Player.h"
#include <string>
#include <cstdlib>  
#include <ctime>
#include <iostream>
using namespace std;

Player::Player(string name): m_name(name) {}
string Player::name() const { return m_name; }
Player::~Player() {}

HumanPlayer::HumanPlayer(string name): Player(name) {}
int HumanPlayer::chooseMove(const Board& b, Side s) const {
	std::cout << "Your move: ";
	int playerMove = 0;
	do {
		cin >> playerMove;
		if (playerMove > b.holes()) {
			std::cout << "Your move of " << playerMove << " was invalid. Please choose again: ";
		}
		if (b.beans(s, playerMove) == 0) {
			std::cout << "There are no beans at hole " << playerMove << ". Please choose again: ";
		}
	} while (playerMove > b.holes() || b.beans(s, playerMove) == 0);
	return playerMove;
}


BadPlayer::BadPlayer(std::string name) : Player(name) {}
int BadPlayer::chooseMove(const Board& b, Side s) const {
	srand(time(0));
	int randomHole = 0;
	do {
		randomHole = (rand() % 10) + 1;
		if (randomHole != 0 && randomHole <= b.holes() && b.beans(s,randomHole) != 0) {
			break;
		}
	} while (randomHole == 0 || randomHole > b.holes() || b.beans(s,randomHole) == 0);
	return randomHole;
}

SmartPlayer::SmartPlayer(std::string name) : Player(name) {}

int SmartPlayer::evaluate(const Board& b, Side s) const {
	if (b.beans(s, 0) == 0 && b.beans(opponent(s), 0) == 0) { return 0; }
	if (b.beansInPlay(s) == 0 && b.beansInPlay(opponent(s)) == 0) {			// when the game is over
		if (b.beans(s,0) == b.beans(opponent(s),0)) { return 0; };						// game tied
		// compared whichever side has more beans in pot <=> that's the winner
		if (b.beans(s, 0) > b.beans(opponent(s), 0)) {
			return inf;
		}
		else {
			return -inf;
		}
	}
	// if one side is empty and the opponent has one bean left in play, then the game is basically over, and copare the beans that each side 
	// possesses to determine the winner. 
	if ((b.beansInPlay(s) == 0 && b.beansInPlay(opponent(s)) == 1) || (b.beansInPlay(s) == 1 && b.beansInPlay(opponent(s)) == 0)) {
		int OurBeans = (b.beans(s, 0) == 0 + b.beans(s, 1));
		int OppoBeans = (b.beans(opponent(s), 0) == 0 + b.beans(opponent(s), 1));
		if (OurBeans > OppoBeans) {
			return inf;
		}
		else {
			return -inf;
		}
	}
	// If a side has way more beans in its pot than beans in play on both sides and the opponent's pots, then they win. 
	if (b.beans(s, 0) > (b.beans(opponent(s),0) + b.beansInPlay(s) + b.beansInPlay(opponent(s)))) { return inf; }
	if ((b.beans(opponent(s), 0) > (b.beans(s,0)) + b.beansInPlay(s) + b.beansInPlay(opponent(s)))) { return -inf; }
	// else, then simply compare whichever sides have more beans in their pot.
	if (b.beansInPlay(s) >= 1 && b.beansInPlay(opponent(s)) >= 1) {
		if (b.beans(s,0) == b.beans(opponent(s), 0)) {
			return (b.beans(s, 0) + b.beansInPlay(s)) - (b.beans(opponent(s), 0) + b.beansInPlay(opponent(s)));
			// If both sides have equal amount of beans, then I used the number of beans on their sides as the tie-breaker
		}
		else {
			return b.beans(s, 0) - b.beans(opponent(s), 0);
		}
	}
	return 0;
}

/* only return true when another turn is granted for a side.*/
bool SmartPlayer::smartSow(Board& b, Side s, int hole) const {
	Side endside;
	int endhole = 0;
	bool didshow = b.sow(s,hole, endside, endhole);
	if (!didshow) {
		return false;	// board didn't sow
	}
	if (s == endside && b.beans(s, endside) == 1 && b.beans(opponent(s), endhole) != 0) {
		b.moveToPot(s, endhole, s);
		b.moveToPot(opponent(s), endhole, s);
		return false;	// beans captured and turn ended
	}		
	if (endside == s && endhole == 0) {
		return true;	// last beans ended in their own pot so they can go again
	}
	return false;
} 

int SmartPlayer::smartMove(const Board& b, Side currSide, int depth) const {
	int bestValue = 0;
	// Recursion base case: If the game is over, if the depth is execeeded, or if the time is over, then evaluate leaf nodes
	if (b.beansInPlay(currSide) == 0 || b.beansInPlay(opponent(currSide)) == 0 || depth > 3) {
		int value = evaluate(b, currSide);
		return value;
	}
	bestValue = inf;
	int newValue;
	for (int h = 1; h <= b.holes(); h++) {
		if (b.beans(currSide, h) == 0) {
			continue;
		}
		Board copy(b);
		bool MoveNotOver = smartSow(copy, currSide, h);
		if (MoveNotOver) {
			// If the player can make another turn, then recursion is called on a copied board
			newValue = smartMove(copy, currSide, depth + 1);
		}
		else {
			// If the turn is over, then recursion is called on a copied board for the opponent 
			newValue = smartMove(copy, opponent(currSide), depth + 1);
		}
		/* Basically, my opponents will always try to minimize my winning odds, and I will also try to minimize
		their winning odds, so I would evaluate each leaf nodes in the perspective of the current player, and then when I go up
		a depth to their opponent, I would only pick the smallest values */
		if (bestValue >= newValue) {
			bestValue = newValue;
		}
	}
	return bestValue;
}

int SmartPlayer::chooseMove(const Board& b, Side s) const { 
	if (b.beansInPlay(s) == 0) {
		// return -1 here so that my move can recognize that the player can't make a move
		return -1;
	}
	if (b.beansInPlay(opponent(s)) == 0) {
		// If the game is not over, but the opponents have no more beans in play, I should 
		// sow the pot with the smallest number of beans, prevent it to jump to their side. I will then win.
		int minBeans = inf;
		int bestHole = -1;
		for (int i = 1; i <= b.holes(); i++) {
			if (b.beans(s,i) != 0 && b.beans(s, i) < minBeans) {
				minBeans = b.beans(s, i);
				bestHole = i;
			}
		}
		return bestHole;
	}
	int depth = 0;
	int BestHole = 0;
	int bestValue = -inf;
	int tempValue = 0;
	// For each hole the player can sow, create a copy of the board and sow, then call recursion
	for (int h = 1; h <= b.holes(); h++) {
		if (b.beans(s,h) == 0) { 
			continue; 
		}
		Board copy(b);
		bool moveNotOver = smartSow(copy, s, h);
		if (moveNotOver) {	// if player has another turn.
			tempValue = smartMove(copy, s, depth + 1);
		}
		else {
			tempValue = smartMove(copy, opponent(s), depth + 1);
		}
		/* Though smartMove has been choosing the worse outcome going up the trees, now that the current player
		will choose for the last time, the player would pick the best value, one that maximizes their odd. This odd 
		has been minimized by their opponent but not that it is their actual turn, They can ensure success by picking the best outcome possible. */
		if (tempValue >= bestValue) {
			bestValue = tempValue;
			BestHole = h;
		}
	}
	cout << name() << " picks hole " << BestHole << endl;
	return BestHole;
}


