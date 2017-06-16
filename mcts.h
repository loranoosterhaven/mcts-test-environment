/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#ifndef MCTS_H
#define MCTS_H

#define SIMULATION_SCALE 50

enum EThreadState
{
	THREAD_IDLE,
	THREAD_WORKING,
	THREAD_SHUTDOWN
};

extern int* wrongMoves;

class MCTS : public Search
{
public:
	MCTS( float positiveConstant, bool budgetInMs, int numThreads, bool transpositions ) : 
		computationalBudget( 0 ), simulationDepth( 0 ), positiveConstant( positiveConstant ), budgetInMs( budgetInMs ), numThreads( numThreads ), transpositions( transpositions ) {}

	virtual SearchResult* search( State* state );

	void setComputationalBudget( long long computationalBudget ) { this->computationalBudget = computationalBudget; }
	void setSimulationDepth( int simulationDepth ) { this->simulationDepth = simulationDepth; }
	void setPositiveConstant( float positiveConstant ) { this->positiveConstant = positiveConstant; }

private:
	Node* treePolicy( Node* targetNode );

	Node* bestChild( Node* targetNode, float explorationScale );
	Node* probablisticChild( Node* targetNode );
	
	std::vector<float> defaultPolicy( State* targetState );
	void backup( Node* selection, std::vector<float>* deltaValue );

private:
	bool budgetInMs;
	long long computationalBudget;

	int simulationDepth;
	float positiveConstant;

	int numThreads;

	bool transpositions;
	Node** transTable;
};

#endif