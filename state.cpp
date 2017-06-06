/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#include "stdafx.h"

void State::destroyActions( std::vector<Action*>* actions, Action* toKeep )
{
	for( std::vector<Action*>::iterator it = actions->begin(); it != actions->end(); it++ )
	{
		if( *it != toKeep )
			delete *it;
	}

	actions->clear();
}

Action* State::getRandomAction()
{
	if( turn == PLAYER_CHANCE )
		return getProbabilisticAction();

	std::vector<Action*> actions;
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

	std::vector<Action*> actions;
	computeActions( &actions );
	
	for( std::vector<Action*>::iterator it = actions.begin(); it != actions.end(); it++ )
	{
		Action* action = *it;

		bestAction = action;
		cumulativeProb += action->probability;

		if( cumulativeProb >= randomFloat )
			break;
	}

	destroyActions( &actions, bestAction );

	return bestAction;
}