/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#ifndef VERTEX_H
#define VERTEX_H

class ParentData
{
public:
	class Vertex* parentVertex;
	Action* appliedAction;
};

class Vertex
{
public:
	Vertex( State* state, Vertex* parent, Action* appliedAction );
	~Vertex();

	Vertex* expand( Vertex** transpositions );
	void update( std::vector<float>* valueEstimate );

	ParentData* getParent( int parent );
	int getNumParents() { return ( int )parents.size(); }
	void addParent( ParentData* parentData ) { parents.push_back( parentData ); }

	Vertex* getChild( int child );
	int getNumChildren() { return ( int )children.size(); }

	State* getState() { return state; }

	bool isFullyExpanded() { return !actions.empty() && children.size() == actions.size(); }
	bool isTerminal() { return state->isTerminal(); }
	
	Action* getAppliedAction( Vertex* parent );

	float getVertexValue( int player ) { return vertexValue[player]; }
	int getNumVisits() { return numVisits; }
	
private:
	std::vector<Action*> actions;

	State* state;
	std::vector<Vertex*> children;
	std::vector<ParentData*> parents;
	
	std::vector<float> vertexValue;
	int numVisits;
};

#endif