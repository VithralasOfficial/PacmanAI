#include "Ghost.h"

Ghost::~Ghost()
{
	free(this->cell);
	this->visited.clear();
	this->notVisited.clear();
	while (!this->aStarPq.empty())
		this->aStarPq.pop();
}

Ghost::Ghost(Cell* cell)
{
	this->cell = cell;
	this->isDone = false;
	this->isStart = true;
}

Cell* Ghost::getCell()
{
	return this->cell;
}

bool Ghost::getIsDone()
{
	return this->isDone;
}

bool Ghost::getIsStart()
{
	return this->isStart;
}

bool Ghost::getSteppedOnCoin()
{
	return this->steppedOnCoin;
}

priority_queue<Cell, vector<Cell>, CompareAStarCells>& Ghost::getPq()
{
	return this->aStarPq;
}

vector<Cell*>& Ghost::getVisited()
{
	return this->visited;
}

vector<Cell*>& Ghost::getNotVisited()
{
	return this->notVisited;
}

void Ghost::setIsDone(bool isDone)
{
	this->isDone = isDone;
}

void Ghost::setSteppedOnCoin(bool isCoin)
{
	this->steppedOnCoin = isCoin;
}

void Ghost::setIsStart(bool newIsStart)
{
	this->isStart = newIsStart;
}