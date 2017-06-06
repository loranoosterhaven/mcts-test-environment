/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#include "stdafx.h"

void State::destroyActions( std::vector<Action*>* actions, Action* toKeep )
{
	for( int i = 0; i < ( int )actions->size(); i++ )
	{
		if( ( *actions )[i] != toKeep )
			delete ( *actions )[i];
	}

	actions->clear();
}

Action* State::getRandomAction()
{
	if( turn == PLAYER_CHANCE )
		return getProbabilisticAction();

	std::vector<Action*> actions( MAX_ACTIONS );
	computeActions( &actions );

	Action* targetAction = actions[rand() % actions.size()];
	destroyActions( &actions, targetAction );

	return targetAction;
}

Action* State::getProbabilisticAction()
{
	Action* bestAction = NULL;

	float randomFloat = ( float )rand() / RAND_MAX;
	float cumulativeProb = 0.0f;

	std::vector<Action*> actions( MAX_ACTIONS );
	computeActions( &actions );
	
	for( int i = 0; i < ( int )actions.size(); i++ )
	{
		bestAction = actions[i];
		cumulativeProb += actions[i]->probability;

		if( cumulativeProb >= randomFloat )
			break;
	}

	destroyActions( &actions, bestAction );

	return bestAction;
}