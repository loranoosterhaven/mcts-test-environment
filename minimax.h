/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#ifndef MINIMAX_H
#define MINIMAX_H

class MinimaxSearch : public Search
{
public:
	virtual SearchResult* search( State* state );

private:
	float maxValue( State* state );
	float minValue( State* state );
	int agent;
};

#endif