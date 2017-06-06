/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#ifndef TICTACTOESTATE_H
#define TICTACTOESTATE_H

class TicTacToeAction : public Action
{
public:
	TicTacToeAction( unsigned int boardIndex ) : 
		boardIndex( boardIndex ) { probability = 1.0f; }
	
	virtual void print()
	{
		printf( "Draw at index %d\n", boardIndex );
	}
	
	virtual Action* clone()
	{
		return new TicTacToeAction( this->boardIndex );
	}

	unsigned int boardIndex;
};

class TicTacToeState : public State
{
public:
	TicTacToeState();
	
	virtual void print();

	virtual void computeActions( std::vector<Action*>* actions );
	virtual void transitionModel( Action* action );

	virtual bool isTerminal();
	virtual std::vector<float> utility();

	virtual State* clone();
	virtual void copy( State* state );

private:
	void computeTerminal();

private:
	int board[9];
	int winner;
	bool terminal;
};

#endif