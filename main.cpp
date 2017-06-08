/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#include "stdafx.h"

void processStats( State* terminalState, int* wins, int gameNum )
{		
	printf( "%4d\t\t(", gameNum + 1 );

	std::vector<float> endUtility = terminalState->utility();

	for( int i = 0; i < ( int )endUtility.size(); i++ )
		printf( " %.2f ", endUtility[i] );

	printf( ")\t\t" );

	bool draw = true;
	int winner = 0;
	float winnerUtility = 0.0f;

	for( int i = 0; i < ( int )endUtility.size(); i++ )
	{
		if( i < ( int )endUtility.size() - 1 && endUtility[i] != endUtility[i + 1] )
			draw = false;

		if( endUtility[i] > winnerUtility )
		{
			winnerUtility = endUtility[i];
			winner = i;
		}
	}

	if( draw )
	{
		printf( "Draw\n" );
	}
	else
	{
		printf( "Player %d\n", winner + 1 );
		wins[winner]++;
	}
}

void simulateGames()
{
	int maxGames = 50;
	int numGames = 0;
	int wins[2] = { 0, 0 };

	printf( "Simulating %d games\n\nGame:\t\tUtility:\t\tWinner:\n", maxGames );

	while( numGames < maxGames )
	{
		State* state = new NimState( 21, 3 );

		MCTS* mcts = new MCTS( 1 / sqrtf( 2.0f ), false, 1, false );
		mcts->setComputationalBudget( 300000 );
		mcts->setSimulationDepth( 1000 );

		RandomSearch* randomSearch = new RandomSearch();
		MinimaxSearch* minimaxSearch = new MinimaxSearch();

		ExpectiminimaxSearch* expectiminimaxSearch = new ExpectiminimaxSearch();
		expectiminimaxSearch->setDepthLimit( 4 );

		while( !state->isTerminal() )
		{
			SearchResult* result = NULL;

			if( state->getActingPlayer() == PLAYER_CHANCE )
				result = randomSearch->search( state );
			else
				result = mcts->search( state );

			delete state;
			state = result->bestState->clone();

			if( state->isTerminal() )
			{
				processStats( state, wins, numGames );
				break;
			}

			delete result;
		}

		delete randomSearch;
		delete mcts;
		delete state;
		
		numGames++;
	}

	int totalWins = wins[0];
	int totalDraws = numGames - wins[0] - wins[1];
	int totalLoss = wins[1];

	printf( "\nWins %d Draws %d Loss %d\nWin rate %.2f Draw rate %.2f Loss rate %.2f", totalWins, totalDraws, totalLoss, 
		( float )totalWins / ( float )numGames, ( float )totalDraws / ( float )numGames, ( float )totalLoss / ( float )numGames );
}

void simulateNimMoves()
{
	int numChips = 21;
	int maxChips = 3;
	int maxSimulations = 100;		
	
	long computationalBudget = 45000;

	NimState* nimState = new NimState( numChips, maxChips );

	int optimalChips = nimState->getOptimalChips();

	if( optimalChips == 0 )
	{
		printf( "Wrong choice of chips.\n" );

		delete nimState;

		return;
	}

	printf( "Simulating %d searches of the initial Nim with n=%d and k=%d\n\n", maxSimulations, numChips, maxChips );


	MCTS* mcts = new MCTS( 1 / sqrtf( 2.0f ), false, 1, false );
	mcts->setSimulationDepth( 1000 );
	mcts->setComputationalBudget( computationalBudget );
	
	int numWrongMoves = 0;
		
	for( int i = 0; i < maxSimulations; i++ )
	{
		SearchResult* result = mcts->search( nimState );

		NimAction* nimAction = ( NimAction* )result->bestAction;
			
		if( optimalChips != nimAction->chips )
		{
			printf( "%d: Wrong move.\n", i );
			numWrongMoves++;
		}
		else
			printf( "%d: Correct move.\n", i );

		delete result;
	}

	printf( "\nOptimal move rate %.2f\n", 1.0f - ( ( float )numWrongMoves / ( float )maxSimulations ) );

	delete nimState;
	delete mcts;
}

int main( int argc, char* argv[] )
{
	srand( ( unsigned int )std::time( NULL ) );

	simulateNimMoves();
	getchar();

	return 0;
}