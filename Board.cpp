#include "Board.h"
using namespace std;

Board::Board(int nHoles, int nInitialBeansPerHole)
//Construct a Board with the indicated number of holes per side(not counting the pot) 
//and initial number of beans per hole.If nHoles is not positive, act as if it were 1; 
//if nInitialBeansPerHole is negative, act as if it were 0.
{
	if (nHoles <= 0) //not positive
		nHoles = 1;
	if (nInitialBeansPerHole < 0) //negative
		nInitialBeansPerHole = 0;
	m_nHoles = nHoles;
	m_nInitialBeansPerHole = nInitialBeansPerHole;

	//LinkedList Creation
	nPot = new Node; //creating north pot
	nPot->side = NORTH;
	nPot->val = 0;
	nPot->index = POT;

	Node * p = nPot;
	for (int i = 0; i < nHoles; i++)
	{
		p->next = new Node; //creating south side bean pots
		p = p->next;
		p->index = i + 1;
		p->val = nInitialBeansPerHole;
		p->side = SOUTH;
	}

	p->next = new Node;
	sPot = p->next; //creating south pot
	sPot->side = SOUTH;
	sPot->val = 0;
	sPot->index = POT;

	p = sPot;
	for (int i = 0; i < nHoles; i++)
	{
		p->next = new Node; //creating north side bean pots
		p = p->next;
		p->index = nHoles - i;
		p->val = nInitialBeansPerHole;
		p->side = NORTH;
	}
	p->next = nPot; //circular linkedbeans complete
}

//destructor
Board::~Board()
{
	Node * p = nPot;
	Node * q;
	for (int i = 0; i < (m_nHoles + 1) * 2 - 1; i++) //every node except one
	{
		q = p->next;
		delete p;
		p = q;
	}
	delete p; //delete last one
}

//copy constructor (from assignment operator)
Board::Board(const Board & b)
{
	*this = b;
}

//assignment operator
Board & Board::operator=(const Board & b)
{
	if (&b == this) //necessary statement if both are the same
		return *this;

	//destruct current data
	Node * p = nPot;
	Node * q;
	if (p != nullptr) //ensures that we aren't destroying vacant space
	{
		for (int i = 0; i < (m_nHoles + 1) * 2 - 1; i++) //every node except one
		{
			q = p->next;
			delete p;
			p = q;
		}
		delete p; //delete last one
	}

	//re-add new data using similar code to original constructor
	m_nHoles = b.m_nHoles;
	m_nInitialBeansPerHole = b.m_nInitialBeansPerHole;

	//LinkedList Creation (copying values from b)
	nPot = new Node; //creating north pot
	nPot->side = NORTH;
	nPot->val = b.nPot->val;
	nPot->index = POT;

	p = nPot;
	q = b.nPot; //creates comparison traversal pointer
	for (int i = 0; i < m_nHoles; i++)
	{
		p->next = new Node; //creating south side bean pots
		p = p->next; 
		q = q->next; 
		p->index = i + 1;
		p->val = q->val;
		p->side = SOUTH;
	}

	p->next = new Node;
	sPot = p->next; //creating south pot
	sPot->side = SOUTH;
	sPot->val = b.sPot->val;
	sPot->index = POT;

	p = sPot;
	q = b.sPot;
	for (int i = 0; i < m_nHoles; i++)
	{
		p->next = new Node; //creating north side bean pots
		p = p->next;
		q = q->next;
		p->index = m_nHoles - i;
		p->val = q->val;
		p->side = NORTH;
	}
	p->next = nPot; //copied circular linkedbeans complete

	//return new board
	return *this;
}

int Board::holes() const
//Return the number of holes on a side(not counting the pot).
{
	return m_nHoles;
}

int Board::beans(Side s, int hole) const
//Return the number of beans in the indicated hole or pot, or −1 if the hole number is 
//invalid.
{
	if (s != SOUTH && s != NORTH) //checks side
		return -1;
	if (hole > m_nHoles || hole < 0) //invalid hole
		return -1;
	Node * p = getNode(s, hole); //uses defined getNode function to return the value
	return p->val;
}

