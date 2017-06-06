/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#include "stdafx.h"

SearchResult* DepthLimitedMinimaxSearch::search( State* state )
{
	agent = state->getActingPlayer();

	HeuristicState* usedState = ( HeuristicState* )state->clone();

	// Compute all the actions from the current state.
	std::vector<Action*> actions( MAX_ACTIONS );
	usedState->computeActions( &actions );

	int bestActionIndex = 0;
	float bestValue = -std::numeric_limits<float>::max();

	for( int i = 0; i < ( int )actions.size(); i++ )
	{
		usedState->transitionModel( actions[i] );

		// Evaluate the recursive function.
		float value = minValue( usedState, 1 );

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

float DepthLimitedMinimaxSearch::maxValue( HeuristicState* state, int depth )
{
	// If the state is a terminal state we return the utility for the target agent.
	if( state->isTerminal() )
		return state->utility()[agent];

	// Perform the cut off test.
	if( depth > depthLimit )
		return state->heuristicValue()[agent];

	float bestValue = -std::numeric_limits<float>::max();

	HeuristicState* usedState = ( HeuristicState* )state->clone();

	std::vector<Action*> actions( MAX_ACTIONS );
	usedState->computeActions( &actions );

	for( int i = 0; i < ( int )actions.size(); i++ )
	{
		usedState->transitionModel( actions[i] );
		float value = minValue( usedState, depth + 1 );

		if( value > bestValue )
			bestValue = value;
		
		usedState->copy( state );
	}

	state->destroyActions( &actions, NULL );

	delete usedState;

	return bestValue;
}

float DepthLimitedMinimaxSearch::minValue( HeuristicState* state, int depth )
{
	// If the state is a terminal state we return the utility for the target agent.
	if( state->isTerminal() )
		return state->utility()[agent];

	// Perform the cut off test.
	if( depth > depthLimit )
		return state->heuristicValue()[agent];

	float bestValue = std::numeric_limits<float>::max();

	HeuristicState* usedState = ( HeuristicState* )state->clone();

	std::vector<Action*> actions( MAX_ACTIONS );
	usedState->computeActions( &actions );

	for( int i = 0; i < ( int )actions.size(); i++ )
	{
		usedState->transitionModel( actions[i] );
		float value = maxValue( usedState, depth + 1 );

		if( value < bestValue )
			bestValue = value;
		
		usedState->copy( state );
	}

	state->destroyActions( &actions, NULL );

	delete usedState;

	return bestValue;
}