/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#ifndef STATE_H
#define STATE_H

enum Player
{
	PLAYER_NONE = -1,
	PLAYER_PLAYER1 = 0,
	PLAYER_PLAYER2 = 1,
	PLAYER_CHANCE = 2,
};

class Action
{
public:
	virtual void print() = 0;
	virtual Action* clone() = 0;

	float probability;
};

class State
{
public:
	virtual void print() = 0;

	virtual void computeActions( std::vector<Action*>* actions ) = 0;
	virtual void destroyActions( std::vector<Action*>* actions, Action* toKeep );
	virtual Action* getRandomAction();
	virtual Action* getProbabilisticAction();
	virtual void transitionModel( Action* action ) = 0;

	virtual bool isTerminal() = 0;
	virtual std::vector<float> utility() = 0;

	virtual State* clone() = 0;
	virtual void copy( State* state ) = 0;
	
	virtual int getActingPlayer() { return turn; }
	int getNumPlayers() { return numPlayers; }

protected:
	int turn;
	int numPlayers;
};

class HeuristicState : public State
{
public:
	virtual std::vector<float> heuristicValue() = 0;
};

#endif