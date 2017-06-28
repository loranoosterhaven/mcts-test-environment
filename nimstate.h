/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#ifndef NIMSTATE_H
#define NIMSTATE_H

class NimAction : public Action
{
public:
	NimAction() { probability = 1.0f; }
	
	virtual void print()
	{
		printf( "Take %d chips\n", chips );
	}
	
	virtual Action* clone()
	{
		return new NimAction( *this );
	}

	int chips;
};

class NimState : public State
{
public:
	NimState( int chips, int maxTakeChips ) 
		: chips( chips ), maxTakeChips( maxTakeChips ) { numPlayers = 2; turn = PLAYER_PLAYER1; };
	
	virtual void print();

	virtual void computeActions( std::vector<Action*>* actions );
	virtual void transitionModel( Action* action );

	virtual bool isTerminal();
	virtual std::vector<float> utility();

	virtual State* clone();
	virtual void copy( State* state );
	int getOptimalChips() { return chips % ( maxTakeChips + 1 ); }
	virtual int getAverageBranchingFactor() { return maxTakeChips; }
		
	virtual int getIntegerHash();

	int getChips() { return chips; }

private:
	int chips;
	int maxTakeChips;
};

#endif