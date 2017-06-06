/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#include "stdafx.h"

SearchResult* RandomSearch::search( State* state )
{
	SearchResult* result = new SearchResult();
	result->bestAction = state->getRandomAction();
	result->bestState = state->clone();
	result->bestState->transitionModel( result->bestAction );

	return result;
}