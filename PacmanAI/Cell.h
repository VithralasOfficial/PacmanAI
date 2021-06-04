#pragma once
class Cell
{
private:
	int xPos, yPos;
	double h, g, f;
	Cell* parent;

public:
	Cell();
	Cell(int r, int c, double newH, Cell* p);
	Cell(int r, int c, double newH, double newG, Cell* p);
 	int getYPos() { return yPos; }
	int getXPos() { return xPos; }
	double getH() { return h; }
	double getG() { return g; }
	double getF() { return f; }
	void setH(double newH) { h = newH; }
	void setG(double newG) { g = newG; }
	void setF(double newF) { f = newF; }
	void setParentNull();
	void setXPos(int newXPos) { xPos = newXPos; }
	void setYPos(int newYPos) { yPos = newYPos; }
	Cell* getParent() { return parent; }
	bool operator == (const Cell& other);
	bool compareCells(Cell* other);
	Cell* biggerCell(Cell* other);
};