/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#ifndef RANDOMSEARCH_H
#define RANDOMSEARCH_H

class RandomSearch : public Search
{
public:
	virtual SearchResult* search( State* state );
};

#endif