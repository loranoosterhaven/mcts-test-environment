/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#ifndef NODE_H
#define NODE_H

class Node
{
public:
	Node( State* state, Node* parent, Action* appliedAction );
	~Node();

	Node* expand();
	void update( std::vector<float>* valueEstimate );

	Node* getParent() { return parent; }

	Node* getChild( int child );
	int getNumChildren() { return children.size(); }

	State* getState() { return state; }

	bool isFullyExpanded() { return !actions.empty() && children.size() == actions.size(); }
	bool isTerminal() { return state->isTerminal(); }

	Action* getAppliedAction() { return appliedAction; }

	float getNodeValue( int player ) { return nodeValue[player]; }
	int getNumVisits() { return numVisits; }
	
private:
	Action* appliedAction;

	Node* parent;
	std::vector<Action*> actions;

	State* state;
	std::vector<Node*> children;
	
	std::vector<float> nodeValue;
	int numVisits;
};

#endif