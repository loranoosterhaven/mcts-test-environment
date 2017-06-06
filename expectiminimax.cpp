/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#include "stdafx.h"

SearchResult* ExpectiminimaxSearch::search( State* state )
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
		float value = minimaxValue( usedState, 1, -std::numeric_limits<float>::max(), std::numeric_limits<float>::max() );

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

float ExpectiminimaxSearch::chanceValue( HeuristicState* state, int depth, float alpha, float beta )
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

	float expectedValue = 0.0f;

	for( int i = 0; i < ( int )actions.size(); i++ )
	{
		usedState->transitionModel( actions[i] );
		
		if( usedState->getActingPlayer() == PLAYER_CHANCE )
			expectedValue += actions[i]->probability * chanceValue( usedState, depth + 1, alpha, beta );
		else
			expectedValue += actions[i]->probability * minimaxValue( usedState, depth + 1, alpha, beta );

		usedState->copy( state );
	}

	state->destroyActions( &actions, NULL );

	delete usedState;

	return expectedValue;
}

float ExpectiminimaxSearch::minimaxValue( HeuristicState* state, int depth, float alpha, float beta )
{
	// If the state is a terminal state we return the utility for the target agent.
	if( state->isTerminal() )
		return state->utility()[agent];

	// Perform the cut off test.
	if( depth > depthLimit )
		return state->heuristicValue()[agent];

	float bestValue = -std::numeric_limits<float>::max();

	if( state->getActingPlayer() != PLAYER_PLAYER1 )
		bestValue = std::numeric_limits<float>::max();

	HeuristicState* usedState = ( HeuristicState* )state->clone();

	std::vector<Action*> actions( MAX_ACTIONS );
	usedState->computeActions( &actions );

	for( int i = 0; i < ( int )actions.size(); i++ )
	{
		usedState->transitionModel( actions[i] );
		
		float value = 0.0f;
		
		if( usedState->getActingPlayer() == PLAYER_CHANCE )
			value = chanceValue( usedState, depth + 1, alpha, beta );
		else
			value = minimaxValue( usedState, depth + 1, alpha, beta );
		
		if( state->getActingPlayer() == PLAYER_PLAYER1 )
		{
			if( value > bestValue )
				bestValue = value;

			if( bestValue >= beta )
				break;

			alpha = std::max( alpha, bestValue );
		}
		else
		{
			if( value < bestValue )
				bestValue = value;

			if( bestValue <= alpha )
				break;

			beta = std::min( beta, bestValue );
		}

		usedState->copy( state );
	}

	state->destroyActions( &actions, NULL );

	delete usedState;

	return bestValue;
}