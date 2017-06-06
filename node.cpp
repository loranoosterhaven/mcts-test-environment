/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#include "stdafx.h"

Node::Node( State* state, Node* parent, Action* appliedAction )
{
	this->state = state;
	this->parent = parent;
	this->appliedAction = appliedAction;

	for( int i = 0; i < state->getNumPlayers(); i++ )
		nodeValue.push_back( 0.0f );

	numVisits = 0;
}

Node::~Node()
{
	if( appliedAction != NULL )
		delete appliedAction;

	actions.clear();

	for (std::vector<Node*>::iterator it = children.begin(); it != children.end(); it++ )
		delete *it;

	children.clear();

	delete state;
}

Node* Node::expand()
{
	if( actions.empty() )
	{
		state->computeActions( &actions );
		std::random_shuffle( actions.begin(), actions.end() );
	}

	Action* nextAction = actions[children.size()];

	State* nodeState = state->clone();
	nodeState->transitionModel( nextAction );

	Node* child = new Node( nodeState, this, nextAction );
	children.push_back( child );

	return child;
}

void Node::update( std::vector<float>* deltaValue )
{
	numVisits++;

	for( int i = 0; i < state->getNumPlayers(); i++ )
		nodeValue[i] += ( *deltaValue )[i];
}

Node* Node::getChild( int child )
{
	return ( child >= 0 && child < ( int )children.size() )? children[child] : NULL;
}