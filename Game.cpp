#include <iostream>
#include <iomanip>
#include "Game.h"
using namespace std;

Game::Game(const Board& b, Player* south, Player* north) : m_board(b){
	Nplayer = north;
	Splayer = south;
}

void Game::display() const {
	string space = "";
	for (int i = 0; i != m_board.holes(); i++) {
		space += " ";
	}
	cout << space << Nplayer->name() << endl;
	cout << "  "; 
	for (int i = 1; i <= m_board.holes(); i++) {
		cout << m_board.beans(NORTH, i) << " ";
	}
	cout << endl;
	cout << m_board.beans(NORTH, 0) << space << space << " " << m_board.beans(SOUTH, 0) << endl;
	cout << "  ";
	for (int j = 1; j <= m_board.holes(); j++) {
		cout << m_board.beans(SOUTH, j) << " ";
	}
	cout << endl;
	cout << space << Splayer->name() << endl;
	cout << endl;
}

void Game::FinishingTheGame(Side s) {
	// function moved opponent of S beans into their pot
	if (s == NORTH) {
		for (int i = m_board.holes(); i != 0; i--) {
			m_board.moveToPot(SOUTH, i, SOUTH);
		}
	}
	if (s == SOUTH) {
		for (int i = m_board.holes(); i != 0; i--) {
			m_board.moveToPot(NORTH, i, NORTH);
		}
	}
}

bool Game::move(Side s) { 
	if (s == 0) {	// North Player
		if (m_board.beansInPlay(s) != 0) {
			int tempEndHole = 0;
			Side tempEndSide = NORTH;
			int holechoice = Nplayer->chooseMove(m_board, s);			// chose a hole
			if (holechoice == -1) {
				// if a SmartPlayer return -1, they can't make a move so the game is over
				FinishingTheGame(NORTH);
				return false;
			}
			m_board.sow(s, holechoice, tempEndSide, tempEndHole);		// sow that hole
			while (tempEndHole == 0 && tempEndSide == s) {
				// if the last bean land in the player's pot
				if (m_board.beansInPlay(NORTH) == 0) {
					// and the player can't make another move, then game over
					FinishingTheGame(NORTH);
					return false;
				}
				if (tempEndHole != 0 && tempEndSide == s) {
					break;
				}
				display();
				// the player can sow again
				holechoice = Nplayer->chooseMove(m_board, s);
				m_board.sow(s, holechoice, tempEndSide, tempEndHole);
			}
			// capturing from the South's side
			if (s == tempEndSide && tempEndSide == 0 && m_board.beans(s, tempEndHole) == 1 && m_board.beans(SOUTH, tempEndHole) != 0) {
				m_board.moveToPot(NORTH, tempEndHole, NORTH);
				m_board.moveToPot(SOUTH, tempEndHole, NORTH);
			}
			return true;
		}
		// if the player can't make a game, then its game over and sweep the South's beans into their pot
		else {
			FinishingTheGame(NORTH);
			return false;
		}
	}
	if (s == 1) {	// South Player
		if (m_board.beansInPlay(s) != 0) {
			int tempEndHole = 0;
			Side tempEndSide = SOUTH;
			int holechoice = Splayer->chooseMove(m_board, s);			// chose a hole
			if (holechoice == -1) {
				// if a SmartPlayer return -1, they can't make a move so the game is over
				FinishingTheGame(SOUTH);
				return false;
			}
			m_board.sow(s, holechoice, tempEndSide, tempEndHole);		// sow that hole
			while (tempEndHole == 0 && tempEndSide == s) {
				// if the last bean landed in the South's pot
				if (m_board.beansInPlay(SOUTH) == 0) {
					// and the player can't make another move, then game over
					FinishingTheGame(SOUTH);
					return false;
				}
				if (tempEndHole != 0 && tempEndSide == s) {
					break;
				}
				display();
				// the player can sow again
				holechoice = Splayer->chooseMove(m_board, s);
				m_board.sow(s, holechoice, tempEndSide, tempEndHole);
			}
			if (s == tempEndSide && tempEndSide == 1 && m_board.beans(s, tempEndHole) == 1 && m_board.beans(NORTH, tempEndHole) != 0) {
				m_board.moveToPot(SOUTH, tempEndHole, SOUTH);
				m_board.moveToPot(NORTH, tempEndHole, SOUTH);
			}
			return true;
		}
		else {
			FinishingTheGame(SOUTH);
			return false;
		}
	}
	return false;
}

bool Game::GameOver() const {
	if (m_board.beansInPlay(NORTH) > 0 && m_board.beansInPlay(SOUTH) > 0) {
		// game over when there are beans lef to play on both sides
		return false;
	}
	return true;
}

void Game::status(bool& over, bool& hasWinner, Side& winner) const {
	if (GameOver()) {
		over = true;
		if (m_board.beans(NORTH, 0) == m_board.beans(SOUTH, 0)) {
			hasWinner = false;
		}
		else {
			if (m_board.beans(NORTH, 0) > m_board.beans(SOUTH, 0)) {
				hasWinner = true;
				winner = NORTH;
			}
			if (m_board.beans(NORTH, 0) < m_board.beans(SOUTH, 0)) {
				hasWinner = true;
				winner = SOUTH;
			}
		}
	}
	else {
		over = false;
	}
}

void Game::play() {
	bool gameOver = false;
	bool hasWinner;
	Side Winner = NORTH;
	display();
	if (!(Splayer->isInteractive()) && !(Nplayer->isInteractive())) {
		cout << "Press Enter to continue: ";
		cin.ignore();
		cout << endl;
	}
	while (gameOver != true) {
		// game over is checked when a player can't make a move
		if (!move(SOUTH)) {
			gameOver = GameOver();
			status(gameOver, hasWinner, Winner);
			break;
		}
		display();
		if (!move(NORTH)) {
			gameOver = GameOver();
			status(gameOver, hasWinner, Winner);
			break;
		}
		display();
		if (!(Splayer->isInteractive()) && !(Nplayer->isInteractive())) {
			cout << "Press Enter to continue: ";
			cin.ignore();
			cout << endl;
		}
	}
	display();
	if (hasWinner == true) {
		if (Winner == NORTH) {
			cout << "The winner is " << Nplayer->name() << "!" << endl;
		}
		else {
			cout << "The winner is " << Splayer->name() << "!" << endl;
		}
	}
	if (hasWinner == false) {
		cout << "The game is tied!" << endl;
	}
}

int Game::beans(Side s, int hole) const { 
	return m_board.beans(s, hole);
} 