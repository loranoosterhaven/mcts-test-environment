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
	int maxSimulations = 1000;		
	int numCores = 1;
	float positiveConstant = 1.0f / sqrtf( 2.0f );
	bool transpositions = 0;
		
	MCTS* mcts = new MCTS( positiveConstant, false, numCores, transpositions );
	mcts->setSimulationDepth( 20000 );
	
	long long computationalBudget = 14000;

	mcts->setComputationalBudget( computationalBudget );

	int numChips = 18;
	int maxChips = 4;

	NimState* nimState = new NimState( numChips, maxChips );

	int optimalChips = nimState->getOptimalChips();

	if( optimalChips == 0 )
	{
		delete nimState;
		return;
	}
	
	char fileName[40];
	sprintf( fileName, "output-B%ld-N%d-K%d.csv", computationalBudget, numChips, maxChips );
	OutputFile = fopen( fileName, "w+" );
	
	PRINT( "optimalRate,iterations,maxDepth,branchingFactor,cores,transpositions,positiveConstant,chance\n" );

	for( int p = 0; p < 300; p++ )
	{
		positiveConstant = ( float )p / 300.0f;

		long numMeasurements = ( long )( computationalBudget / SIMULATION_SCALE );
		wrongMoves = new int[numMeasurements];

		memset( wrongMoves, 0, sizeof( int ) * numMeasurements );

		mcts->setPositiveConstant( positiveConstant );

		printf( "Starting simulations with maxDepth=%d maxBudget=%ld\n\n", numChips, computationalBudget );

		int numWrongMoves = 0;
		
		for( int i = 0; i < maxSimulations; i++ )
		{
			SearchResult* result = mcts->search( nimState );

			printf( "Search %d done current optimal rate (%.2f)\n", i + 1, 1.0f - ( ( float )wrongMoves[numMeasurements - 1] / ( float )( i + 1 ) ) );

			delete result;
		}

		float optimalRate = 1.0f - ( ( float )wrongMoves[numMeasurements - 1] / ( float )maxSimulations );

		PRINT( "%.3f,%ld,%d,%d,%d,%d,%f,%f\n", optimalRate, numMeasurements * SIMULATION_SCALE, numChips, maxChips, numCores, ( int )transpositions, positiveConstant, 1.0f / ( float )maxChips );
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

	/*printf("n,k,nodes\n");

	for( int i = 5; i <= 29; i++ )
	{
		for( int j = 2; j <= 10; j++ )
			printf( "%d,%d,%d\n", i, j, computeNumNodes( i, j ) );
	}

	getchar();*/

	simulateNimMoves();

	return 0;
}