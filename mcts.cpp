/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#include "stdafx.h"

int* wrongMoves = NULL;

SearchResult* MCTS::search( State* state )
{
	// Create root node from target state.
	Node* root = new Node( state->clone(), NULL, NULL );
			
	std::clock_t startClock = std::clock();
	std::clock_t endClock = startClock;

	long long computationalBudgetClock = computationalBudget * CLOCKS_PER_SEC / 1000;
	long long numIterations = 0;

	// Run until computational budget is reached.
	while( ( budgetInMs && endClock - startClock < computationalBudgetClock ) 
		|| ( !budgetInMs && numIterations < computationalBudgetClock ) )
	{
		// Select a child according to the tree policy.
		Node* selection = treePolicy( root );
		
		// Run a simulation from the state of the selected node.
		std::vector<float> deltaValue = defaultPolicy( selection->getState() );

		// Perform backpropagation using the obtained estimate.
		backup( selection, &deltaValue );
			
		endClock = std::clock();
		
		if( !budgetInMs && numIterations > 0 && numIterations % 1000000 == 0 )
			printf( "%ld simulations remaining...\n", computationalBudgetClock - numIterations );

		if( numIterations % SIMULATION_SCALE == 0 && ( ( NimAction* )bestChild( root, 0.0f )->getAppliedAction() )->chips != ( ( NimState* )state )->getOptimalChips() )
			wrongMoves[numIterations / SIMULATION_SCALE]++;

		numIterations++;
	}

	Node* bestNode = bestChild( root, 0.0f );

	int optimalDepth = 1;
	Node* optimalDepthNode = bestNode;

	while( optimalDepthNode != NULL )
	{
		optimalDepthNode = bestChild( optimalDepthNode, 0.0f );
		optimalDepth++;
	}

	SearchResult* result = new SearchResult();
	result->bestState = state->clone();
	result->mctsIterations = numIterations;

	// Store the search results.
	if( bestNode != NULL && bestNode->getAppliedAction() != NULL )
	{
		result->bestAction = bestNode->getAppliedAction()->clone();
		result->bestState->transitionModel( result->bestAction );
	}

	delete root;

	return result;
}

Node* MCTS::treePolicy( Node* targetNode )
{
	Node* selection = targetNode;

	while( !selection->isTerminal() )
	{
		// If the current node is not fully expanded yet add an additional child. Else select based on some policy.
		if( !selection->isFullyExpanded() )
			return selection->expand();
		else if( selection->getState()->getActingPlayer() == PLAYER_CHANCE )
			selection = probablisticChild( selection );
		else
			selection = bestChild( selection, positiveConstant );
	}

	return selection;
}
	
Node* MCTS::bestChild( Node* targetNode, float explorationScale )
{
	// Only works on choice nodes.
	float bestUCTScore = -std::numeric_limits<float>::max();
    Node* bestNode = NULL;

    int numChildren = targetNode->getNumChildren();
	float parentVisits = ( float )targetNode->getNumVisits();

    for( int i = 0; i < numChildren; i++ ) 
	{
        Node* child = targetNode->getChild( i );
		float childVisits = ( float )child->getNumVisits();

		// The UCT exploitation term.
        float UCTExploitation = child->getNodeValue( targetNode->getState()->getActingPlayer() ) / childVisits;

		// The UCT exploration term.
        float UCTExploration = sqrtf( 2.0f * logf( parentVisits ) / childVisits );

		// Add the terms and scale the exploration term.
        float UCTScore = UCTExploitation + 2.0f * explorationScale * UCTExploration;

		// Save the child with the highest UCT score.
        if( UCTScore > bestUCTScore )
		{
            bestUCTScore = UCTScore;
            bestNode = child;
        }
    }

    return bestNode;
}

Node* MCTS::probablisticChild( Node* targetNode )
{
	Node* bestChild = NULL;

	float randomFloat = ( float )rand() / RAND_MAX;
	float cumulativeProb = 0.0f;

    int numChildren = targetNode->getNumChildren();

	// Pick a child based on the probability distribution.
    for( int i = 0; i < numChildren; i++ ) 
	{
        Node* child = targetNode->getChild( i );
		bestChild = child;

		if( cumulativeProb > randomFloat )
			break;
		
		cumulativeProb += child->getAppliedAction()->probability;
	}

	return bestChild;
}

std::vector<float> MCTS::defaultPolicy( State* targetState )
{
	State* simulationState = targetState->clone();

	int depth = 0;

	// Run simulations of from the target state up to a terminal state or a given depth.
	while( depth < simulationDepth )
	{
		if( simulationState->isTerminal() )
			break;

		// Select an action uniformly random or if it is a chance event based on the probability distribution.
		Action* action = simulationState->getRandomAction();

		simulationState->transitionModel( action );
		delete action;

		depth++;
	}
	
	std::vector<float> result = simulationState->utility();

	delete simulationState;

	return result;
}

void MCTS::backup( Node* selection, std::vector<float>* deltaValue )
{
	Node* current = selection;
	
	// Update all parent nodes based on the obtained estimate.
	while( current != NULL )
	{
		current->update( deltaValue );
		current = current->getParent();
	}
}