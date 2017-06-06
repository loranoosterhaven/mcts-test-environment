/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#ifndef MCTS_H
#define MCTS_H

class MCTS : public Search
{
public:
	MCTS();

	virtual SearchResult* search( State* state );

	void setComputationalBudget( int computationalBudget ) { this->computationalBudget = computationalBudget; }
	void setSimulationDepth( int simulationDepth ) { this->simulationDepth = simulationDepth; }

private:
	Node* treePolicy( Node* targetNode );

	Node* bestChild( Node* targetNode, float explorationScale );
	Node* probablisticChild( Node* targetNode );

	std::vector<float> defaultPolicy( State* targetState );
	void backup( Node* selection, std::vector<float>* deltaValue );

private:
	int computationalBudget;
	int simulationDepth;
	float positiveConstant;
};

#endif