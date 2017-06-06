/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#include "stdafx.h"

TicTacToeState::TicTacToeState()
{
	turn = PLAYER_PLAYER1;
	winner = PLAYER_NONE;

	for( int i = 0; i < 9; i++ )
		board[i] = PLAYER_NONE;

	terminal = false;
	numPlayers = 2;
}

void TicTacToeState::print()
{
	for( int i = 0; i < 3; i++ )
	{
		printf( "{ " );

		for( int j = 0; j < 3; j++ )
		{
			printf( " %i ", board[3 * i + j] );
		}

		printf( "}\n" );
	}
}

void TicTacToeState::computeActions( std::vector<Action*>* actions )
{
	actions->clear();

	for( int i = 0; i < 9; i++ )
	{
		if( board[i] == PLAYER_NONE )
			actions->push_back( new TicTacToeAction( i ) );
	}
}

void TicTacToeState::transitionModel( Action* action )
{
	TicTacToeAction* ticTacToeAction = ( TicTacToeAction* )action;

	if( ticTacToeAction->boardIndex < 9 && board[ticTacToeAction->boardIndex] == PLAYER_NONE )
	{
		board[ticTacToeAction->boardIndex] = turn;
		turn = 1 - turn;

		computeTerminal();
	}
}

bool TicTacToeState::isTerminal()
{
	return terminal;
}

std::vector<float> TicTacToeState::utility()
{
	std::vector<float> result(numPlayers);

	if( terminal )
	{
		if( winner == PLAYER_NONE )
		{
			result[PLAYER_PLAYER1] = 0.5f;
			result[PLAYER_PLAYER2] = 0.5f;
		}
		else if( winner == PLAYER_PLAYER1 )
		{
			result[PLAYER_PLAYER1] = 1.0f;
			result[PLAYER_PLAYER2] = 0.0f;
		}
		else if( winner == PLAYER_PLAYER2 )
		{
			result[PLAYER_PLAYER1] = 0.0f;
			result[PLAYER_PLAYER2] = 1.0f;
		}
	}

	return result;
}

State* TicTacToeState::clone()
{
	return new TicTacToeState( *this );
}

void TicTacToeState::copy( State* state )
{
	*this = *( TicTacToeState* )state;
}

void TicTacToeState::computeTerminal()
{
	const int winningSet[8][3] = { 
		{ 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, 
		{ 0, 3, 6 }, { 1, 4, 7 }, { 2, 5, 8 },
		{ 0, 4, 8 }, { 6, 4, 2 }
	};

	for( int i = 0; i < 8; i++ )
	{
		int targetPlayer = board[winningSet[i][0]];

		if( targetPlayer != PLAYER_NONE )
		{
			if( board[winningSet[i][1]] == targetPlayer && board[winningSet[i][2]] == targetPlayer )
			{
				terminal = true;
				winner = targetPlayer;
				break;
			}
		}
	}

	if( !terminal )
	{
		bool freeTile = false;

		for( int i = 0; i < 9; i++ )
		{
			if( board[i] == PLAYER_NONE )
			{
				freeTile = true;
				break;
			}
		}
		
		if( !freeTile )
		{
			terminal = true;
			winner = PLAYER_NONE;
		}
	}
}