int Board::beansInPlay(Side s) const
//Return the total number of beans in all the holes on the indicated side, not counting 
//the beans in the pot.
{
	int count = 0;
	for (int i = 1; i <= m_nHoles; i++)
	{
		Node * p = getNode(s, i); 
		count += p->val;
	}
	return count;
}

int Board::totalBeans() const
//Return the total number of beans in the game, including any in the pots.
{
	int sum = 0;
	Node * p = getNode(SOUTH, POT);
	for (int i = 0; i < 2 * m_nHoles + 2; i++) //iterates through entire linkedbeans, adding up values
	{
		sum += p->val;
		p = p->next;
	}
	return sum;
	
}

bool Board::sow(Side s, int hole, Side& endSide, int& endHole)
//If the hole indicated by(s, hole) is empty or invalid or a pot, this function returns
//false without changing anything.Otherwise, it will return true after sowing the beans :
//the beans are removed from hole(s, hole) and sown counterclockwise, including s's pot 
//if encountered, but skipping s's opponent's pot. The parameters endSide and endHole are 
//set to the side and hole where the last bean was placed. (This function does not make 
//captures or multiple turns; different Kalah variants have different rules about these 
//issues, so dealing with them should not be the responsibility of the Board class.)
{
	Node * p = getNode(s, hole);
	if (p->val == 0 || hole == POT)
		return false;
	if (hole > m_nHoles || hole < 1)
		return false;
	int toSow = p->val;
	Side opp = opponent(s); //store opponent's side
	p->val = 0; //set the value to zero

	for (int i = 0; i < toSow; i++)
	{
		p = p->next;
		if (p->side == opp && p->index == POT) //skips the opponent's pot
			p = p->next;
		p->val++;
	}
	endSide = p->side;
	endHole = p->index;
	return true;
}

bool Board::moveToPot(Side s, int hole, Side potOwner)
//If the indicated hole is invalid or a pot, return false without changing anything. 
//Otherwise, move all the beans in hole(s, hole) into the pot belonging to potOwner and
//return true.
{
	if (hole == POT || hole > m_nHoles || hole < 0)
		return false;
	Node * p = getNode(s, hole);
	Node * owner = getNode(potOwner, POT);
	owner->val += p->val; //increments the owner's pot by the value in the current hole
	p->val = 0; //resets value of p to zero
	return true;
}

bool Board::setBeans(Side s, int hole, int beans)
//If the indicated hole is invalid or beans is negative, this function returns false 
//without changing anything.Otherwise, it will return true after setting the number of
//beans in the indicated hole or pot to the value of the third parameter. (This may change
//what beansInPlay and totalBeans return if they are called later.) This function exists 
//solely so that we and you can more easily test your program : None of your code that 
//implements the member functions of any class is allowed to call this function directly
//or indirectly. (We'll show an example of its use below.)
{
	if (hole > m_nHoles || hole < 0 || beans < 0)
		return false;
	Node * q = getNode(s, hole);
	q->val = beans;
	return true;
}

//PRIVATE FXN - gets the desired node
Board::Node *Board::getNode(Side s, int index) const
{
	if (s == SOUTH && index == POT) //south pot
		return sPot;
	else if (s == SOUTH) //south side
	{
		int target = index; //the node we want
		Node * p = nPot; //start at nPot
		for (int i = 0; i < target; i++)
		{
			p = p->next;
		}
		return p;
	}

	if (s == NORTH && index == POT) //north pot
		return nPot;
	else if (s == NORTH)
	{
		int target = m_nHoles + 1 - index; //the node we want
		Node * p = sPot; //start at sPot
		for (int i = 0; i < target; i++)
		{
			p = p->next;
		}
		return p;
	}
	return nullptr;
}