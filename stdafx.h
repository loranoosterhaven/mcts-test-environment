/*	Monte-Carlo Tree Search Test Environment
	by Loran Oosterhaven
	Copyright University of Groningen 2017 */

#ifndef STDAFX_H
#define STDAFX_H

#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <algorithm>
#include <ctime>
#include <limits>
#include <algorithm>
#include <thread>
#include <map>

#include "state.h"
#include "node.h"
#include "vertex.h"
#include "search.h"
#include "mcts.h"
#include "transpositionmcts.h"
#include "minimax.h"
#include "depthlimitedminimax.h"
#include "expectiminimax.h"
#include "randomsearch.h"

#include "tictactoestate.h"
#include "backgammonstate.h"
#include "nimstate.h"

#endif