/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#include "stdafx.h"

void NimState::print()
{
	printf( "Chips: %d Turn: Player %d\n", chips, turn + 1 );
}

void NimState::computeActions( std::vector<Action*>* actions )
{
	actions->clear();

	for( int i = 1; i <= std::min( 3, chips ); i++ )
	{
		NimAction* action = new NimAction();
		action->chips = i;

		actions->push_back( action );
	}

	std::random_shuffle( actions->begin(), actions->end() );
}

void NimState::transitionModel( Action* action )
{
	NimAction* nimAction = ( NimAction* )action;

	chips -= nimAction->chips;
	turn = 1 - turn;
}

bool NimState::isTerminal()
{
	return chips <= 0;
}

std::vector<float> NimState::utility()
{
	std::vector<float> result(numPlayers);

	if( turn == PLAYER_PLAYER1 )
	{
		result[PLAYER_PLAYER1] = 1.0f;
		result[PLAYER_PLAYER2] = 0.0f;
	}
	else
	{
		result[PLAYER_PLAYER1] = 0.0f;
		result[PLAYER_PLAYER2] = 1.0f;
	}

	return result;
}

State* NimState::clone()
{
	return new NimState( *this );
}

void NimState::copy( State* state )
{
	*this = *( NimState* )state;
}