#include "Cell.h"
#include <queue>
#include <vector>
#include "CompareBFSCells.h"
#include "CompareAStarPacman.h"
using namespace std;

#pragma once
class Pacman 
{
private: 
	Cell* cell;
	int minimumGhostRange = 10;
	int numOfCoins = 0;
	vector<Cell*> BFSCells;
	vector<Cell*> visited;
	vector<Cell*> notVisited;
	priority_queue<Cell, vector<Cell>, CompareAStarPacman> escapePq;
	bool BFSDone = false;
	bool escapeDone = false;
	bool isStart = true;

public:
	~Pacman();
	Pacman(Cell* cell);
	Cell* getCell();
	int getMinimumGhostRange();
	int getNumOfCoins();
	bool getBFSDone();
	bool getEscapeDone();
	bool getIsStart();
	vector<Cell*>& getVisited();
	vector<Cell*>& getNotVisited();
	vector<Cell*>& getBFSCells();
	priority_queue<Cell, vector<Cell>, CompareAStarPacman>& getEscapePq();

	void addCoin();
	void setBFSDone(bool isDone);
	void setIsStart(bool newIsStart);
	void setEscapeDone(bool isDone);
};