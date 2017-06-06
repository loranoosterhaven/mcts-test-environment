/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#include "stdafx.h"

const int initialBoard[NUM_POSITIONS] = {
	 0, // Number of entering checkers for black
	-2, 0, 0, 0,  0, 5,		 0, 3, 0, 0, 0, -5, // Bottom row of the board
	 5, 0, 0, 0, -3, 0,		-5, 0, 0, 0, 0,  2, // Top row of the board
	 0 // Number of entering checkers for white
};

const int initialWhiteCheckers[NUM_CHECKERS] = { 24, 13, 8, 6 };
const int initialBlackCheckers[NUM_CHECKERS] = { 1, 12, 17, 19 };

BackgammonState::BackgammonState()
{
	// Backgammon is only possible with two players.
	numPlayers = 2;

	// Throw the starting dice to determine who's turn it is.
	do
	{
		dice = throwDices();
	}
	while( dice.isDouble() );

	// Determine who gets to make the opening move.
	turn = ( rand() % 2 )? PLAYER_WHITE : PLAYER_BLACK;
	actiontaker = turn;

	// Copy the initial board.
	memcpy( board, initialBoard, sizeof( board ) );

	memcpy( whiteCheckers, initialWhiteCheckers, sizeof( initialWhiteCheckers ) );
	numWhitePositions = 4;
	whiteBearOff = false;

	memcpy( blackCheckers, initialBlackCheckers, sizeof( initialBlackCheckers ) );
	numBlackPositions = 4;
	blackBearOff = false;

	// The initial state is not a terminal state;
	winner = PLAYER_NONE;
}

void BackgammonState::print()
{
	// Print the top row of the board.
	printf( "Player: %d\n|", turn + 1 );

	for( int i = 13; i < NUM_POSITIONS - 1; i++ )
		printf( " %d ", board[i] );
		
	printf( "| %d\n", board[NUM_POSITIONS - 1] );

	// Print the bottom row of the board.
	printf( "|" );

	for( int i = 12; i > 0; i-- )
		printf( " %d ", board[i] );
		
	printf( "| %d\nDice: (%d, %d)\n", board[0], dice.high, dice.low );
}

void BackgammonState::computeActions( std::vector<Action*>* actions )
{
	actions->clear();

	if( turn == PLAYER_CHANCE )
	{
		// Generate all possible dices.
		for( int i = 1; i < 7; i++ )
		{
			BackgammonAction* action = new BackgammonAction();
			action->probability = 0.0277778f;
			action->dice.high = i;
			action->dice.low = i;

			actions->push_back( action );

			for( int j = i + 1; j < 7; j++ )
			{
				BackgammonAction* action = new BackgammonAction();
				action->probability = 0.0555556f;
				action->dice.high = j;
				action->dice.low = i;

				actions->push_back( action );
			}
		}
	}
	else
	{
		// Set target variables for the white player.
		int barIndex = NUM_POSITIONS - 1;
		int opponentBarIndex = 0;
		int sign = 1;
		int numPositions = numWhitePositions;
		int* checkers = whiteCheckers;
		bool bearOff = whiteBearOff;

		// If we are the black player we have to change the target variables.
		if( turn == PLAYER_BLACK )
		{
			barIndex = 0;
			opponentBarIndex = NUM_POSITIONS - 1;
			sign = -1;
			numPositions = numBlackPositions;
			checkers = blackCheckers;
			bearOff = blackBearOff;
		}

		int numDice = 2;
		int diceNums[4];

		if( dice.high == dice.low )
		{
			numDice = 4;
			diceNums[0] = dice.high;
			diceNums[1] = dice.high;
			diceNums[2] = dice.high;
			diceNums[3] = dice.high;
		}
		else
		{
			diceNums[0] = dice.high;
			diceNums[1] = dice.low;
		}

		for( int i = 0; i < numDice; i++ )
		{
			int diceNum = diceNums[i];

			// We have to enter with a checker.
			if( sign * board[barIndex] > 0 )
			{
				if( sign * board[barIndex - sign * diceNum] >= -1 )
				{
					BackgammonAction* action = new BackgammonAction();
					action->moves[0].src = barIndex;
					action->moves[0].dest = barIndex - sign * diceNum;
					action->numMoves = 1;

					actions->push_back( action );
				}
			}
			else
			{
				// We determine the possible moves for each available tower.
				for( int i = 0; i < numPositions; i++ )
				{
					int dest = checkers[i] - sign * diceNum;

					// If the destination according to the dice is available, we create a move.
					if( sign * dest > sign * opponentBarIndex && sign * board[dest] >= -1 )
					{
						BackgammonAction* action = new BackgammonAction();
						action->moves[0].src = checkers[i];
						action->moves[0].dest = dest;
						action->numMoves = 1;
					
						actions->push_back( action );
					}
					else if( bearOff ) // We are able to create a bear off move.
					{
						int maxAllowed = sign * ( checkers[0] - opponentBarIndex );

						if( dest == opponentBarIndex || ( diceNum > maxAllowed && i == 0 ) ) 
						{
							BackgammonAction* action = new BackgammonAction();
							action->moves[0].src = checkers[i];
							action->moves[0].dest = POSITION_BEARINGOFF;
							action->numMoves = 1;

							actions->push_back( action );
						}
					}
				}
			}
		}
	}

	// If no actions are possible. The player performs a no move.
	if( actions->empty() )
	{
		BackgammonAction* action = new BackgammonAction();
		action->numMoves = 0;

		actions->push_back( action );
	}
	else
	{
		std::random_shuffle( actions->begin(), actions->end() );
	}
}

