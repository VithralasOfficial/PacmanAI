#include "Pacman.h"

Pacman::~Pacman()
{
	free(this->cell);
	this->BFSCells.clear();
	this->visited.clear();
	this->notVisited.clear();
	while (!this->escapePq.empty())
		this->escapePq.pop();
}

Pacman::Pacman(Cell* cell)
{
	this->cell = cell;
}

Cell* Pacman::getCell()
{
	return this->cell;
}

int Pacman::getMinimumGhostRange()
{
	return this->minimumGhostRange;
}

int Pacman::getNumOfCoins()
{
	return this->numOfCoins;
}

bool Pacman::getBFSDone()
{
	return this->BFSDone;
}

bool Pacman::getEscapeDone()
{
	return this->escapeDone;
}

bool Pacman::getIsStart()
{
	return this->isStart;
}

vector<Cell*>& Pacman::getVisited()
{
	return this->visited;
}

vector<Cell*>& Pacman::getNotVisited()
{
	return this->notVisited;
}

vector<Cell*>& Pacman::getBFSCells()
{
	return this->BFSCells;
}

priority_queue<Cell, vector<Cell>, CompareAStarPacman>& Pacman::getEscapePq()
{
	return this->escapePq;
}

void Pacman::addCoin()
{
	this->numOfCoins++;
}

void Pacman::setBFSDone(bool isDone)
{
	this->BFSDone = isDone;
}

void Pacman::setIsStart(bool newIsStart)
{
	this->isStart = newIsStart;
}

void Pacman::setEscapeDone(bool isDone)
{
	this->escapeDone = isDone;
}
