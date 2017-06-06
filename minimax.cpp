/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#include "stdafx.h"

SearchResult* MinimaxSearch::search( State* state )
{
	agent = state->getActingPlayer();

	State* usedState = state->clone();

	// Compute all the actions from the current state.
	std::vector<Action*> actions;
	usedState->computeActions( &actions );

	int bestActionIndex = 0;
	float bestValue = -std::numeric_limits<float>::max();

	for( int i = 0; i < ( int )actions.size(); i++ )
	{
		usedState->transitionModel( actions[i] );
		float value = minValue( usedState );

		if( value > bestValue )
		{
			bestValue = value;
			bestActionIndex = i;
		}
		
		usedState->copy( state );
	}
	
	SearchResult* result = new SearchResult();
	result->bestAction = actions[bestActionIndex];
	result->bestState = state->clone();
	result->bestState->transitionModel( result->bestAction );

	// Clean up.
	state->destroyActions( &actions, result->bestAction );

	delete usedState;

	return result;
}

float MinimaxSearch::maxValue( State* state )
{
	if( state->isTerminal() )
		return state->utility()[agent];

	float bestValue = -std::numeric_limits<float>::max();

	State* usedState = state->clone();

	std::vector<Action*> actions;
	usedState->computeActions( &actions );

	for( int i = 0; i < ( int )actions.size(); i++ )
	{
		usedState->transitionModel( actions[i] );
		float value = minValue( usedState );

		if( value > bestValue )
			bestValue = value;
		
		usedState->copy( state );
	}

	state->destroyActions( &actions, NULL );

	delete usedState;

	return bestValue;
}

float MinimaxSearch::minValue( State* state )
{
	if( state->isTerminal() )
		return state->utility()[agent];

	float bestValue = std::numeric_limits<float>::max();

	State* usedState = state->clone();

	std::vector<Action*> actions;
	usedState->computeActions( &actions );

	for( int i = 0; i < ( int )actions.size(); i++ )
	{
		usedState->transitionModel( actions[i] );
		float value = maxValue( usedState );

		if( value < bestValue )
			bestValue = value;
		
		usedState->copy( state );
	}

	state->destroyActions( &actions, NULL );

	delete usedState;

	return bestValue;
}