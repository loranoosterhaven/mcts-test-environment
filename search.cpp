/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#include "stdafx.h"

void SearchResult::print()
{
	printf( "Search Results\n" );

	if( bestAction != NULL )
	{
		printf( "Best action: " );
		bestAction->print();
	}

	if( mctsIterations != 0 )
		printf( "Number of simualtions: %d\n", mctsIterations );

	printf( "Best state:\n");
	bestState->print();
}