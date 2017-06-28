/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#ifndef TRANSPOSITIONMCTS_H
#define TRANSPOSITIONMCTS_H

class TranspositionMCTS : public Search
{
public:
	TranspositionMCTS( float positiveConstant, bool budgetInMs ) : 
		computationalBudget( 0 ), simulationDepth( 0 ), positiveConstant( positiveConstant ), budgetInMs( budgetInMs ) {}

	virtual SearchResult* search( State* state );

	void setComputationalBudget( long long computationalBudget ) { this->computationalBudget = computationalBudget; }
	void setSimulationDepth( int simulationDepth ) { this->simulationDepth = simulationDepth; }
	void setPositiveConstant( float positiveConstant ) { this->positiveConstant = positiveConstant; }

private:
	Vertex* treePolicy( Vertex* targetVertex );

	Vertex* bestChild( Vertex* targetVertex, float explorationScale );
	Vertex* probablisticChild( Vertex* targetNode );
	
	std::vector<float> defaultPolicy( State* targetState );
	void backup( Vertex* selection, std::vector<float>* deltaValue );

private:
	bool budgetInMs;
	long long computationalBudget;

	int simulationDepth;
	float positiveConstant;

	Vertex** transpositions;
};

#endif