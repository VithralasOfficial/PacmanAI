#pragma once
#include "Cell.h"
class CompareBFSCells
{
public:
	bool operator () (Cell c1, Cell c2) { return c1.getH() > c2.getH(); }
};