void BackgammonState::transitionModel( Action* action )
{
	BackgammonAction* backgammonAction = ( BackgammonAction* )action;

	if( turn != PLAYER_CHANCE )
	{
		// Set target variables for the white player.
		int barIndex = NUM_POSITIONS - 1;
		int opponentBarIndex = 0;
		int sign = 1;

		// If we are the black player we have to change the target variables.
		if( turn == PLAYER_BLACK )
		{
			barIndex = 0;
			opponentBarIndex = NUM_POSITIONS - 1;
			sign = -1;
		}

		// Apply each move of the action.
		for( int i = 0; i < backgammonAction->numMoves; i++ )
		{
			int dest = backgammonAction->moves[i].dest;
			int src = backgammonAction->moves[i].src;

			if( dest != POSITION_BEARINGOFF )
			{
				// Check if it hits a checker.
				if( sign * board[dest] == -1 )
				{
					board[dest] = sign * 1;
					
					// Place checker on the bar.
					board[opponentBarIndex] -= sign;
 				}
				else
				{
					board[dest] += sign;
				}
			}

			board[src] -= sign;
		}

		// Fix the positions arrays.
		numWhitePositions = 0;

		// Create array in descending order.
		for( int i = NUM_POSITIONS - 1; i >= 0; i-- )
		{
			if( board[i] > 0 )
				whiteCheckers[numWhitePositions++] = i;
		}

		numBlackPositions = 0;
			
		// Create array in ascending order.
		for( int i = 0; i < NUM_POSITIONS; i++ )
		{
			if( board[i] < 0 )
				blackCheckers[numBlackPositions++] = i;
		}

		whiteBearOff = whiteCheckers[0] <= POSITION_WHITE_HOMEBOARD;
		blackBearOff = blackCheckers[0] >= POSITION_BLACK_HOMEBOARD;

		// If either white or black has no checkers in any positions the game is over.
		if( numWhitePositions == 0 )
			winner = PLAYER_WHITE;
		else if( numBlackPositions == 0 )
			winner = PLAYER_BLACK;

		turn = PLAYER_CHANCE;
		actiontaker = 1 - actiontaker;

		dice.high = 0;
		dice.low = 0;
	}
	else
	{
		// Set the dice according to the dice action.
		dice.high = backgammonAction->dice.high;
		dice.low = backgammonAction->dice.low;

		turn = actiontaker;
	}
}

Action* BackgammonState::getProbabilisticAction()
{
	BackgammonAction* action = new BackgammonAction();
	action->dice = throwDices();
	action->probability = 1.0f;

	return action;
}

bool BackgammonState::isTerminal()
{
	return winner != PLAYER_NONE;
}

std::vector<float> BackgammonState::utility()
{
	std::vector<float> result( numPlayers );

	if( winner != PLAYER_NONE )
	{
		if( winner == PLAYER_WHITE )
		{
			// Check for backgammon, gammon or a normal win.
			if( board[0] < 0 )
				result[PLAYER_WHITE] = 1.0f;
			else if( blackCheckers[0] < POSITION_BLACK_HOMEBOARD )
				result[PLAYER_WHITE] = 0.667f;
			else
				result[PLAYER_WHITE] = 0.333f;

			result[PLAYER_BLACK] = 0.0f;
		}
		else if( winner == PLAYER_BLACK )
		{
			result[PLAYER_WHITE] = 0.0f;
			
			// Check for backgammon, gammon or a normal win.
			if( board[NUM_POSITIONS - 1] > 0 )
				result[PLAYER_BLACK] = 1.0f;
			else if( whiteCheckers[0] > POSITION_WHITE_HOMEBOARD )
				result[PLAYER_BLACK] = 0.667f;
			else
				result[PLAYER_BLACK] = 0.333f;
		}
	}
	else
	{
		result[PLAYER_WHITE] = 0.0f;
		result[PLAYER_BLACK] = 0.0f;
	}

	return result;
}

std::vector<float> BackgammonState::heuristicValue()
{
	if( winner != PLAYER_NONE )
		return utility();

	int maxTotalSteps = NUM_CHECKERS * 24;
	int whiteTotalSteps = 0;

	for( int i = 0; i < numWhitePositions; i++ )
		 whiteTotalSteps += whiteCheckers[i] * board[whiteCheckers[i]];

	int blackTotalSteps = 0;

	for( int i = 0; i < numBlackPositions; i++ )
		blackTotalSteps += blackCheckers[i] * abs( board[blackCheckers[i]] );

	std::vector<float> heuristic( numPlayers );
	heuristic[PLAYER_WHITE] = ( float )( maxTotalSteps - whiteTotalSteps ) / ( float )maxTotalSteps;
	heuristic[PLAYER_BLACK] = ( float )( maxTotalSteps - blackTotalSteps ) / ( float )maxTotalSteps;

	return heuristic;
}

State* BackgammonState::clone()
{
	return new BackgammonState( *this );
}

void BackgammonState::copy( State* state )
{
	*this = *( BackgammonState* )state;
}

Dice BackgammonState::throwDices()
{
	int firstDice = 1 + rand() % 6;
	int secondDice = 1 + rand() % 6;

	Dice dice;
	dice.high = std::max( firstDice, secondDice );
	dice.low = std::min( firstDice, secondDice );

	return dice;
}