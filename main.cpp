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

		MCTS* mcts = new MCTS( 1 / sqrtf( 2.0f ), false );
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
	int numChips = 13;
	int maxChips = 4;
	int maxSimulations = 1000;		
	float positiveConstant = 0.220f;
	long long computationalBudget = 30000;
		
	MCTS* mcts = new MCTS( positiveConstant, false );
	mcts->setSimulationDepth( 10000 );

	long numMeasurements = ( long )( computationalBudget / SIMULATION_SCALE );
	wrongMoves = new int[numMeasurements];

	memset( wrongMoves, 0, sizeof( int ) * numMeasurements );
	NimState* nimState = new NimState( numChips, maxChips );
		
	int optimalChips = nimState->getOptimalChips();

	if( optimalChips == 0 )
	{
		printf( "Wrong number of chips.\n" );

		delete nimState;
		
		fclose( OutputFile );

		return;
	}

	mcts->setComputationalBudget( computationalBudget );
	
	printf( "Starting simulations\n\n" );

	int numWrongMoves = 0;

	for( int i = 0; i < maxSimulations; i++ )
	{
		SearchResult* result = mcts->search( nimState );

		if( i > 0 && i % 1000000 == 0 )
			printf( "%d simulations remaining...\n", maxSimulations - i );

		printf( "Search %d done current optimal rate (%.2f)\n", i + 1, 1.0f - ( ( float )wrongMoves[numMeasurements - 1] / ( float )( i + 1 ) ) );

		delete result;
	}

	char fileName[40];
	sprintf_s( fileName, "output-D%d-B%d.csv", numChips, maxChips );
	fopen_s( &OutputFile, fileName, "w+" );

	PRINT( "optimalRate,iterations,maxDepth,branchingFactor,positiveConstant,chance\n" );

	for( int i = 0; i < numMeasurements; i++ )
	{
		float optimalRate = 1.0f - ( ( float )wrongMoves[i] / ( float )maxSimulations );

		PRINT( "%.3f,%ld,%d,%d,%f,%f\n", optimalRate, ( i + 1 ) * SIMULATION_SCALE, numChips, maxChips, positiveConstant, 1.0f / ( float )maxChips );

		if( optimalRate >= 0.999f )
			break;
	}
	
	fclose( OutputFile );

	delete nimState;
	delete mcts;
}

int recNumNodes( int n, int k, int* transpositions )
{
	if( transpositions[n] != -1 )
		return transpositions[n];
	else if( n == 0 )
	{
		transpositions[0] = 1;

		return 1;
	}
	else if( k < n )
	{
		int sum = 1;

		for( int i = 1; i <= k; i++ )
			sum += recNumNodes( n - i, k, transpositions );

		transpositions[n] = sum;

		return sum;
	}
	else
	{
		int sum = 1;

		for( int i = 1; i <= n; i++ )
			sum += recNumNodes( n - i, k, transpositions );
		
		transpositions[n] = sum;

		return sum;
	}
}

int computeNumNodes( int n, int k )
{
	int* transpositions = new int[n + 1];
	memset( transpositions, -1, sizeof( int ) * ( n + 1 ) );

	int numNodes = recNumNodes( n, k, transpositions );
	delete[] transpositions;

	return numNodes;
}

int main( int argc, char* argv[] )
{
	srand( ( unsigned int )std::time( NULL ) );
	
	/*char fileName[40];
	sprintf( fileName, "nodes.csv" );
	OutputFile = fopen( fileName, "w+" );

	PRINT("n,k,nodes\n");

	for( int i = 5; i <= 29; i++ )
	{
		for( int j = 2; j <= i; j++ )
		{
			PRINT( "%d,%d,%d\n", i, j, computeNumNodes( i, j ) );
		}
	}

	fclose( OutputFile );*/

	simulateNimMoves();

	return 0;
}