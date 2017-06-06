/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#ifndef DEPTHLIMITEDMINIMAX_H
#define DEPTHLIMITEDMINIMAX_H

class DepthLimitedMinimaxSearch : public Search
{
public:
	virtual SearchResult* search( State* state );

	void setDepthLimit( int depthLimit ) { this->depthLimit = depthLimit; }

private:
	float maxValue( HeuristicState* state, int depth );
	float minValue( HeuristicState* state, int depth );
	int agent;
	int depthLimit;
};

#endif