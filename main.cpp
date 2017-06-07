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
	int numChips = 50;
	int maxChips = 3;
	int maxMoves = 100;

	NimState* nimState = new NimState( numChips, maxChips );
	printf( "Simulating %d searches of the initial Nim with n=%d and k=%d\n\nBudget rate:\tOptimal rate:", maxMoves, numChips, maxChips );
		
	MCTS* mcts = new MCTS( 1 / sqrtf( 2.0f ), false );
	mcts->setSimulationDepth( 1000 );

	for( int j = 1; j <= 100; j++ )
	{
		mcts->setComputationalBudget( j * 5000 );
	
		int numWrongMoves = 0;
		
		for( int i = 0; i < maxMoves; i++ )
		{
			SearchResult* result = mcts->search( nimState );

			NimAction* nimAction = ( NimAction* )result->bestAction;

			int optimalChips = nimState->getOptimalChips();

			if( optimalChips != nimAction->chips )
				numWrongMoves++;
		
			delete result;
		}

		float optimalRate = ( float )( maxMoves - numWrongMoves ) / ( float )maxMoves;
		printf( "\n%12d\t%.4f", j * 5000, optimalRate );
	}

	delete mcts;
}

int main( int argc, char* argv[] )
{
	srand( ( unsigned int )std::time( NULL ) );

	simulateNimMoves();
	getchar();

	return 0;
}