/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#ifndef EXPECTIMINIMAX_H
#define EXPECTIMINIMAX_H

class ExpectiminimaxSearch : public Search
{
public:
	virtual SearchResult* search( State* state );

	void setDepthLimit( int depthLimit ) { this->depthLimit = depthLimit; }

private:
	float chanceValue( HeuristicState* state, int depth, float alpha, float beta );
	float minimaxValue( HeuristicState* state, int depth, float alpha, float beta );
	int agent;
	int depthLimit;
};

#endif