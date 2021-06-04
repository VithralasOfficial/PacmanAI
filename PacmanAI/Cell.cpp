#include "Cell.h"
#include <stdlib.h>

Cell::Cell()
{
	xPos = 0;
	yPos = 0;
	h = 1000;
	g = 1000;
	f = 0;
	parent = nullptr;
}

Cell::Cell(int r, int c, double newH, Cell* p)
{
	xPos = r;
	yPos = c;
	h = newH;
	g = 1000;
	f = 0;
	parent = p;

}

Cell::Cell(int r, int c, double newH, double newG, Cell* p)
{
	xPos = r;
	yPos = c;
	h = newH;
	g = newG;
	f = 0;
	parent = p;

}

void Cell::setParentNull()
{
	this->parent = nullptr;
}

bool Cell::operator== (const Cell& other)
{
	return (this->xPos == other.xPos && this->yPos == other.yPos);
}

bool Cell::compareCells(Cell* other)
{
	if (this->xPos == other->getXPos() && this->yPos == other->getYPos())
		return true;
	return false;
}

Cell* Cell::biggerCell(Cell* other)
 {
	if (this->f > other->getF())
		return other;
	return this;
}
