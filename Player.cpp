#include "Player.h"
#include <iostream>
using namespace std;

// constructors
Player::Player(std::string name)
	:m_name(name)
{}
BadPlayer::BadPlayer(std::string name)
	:Player(name)
{}
SmartPlayer::SmartPlayer(std::string name)
	: Player(name)
{}
HumanPlayer::HumanPlayer(std::string name)
	: Player(name)
{}

std::string Player::name() const
//Return the name of the player.
{
	return m_name;
}

bool Player::isInteractive() const
//Return false if the player is a computer player.Return true if the player is human.
//Most kinds of players will be computer players.
{
	return false;
}

bool HumanPlayer::isInteractive() const
{
	return true;
}

int HumanPlayer::chooseMove(const Board& b, Side s) const
{
	int move = -1;
	if (b.beansInPlay(s) == 0)
		return -1;
	while (move > b.holes() || move < 1 || b.beans(s,move) < 1)
	{
		cout << "Enter the pot you wish to move, " << name() << "." << endl; //TODO -- need to verify i'm allowed to do this
		cin >> move;
		cin.ignore(10000, '\n');
	}
	cout << endl;
	return move;
}

int BadPlayer::chooseMove(const Board& b, Side s) const
{
	if (b.beansInPlay(s) == 0) //returns -1 if none possible
		return -1;
	for (int i = 1; i <= b.holes(); i++) //chooses leftmost hole
	{
		if (b.beans(s, i) > 0)
			return i;
	}
	return -1; //this will never happen; just so control paths return value
}

int SmartPlayer::chooseMove(const Board& b, Side s) const
{
	int MAX_DEPTH = 10;
	int val = -1001;
	int bestHole = -1;
	Board copyBoard = b;

	helperFxn(copyBoard, s, bestHole, val, MAX_DEPTH);
	
	return bestHole; //TODO - 15-20% of grade
}

void SmartPlayer::helperFxn(Board& b, Side s, int& bestHole, int& val, int depth) const
{
	depth--;
	//if no move exists (game is over)
	if (b.beansInPlay(s) == 0 || b.beansInPlay(opponent(s)) == 0)
	{
		bestHole = -1;
		for (int i = 1; i <= b.holes(); i++)
		{
			b.moveToPot(s, i, s);
			b.moveToPot(opponent(s), i, opponent(s));
		}
		//set value as twice the pot difference + the difference in beans on each side
		if (b.beans(s, POT) > b.beans(opponent(s), POT)) //you win
			val = 1000;
		else if (b.beans(s, POT) < b.beans(opponent(s), POT)) //opponent wins
			val = -1000;
		else //draw
			val = 0;
		return;
	}
	//if the criterion says to not search below node (second criteria to no move)
	if (depth <= 0)
	{
		bestHole = -1;
		val = 2 * (b.beans(s, POT) - b.beans(opponent(s), POT)) + b.beansInPlay(s) - b.beansInPlay(opponent(s));
		return;
	}

	//variables to be changed by reference by this for loop
	int v2 = -1001; //comparison value
	int newBest = -1; //new best hole
	//endside and endhole for sow
	Side endS;
	int endH;

	//for every hole h the player can choose
	for (int h = 1; h <= b.holes(); h++)
	{
		if (!(b.beans(s, h) == 0))
		{
			//make move h
			//create a copy of the board
			Board cb(b); //cb stands for copy board
			cb.sow(s, h, endS, endH);
			//NEW MOVE - ends in pot
			if (endS == s && endH == POT)
				helperFxn(cb, s, newBest, v2, depth);
			//not a new move
			else
			{
				//capture
				if (endS == s && cb.beans(opponent(s), endH) > 0 && cb.beans(s, endH) == 1)
				{
					cb.moveToPot(opponent(s), endH, s);
					cb.moveToPot(s, endH, s);
				}
				//switches to opponent's turn
				helperFxn(cb, opponent(s), newBest, v2, depth);
				//need to switch the magnitude of the opponent's move
				v2 = -1 * v2;
			}
			//stores the new value if it's the best
			if (v2 > val)
			{
				val = v2;
				bestHole = h;
			}
		}
	}
}


//destructors
Player::~Player()
{}
HumanPlayer::~HumanPlayer()
{}
BadPlayer::~BadPlayer()
{}
SmartPlayer::~SmartPlayer()
{}