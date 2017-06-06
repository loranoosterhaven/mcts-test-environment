/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#ifndef SEARCH_H
#define SEARCH_H

class SearchResult
{
public:
	~SearchResult()
	{
		if( bestAction != NULL )
			delete bestAction;

		if( bestState != NULL )
			delete bestState;
	}

	void print();

public:
	Action* bestAction;
	State* bestState;
};

class Search
{
public:
	virtual SearchResult* search( State* state ) = 0;
};

#endif