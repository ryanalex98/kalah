#include "Game.h"
#include <iostream>
using namespace std; //TODO

Game::Game(const Board& b, Player* south, Player* north)
//Construct a Game to be played with the indicated players on a copy of the board b.
//The player on the south side always moves first.
	: m_board(b), m_south(south), m_north(north)
{
	sCur = SOUTH; //the current player's turn
	m_over = false;
}

void Game::display() const
//Display the game's board in a manner of your choosing, provided you show the names 
//of the players and a reasonable representation of the state of the board.
{
	//this just took a lot of trial and error
	cout << "========";
	for (int i = 0; i < m_board.holes(); i++)
		cout << "========";
	cout << "==" << endl << "NORTH:";
	for (int i = 0; i < m_board.holes(); i++)
		cout << "\t";
	cout << "SOUTH:" << endl;
	cout << m_north->name();
	for (int i = 0; i < m_board.holes(); i++)
		cout << "\t";
	cout << m_south->name() << endl << endl << "\t";
	for (int i = 1; i <= m_board.holes(); i++)
		cout << i << "\t";
	cout << endl << "NORTH" << "-- ";
	for (int i = 0; i < m_board.holes(); i++)
		cout << "=" << "\t";
	cout << endl <<"|" << "\t";
	for (int i = 1; i <= m_board.holes(); i++)
		cout << m_board.beans(NORTH, i) << "\t";
	cout << endl << m_board.beans(NORTH, POT);
	for (int i = 0; i <= m_board.holes(); i++)
		cout << "\t";
	cout << m_board.beans(SOUTH, 0) << endl << "\t";
	for (int i = 1; i <= m_board.holes(); i++)
		cout << m_board.beans(SOUTH, i) << "\t";
	cout << "|" << endl;
	for (int i = 0; i < m_board.holes(); i++)
		cout << "\t" << "=";
	cout << " ---SOUTH" << endl << "\t";
	for (int i = 1; i <= m_board.holes(); i++)
		cout << i << "\t";
	cout << endl << endl;
}

void Game::status(bool& over, bool& hasWinner, Side& winner) const
//If the game isn't over (i.e., more moves are possible), set over to false and do 
//not change anything else. Otherwise, set over to true and hasWinner to true if the 
//game has a winner, or false if it resulted in a tie. If hasWinner is set to false, 
//leave winner unchanged; otherwise, set it to the winning side.
{
	
	if (!(m_board.beansInPlay(NORTH) == 0 && m_board.beansInPlay(SOUTH) == 0)) //if the game is not over
	{
		over = false;
		return; //end the function
	}
	over = true; // the game is over
	if (m_board.beans(NORTH, POT) > m_board.beans(SOUTH, POT)) //if the north pot has more than the south pot
	{
		hasWinner = true;
		winner = NORTH;
	}
	else if (m_board.beans(SOUTH, POT) > m_board.beans(NORTH, POT)) //vice versa
	{
		hasWinner = true;
		winner = SOUTH;
	}
	else //a tie
		hasWinner = false;

}

bool Game::move()
//If the game is over, return false.Otherwise, make a complete move for the player 
//whose turn it is(so that it becomes the other player's turn) and return true. 
//"Complete" means that the player sows the seeds from a hole and takes any additional 
//turns required or completes a capture. If the player gets an additional turn, you 
//should display the board so someone looking at the screen can follow what's happening.
{
	//keeps track of the opponent
	Side opp = opponent(sCur); 

	//current player
	Player * pCur = getPlayer(sCur);
	
	//SWEEP CHECK
	if (m_board.beansInPlay(SOUTH) == 0 || m_board.beansInPlay(NORTH) == 0)
	{
		for (int i = 1; i <= m_board.holes(); i++)
		{
			m_board.moveToPot(NORTH, i, NORTH);
			m_board.moveToPot(SOUTH, i, SOUTH);
		}
		status(m_over, m_hasWinner, m_winner);
		return false;
	}

	//bool checks if turn is over
	bool turnOver = false;

	//while !turnOver
	while (!turnOver)
	{
		//sweep if needed
		if (m_board.beansInPlay(SOUTH) == 0 || m_board.beansInPlay(NORTH) == 0)
		{
			for (int i = 1; i <= m_board.holes(); i++)
			{
				m_board.moveToPot(NORTH, i, NORTH);
				m_board.moveToPot(SOUTH, i, SOUTH);
			}
			status(m_over, m_hasWinner, m_winner); //collects game status
			break; //get out of the loop
		}
		//choose move
		int hole = pCur->chooseMove(m_board, sCur);
		//declare endhole, endside
		int endHole;
		Side endSide;
		//sow
		m_board.sow(sCur, hole, endSide, endHole);
		
		//opponent case
		if (endSide == opp)
			turnOver = true;
			
		else if (endSide == sCur) //ensures the beans ended up on player's side
		{
			//pot case
			if (endHole == POT)
			{
				//as long as current side still can be played, we display the board
				if (m_board.beansInPlay(sCur) > 0)
					display();
			}
			//capture case
			else if (m_board.beans(sCur, endHole) == 1 && m_board.beans(opp, endHole) > 0)
			{
				m_board.moveToPot(opp, endHole, sCur);
				m_board.moveToPot(sCur, endHole, sCur);
				turnOver = true;
			}
			//any other case
			else
				turnOver = true;
		}
	}
	//switch side at end
	sCur = opp; //ensures the next opponent goes
	return true;
}

void Game::play()
//Play the game. Display the progress of the game in a manner of your choosing, provided 
//that someone looking at the screen can follow what's happening. If neither player is 
//interactive, then to keep the display from quickly scrolling through the whole game, it 
//would be reasonable periodically to prompt the viewer to press ENTER to continue and not 
//proceed until ENTER is pressed. (The ignore function for input streams is useful here.) 
//Announce the winner at the end of the game. You can apportion to your liking the 
//responsibility for displaying the board between this function and the move function. 
//(Note: If when this function is called, South has no beans in play, so can't make the 
//first move, sweep any beans on the North side into North's pot and act as if the game 
//is thus over.)
{
	//while it's not over (beans in both pots aren't empty)
	display();
	while (!m_over)
	{

		move(); //move the players
		//SWEEP CHECK
		if (m_board.beansInPlay(SOUTH) == 0 || m_board.beansInPlay(NORTH) == 0)
		{
			for (int i = 1; i <= m_board.holes(); i++)
			{
				m_board.moveToPot(NORTH, i, NORTH);
				m_board.moveToPot(SOUTH, i, SOUTH);
			}
		}
		display();
		status(m_over, m_hasWinner, m_winner); //call status
		Player * pCur = getPlayer(sCur);
		if (!pCur->isInteractive())
		{
			cout << "Press enter to continue." << endl;
			cin.ignore(10000, '\n');
		}
	}

	//exits loop when game is over
	if (!m_hasWinner)
	{
		cout << "The game ends in a draw." << endl;
		return;
	}
	else
	{
		Player * win = getPlayer(m_winner);
		string winSide;
		if (m_winner == NORTH)
			winSide = "NORTH";
		else
			winSide = "SOUTH";
		cout << "The winner is " << win->name() << " of side " << winSide << "." << endl;
		return;
	}
}

int Game::beans(Side s, int hole) const
//Return the number of beans in the indicated hole or pot of the game's board, or −1 if 
//the hole number is invalid. This function exists so that we and you can more easily 
//test your program.
{
	return m_board.beans(s,hole);
}

//PRIVATE FUNCTIONS

Player * Game::getPlayer(Side s) const
{
	if (s == SOUTH)
		return m_south;
	else
		return m_north;
}