#include "Cell.h"
#include <queue>
#include <vector>
#include "CompareAStarCells.h"
using namespace std;

#pragma once
class Ghost 
{
private:
	bool isStart;
	bool isDone;
	bool steppedOnCoin = false;
	Cell* cell;
	priority_queue<Cell, vector<Cell>, CompareAStarCells>  aStarPq;
	vector<Cell*> visited;
	vector<Cell*> notVisited;

public:
	~Ghost();
	Ghost(Cell* cell);
	Cell* getCell();
	bool getIsDone();
	bool getIsStart();
	bool getSteppedOnCoin();
	priority_queue<Cell, vector<Cell>, CompareAStarCells>& getPq();
	vector<Cell*>& getVisited();
	vector<Cell*>& getNotVisited();
	void setIsDone(bool isDone);
	void setSteppedOnCoin(bool isCoin);
	void setIsStart(bool newIsStart);
};