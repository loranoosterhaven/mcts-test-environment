/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#include "stdafx.h"

#define PRINT( format, ... ) fprintf( OutputFile, format, __VA_ARGS__ ); printf( format, __VA_ARGS__ )

FILE* OutputFile = NULL;

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
	int maxGames = 1000;
	int numGames = 0;
	int wins[2] = { 0, 0 };

	printf( "Simulating %d games\n\nGame:\t\tUtility:\t\tWinner:\n", maxGames );

	while( numGames < maxGames )
	{
		State* state = new NimState( 7, 3 );

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
	int maxSimulations = 1000;		
	
	MCTS* mcts = new MCTS( 1 / sqrtf( 2.0f ), false, 1, false );
	mcts->setSimulationDepth( 20000 );
		
	for( int i = 1; i <= 60; i++ )
	{
		long computationalBudget = i * 1000;

		NimState* nimState = new NimState( numChips, maxChips );

		int optimalChips = nimState->getOptimalChips();

		if( optimalChips == 0 )
		{
			delete nimState;

			return;
		}

		mcts->setComputationalBudget( computationalBudget );
	
		int numWrongMoves = 0;
		
		for( int i = 0; i < maxSimulations; i++ )
		{
			SearchResult* result = mcts->search( nimState );
			
			NimAction* nimAction = ( NimAction* )result->bestAction;
			
			if( optimalChips != nimAction->chips )
				numWrongMoves++;
			
			printf( "Search %d with budget %ld done!\n", i, computationalBudget );

			delete result;
		}

		float optimalRate = 1.0f - ( ( float )numWrongMoves / ( float )maxSimulations );

		PRINT( "%.3f\t%ld\t%d\t%d\t%d\t%d\t%f\n", optimalRate, computationalBudget, numChips, maxChips, 1, 0, 1 / sqrtf( 2.0f ) );

		fflush( OutputFile );

		delete nimState;
	}

	delete mcts;
}

int main( int argc, char* argv[] )
{
	srand( ( unsigned int )std::time( NULL ) );

	char fileName[40];
	sprintf( fileName, "output%d.txt", ( int )time( NULL ) );
	OutputFile = fopen( fileName, "w+" );
	
	simulateNimMoves();

	fclose( OutputFile );
	getchar();

	return 0;
}