/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#ifndef BACKGAMMONSTATE_H
#define BACKGAMMONSTATE_H

#define NUM_MOVES_DOUBLES 4

#define NUM_CHECKERS 15
#define NUM_POSITIONS 26

#define POSITION_BEARINGOFF -1
#define POSITION_WHITE_HOMEBOARD 6
#define POSITION_BLACK_HOMEBOARD 19

#define BLACK_SIDE 25
#define WHITE_SIDE 0

#define PLAYER_WHITE PLAYER_PLAYER1
#define PLAYER_BLACK PLAYER_PLAYER2

class Dice
{
public:
	int high;
	int low;

	bool isDouble() { return high == low; }
};

class BackgammonMove
{
public:
	int dest;
	int src;
};

class BackgammonAction : public Action
{
public:
	BackgammonAction()
	{
		dice.high = 0;
		dice.low = 0;
		numMoves = 0;
		probability = 1.0f;
	}

	virtual void print()
	{
		if( dice.high != 0 || dice.low != 0 )
		{
			printf( "Dice (%d, %d)\n", dice.high, dice.low );
			return;
		}

		if( numMoves == 0 )
		{
			printf( "<NO MOVE>\n" );
			return;
		}

		for( int i = 0; i < numMoves; i++ )
			printf( "Move %d -> %d ", moves[i].src, moves[i].dest );

		putchar('\n');
	}
	
	virtual Action* clone()
	{
		BackgammonAction* clonedAction = new BackgammonAction();
		*clonedAction = *this;

		return clonedAction;
	}

	// If either of these is zero, it is a player move and not a dice move.
	Dice dice;

	// Move a checker from position to position.
	BackgammonMove moves[NUM_MOVES_DOUBLES];

	// The number of moves performed in this action. 
	// Zero moves is only valid when the player is completely blocked.
	int numMoves;
};

class BackgammonState : public HeuristicState
{
public:
	BackgammonState();
	
	virtual void print();

	virtual void computeActions( std::vector<Action*>* actions );
	virtual void transitionModel( Action* action );
	
	virtual Action* getProbabilisticAction();

	virtual bool isTerminal();
	virtual std::vector<float> utility();
	virtual std::vector<float> heuristicValue();

	virtual State* clone();
	virtual void copy( State* state );

	Dice throwDices();

private:
	int winner;
	int actiontaker;

	// Each element represents a position on the board. Starting at white's home board.
	// Negative numbers refer to black checkers. Positive numbers to white checkers.
	int board[NUM_POSITIONS];

	// Distinct positions of the white checkers. Sorted in descending order.
	int whiteCheckers[NUM_CHECKERS];
	int numWhitePositions;
	bool whiteBearOff;

	// Distinct positions of the black checkers. Sorted in ascending order.
	int blackCheckers[NUM_CHECKERS];
	int numBlackPositions; 
	bool blackBearOff;

	// Values representing the dice.
	Dice dice;
};

#endif