#include "glut.h"
#include "Cell.h"
#include "Ghost.h"
#include "CompareBFSCells.h"
#include "CompareAStarCells.h"
#include "CompareAStarPacman.h"
#include <time.h>
#include <vector>
#include <iostream>
#include <queue>
#include <algorithm>
#include <thread>
#include <mutex>
#include "Pacman.h"
using namespace std;

const int W = 600;
const int H = 600;
const int WALL = 1;
const int SPACE = 0;
const int GHOST = 2;
const int PACMAN = 3;
const int COIN = 4;

const int MSZ = 70;

int maze[MSZ][MSZ] = { 0 };
int pacmanCoords[2];
int targetEscape[2];

bool drawCoinsFlag = true;

bool runBFS = false;
bool runAStar = false;
bool aStarComplete = false;
bool gameOver = true;
bool runEscape = false;

bool threadUp = false;

mutex mute;

Ghost* firstGhost;
Ghost* secGhost;
Ghost* thirdGhost;
Pacman* pacman;

void InitMaze();
double distanceFromGhost(int x1, int y1, Ghost* ghost);
double manhattanDistance(int x1, int y1);
void restoreEscapePath(Cell* cell);

void init()
{
	srand(time(0));
	glClearColor(1, 1, 1, 0);// color of window background
	glOrtho(-1, 1, -1, 1, 0, 1);

	InitMaze();
}

void InitMaze()
{
	int i, j;

	for (i = 0; i < MSZ; i++) // frame of WALLS
	{
		maze[0][i] = WALL;
		maze[MSZ - 1][i] = WALL;
		maze[i][0] = WALL;
		maze[i][MSZ - 1] = WALL;
	}

	for (i = 1; i < MSZ - 1; i++)
		for (j = 1; j < MSZ - 1; j++)
		{
			if (i % 2 == 1)
			{
				if (rand() % 100 > 15)
					maze[i][j] = SPACE;
				else
					maze[i][j] = WALL;
			}
			else // the line is even
			{
				if (rand() % 100 > 35)
					maze[i][j] = WALL;
				else
					maze[i][j] = SPACE;
			}
		}

	for (int i = 1; i < MSZ - 2; i++)
		for (int j = 1; j < MSZ - 2; j++)
		{
			if (rand() % 100 > 92)
				maze[i][j] = COIN;
		}

	// Init Pacman in the middle of the maze.
	pacman = new Pacman(new Cell(MSZ / 2, MSZ / 2, 0, nullptr));
	pacman->getBFSCells().push_back(pacman->getCell());
	pacman->getEscapePq().push(*pacman->getCell());
	maze[MSZ / 2][MSZ / 2] = PACMAN;
	
	// Create First Ghost.
	firstGhost = new Ghost(new Cell((rand() % (MSZ - 1) + 1), (rand() % (MSZ - 1) + 1), 0, nullptr));
	firstGhost->getPq().push(*(firstGhost->getCell()));
	firstGhost->getNotVisited().push_back(firstGhost->getCell());
	maze[firstGhost->getCell()->getXPos()][firstGhost->getCell()->getYPos()] = GHOST;
	
	// Create Second Ghost.
	secGhost = new Ghost(new Cell((rand() % (MSZ - 1) + 1), (rand() % (MSZ - 1) + 1), 0, nullptr));
	secGhost->getPq().push(*(secGhost->getCell()));
	secGhost->getNotVisited().push_back(secGhost->getCell());
	maze[secGhost->getCell()->getXPos()][secGhost->getCell()->getYPos()] = GHOST;

	// Create Third Ghost.
	thirdGhost = new Ghost(new Cell(rand() % (MSZ - 1) + 1, rand() % (MSZ - 1) + 1, 0, nullptr));
	thirdGhost->getPq().push(*(thirdGhost->getCell()));
	thirdGhost->getNotVisited().push_back(thirdGhost->getCell());
	maze[thirdGhost->getCell()->getXPos()][thirdGhost->getCell()->getYPos()] = GHOST;
}

void DrawMaze()
{
	int i, j;
	int xsz = W / MSZ;
	int ysz = H / MSZ;
	double x, y;
	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			switch (maze[i][j])   // set color
			{
			case SPACE: // White
				glColor3d(0.9, 0.9, 0.9);
				break;
			case WALL: // Brown
				glColor3d(0.3, 0, 0);
				break;
			case GHOST: // Dark Blue
				glColor3d(0, 0.5, 1);
				break;
			case PACMAN: // Dark Yellow
				glColor3d(1, 0, 0);
				break;
			case COIN: // Grey
				glColor3d(0.5, 0.5, 0.5);
				break;
			}
			// draw square
			x = 2.0 * (j * xsz) / W - 1; // value in range [-1,1)
			y = 2.0 * (i * ysz) / H - 1; // value in range [-1,1)
			glBegin(GL_POLYGON);
			glVertex2d(x, y);
			glVertex2d(x, y + (2.0 * ysz) / H);
			glVertex2d(x + 2.0 * xsz / W, y + (2.0 * ysz) / H);
			glVertex2d(x + 2.0 * xsz / W, y);
			glEnd();
		}
}


// Calculate the H for a BFS run.
double distanceFromGhost(int x1, int y1, Ghost* ghost)
{
	return sqrt((x1 - ghost->getCell()->getXPos()) * (x1 - ghost->getCell()->getXPos()) 
		+ (y1 - ghost->getCell()->getYPos()) * (y1 - ghost->getCell()->getYPos()));
}

// Calculate the H for an A* run for a ghost.
double manhattanDistance(int x1, int y1)
{
	return (abs(x1 - pacman->getCell()->getXPos()) + abs(y1 - pacman->getCell()->getYPos()));
}

// Calculate the H for an A* run for pacman.
double pacmanManhattanDistance(int x1, int y1, Ghost* ghost)
{
	return (abs(x1 - ghost->getCell()->getXPos()) + abs(y1 - ghost->getCell()->getYPos()));
}

// Ghost Related Section.
// Reset The Values of each ghost before starting a new run.
// Only occurs after a path was found.
void resetAStar(Ghost* ghost)
{
	while (!ghost->getPq().empty())
		ghost->getPq().pop();
	ghost->getVisited().clear();
	ghost->getNotVisited().clear();
	ghost->setIsDone(false);
	ghost->setIsStart(true);

	ghost->getPq().push(*ghost->getCell());
	ghost->getNotVisited().push_back(ghost->getCell());
	ghost->getCell()->setParentNull();
}

// Restore the path towards pacman in an A* run and move forward.
void restoreGhostPath(Cell* c, Ghost* ghost)
{
	// Restore Path.
	if (c->getParent() != nullptr && maze[c->getXPos()][c->getYPos()] != GHOST)
	{
		while (maze[c->getParent()->getXPos()][c->getParent()->getYPos()] != GHOST)
		{

			c = c->getParent();
			if (c->getParent() == nullptr)
			{
				mute.lock();
				runAStar = false;
				mute.unlock();
				ghost->setIsDone(true);
				return;
			}
		}
		if (ghost->getSteppedOnCoin())
		{
			maze[c->getParent()->getXPos()][c->getParent()->getYPos()] = COIN;
			ghost->setSteppedOnCoin(false);
		}
		else
		{
			maze[c->getParent()->getXPos()][c->getParent()->getYPos()] = SPACE;
		}
		if (maze[c->getXPos()][c->getYPos()] == COIN)
			ghost->setSteppedOnCoin(true);
		maze[c->getXPos()][c->getYPos()] = GHOST;
		ghost->getCell()->setYPos(c->getYPos());
		ghost->getCell()->setXPos(c->getXPos());
	}
}

// Checks the neighbor status of a cell at a given point and change it accordingly on an A* run For A Ghost.
void checkAStarNeighbor(Cell* cell, Ghost* ghost, int xPos, int yPos)
{
	vector<Cell*>::iterator visitIter;
	vector<Cell*>::iterator notVisitIter;
	if (maze[xPos][yPos] == SPACE || maze[xPos][yPos] == COIN)
	{
		Cell* newCell = new Cell(xPos, yPos, manhattanDistance(xPos, yPos), cell->getG() + 1, cell);
		newCell->setF(newCell->getG() + newCell->getH());
		for (visitIter = ghost->getVisited().begin(); visitIter < ghost->getVisited().end(); ++visitIter)
			if (newCell->compareCells(*visitIter))
				break;
		for (notVisitIter = ghost->getNotVisited().begin(); notVisitIter < ghost->getNotVisited().end(); ++notVisitIter)
		{
			if (newCell->compareCells(*notVisitIter))
				break;
		}
		if (visitIter == ghost->getVisited().end() && notVisitIter == ghost->getNotVisited().end())
		{
			ghost->getPq().push(*newCell);
			ghost->getNotVisited().push_back(newCell);
		}
	}
	else if (maze[xPos][yPos] == PACMAN)
	{
		//cout << "Solution Found!\n";
		mute.lock();
		runAStar = false;
		mute.unlock();
		ghost->setIsDone(true);
		restoreGhostPath(cell, ghost);
	}
}

// Check all neighbors of a given cell on an A* run For A Ghost.
void checkAStarNeighbors(Cell* cell, Ghost* ghost)
{
	checkAStarNeighbor(cell, ghost, cell->getXPos() + 1, cell->getYPos()); // Down
	if (runAStar)
		checkAStarNeighbor(cell, ghost, cell->getXPos() - 1, cell->getYPos()); // Up
	if (runAStar)
		checkAStarNeighbor(cell, ghost, cell->getXPos(), cell->getYPos() + 1); // Right
	if (runAStar)
		checkAStarNeighbor(cell, ghost, cell->getXPos(), cell->getYPos() - 1); // Left
}

// Run one iteration of A* For a Ghost.
void aStarIteration(Ghost* ghost)
{
	vector<Cell*>::iterator notVisit;
	Cell* pCurrent = nullptr; // Current cell.
	if (ghost->getPq().empty())
	{
		cout << "Ghost Is Stuck!\n";
		ghost->setIsDone(true);
		mute.lock();
		runAStar = false;
		mute.unlock();
		return;
	}
	else
	{
		if (maze[ghost->getCell()->getXPos() + 1][ghost->getCell()->getYPos()] == PACMAN ||
			maze[ghost->getCell()->getXPos()][ghost->getCell()->getYPos() + 1] == PACMAN ||
			maze[ghost->getCell()->getXPos() - 1][ghost->getCell()->getYPos()] == PACMAN ||
			maze[ghost->getCell()->getXPos()][ghost->getCell()->getYPos() - 1] == PACMAN)
		{
			cout << "Pacman DEAD!\n";
			gameOver = true;
			ghost->setIsDone(true);
			mute.lock();
			runAStar = false;
			mute.unlock();
			return;
		}
		pCurrent = new Cell(ghost->getPq().top());
		ghost->getPq().pop();
		if (ghost->getIsStart())
		{
			pCurrent->setH(manhattanDistance(pCurrent->getXPos(), pCurrent->getYPos()));
			pCurrent->setF(0);
			ghost->setIsStart(false);
		}
		if (maze[pCurrent->getXPos()][pCurrent->getYPos()] == PACMAN)
		{
			mute.lock();
			runAStar = false;
			mute.unlock();
			ghost->setIsDone(true);
			restoreGhostPath(pCurrent, ghost);
			return;
		}
		else
		{
			ghost->getVisited().push_back(pCurrent);
			for (notVisit = ghost->getNotVisited().begin(); notVisit < ghost->getNotVisited().end(); ++notVisit)
				if (pCurrent->compareCells(*notVisit))
					break;
			if (notVisit != ghost->getNotVisited().end())
			{
				ghost->getNotVisited().erase(notVisit);
			}
			checkAStarNeighbors(pCurrent, ghost);
		}
	}
}

// Pacman Related Section.
// Check who is the closest ghost to pacman and that it is not closer than the minimum range allowed.
// minimum range can be adjusted in "Pacman.h".
Ghost* checkDistanceFromGhosts()
{
	Ghost* ghost = firstGhost;
	double minDistance = 0, tempDistance = 0;
	minDistance = distanceFromGhost(pacman->getCell()->getXPos(), pacman->getCell()->getYPos(), firstGhost);
	tempDistance = distanceFromGhost(pacman->getCell()->getXPos(), pacman->getCell()->getYPos(), secGhost);
	if (tempDistance < minDistance)
	{
		minDistance = tempDistance;
		ghost = secGhost;
	}
	tempDistance = distanceFromGhost(pacman->getCell()->getXPos(), pacman->getCell()->getYPos(), thirdGhost);
	if (tempDistance < minDistance)
	{
		minDistance = tempDistance;
		ghost = thirdGhost;
	}
	if (minDistance < pacman->getMinimumGhostRange())
		return ghost;
	return nullptr;
}

// Checks the neighbor status of a cell at a given point and change it accordingly on an A* run For A Ghost.
void pacmanAStarNeighbor(Cell* cell, Ghost* ghost, int xPos, int yPos)
{
	vector<Cell*>::iterator visitIter;
	vector<Cell*>::iterator notVisitIter;

	if (maze[xPos][yPos] == SPACE || maze[xPos][yPos] == COIN)
	{
		Cell* newCell = new Cell(xPos, yPos, pacmanManhattanDistance(xPos, yPos, ghost), cell->getG() + 1, cell);
		newCell->setF(newCell->getG() + newCell->getH());
		for (visitIter = pacman->getVisited().begin(); visitIter < pacman->getVisited().end(); ++visitIter)
			if (newCell->compareCells(*visitIter))
				break;
		for (notVisitIter = pacman->getNotVisited().begin(); notVisitIter < pacman->getNotVisited().end(); ++notVisitIter)
		{
			if (newCell->compareCells(*notVisitIter))
				break;
		}
		if (visitIter == pacman->getVisited().end() && notVisitIter == pacman->getNotVisited().end())
		{
			pacman->getEscapePq().push(*newCell);
			pacman->getNotVisited().push_back(newCell);
		}
	}
	else if (maze[xPos][yPos] == GHOST)
	{
		//cout << "Solution Found!\n";
		runEscape = false;
		pacman->setEscapeDone(true);
		restoreEscapePath(cell);
	}
}

// Check neighbors for pacman.
void checkPacmanNeighbors(Cell* cell, Ghost* ghost) 
{
	pacmanAStarNeighbor(cell, ghost, cell->getXPos() + 1, cell->getYPos()); // Down
	if (runEscape)
		pacmanAStarNeighbor(cell, ghost, cell->getXPos() - 1, cell->getYPos()); // Up
	if (runEscape)
		pacmanAStarNeighbor(cell, ghost, cell->getXPos(), cell->getYPos() + 1); // Right
	if (runEscape)
		pacmanAStarNeighbor(cell, ghost, cell->getXPos(), cell->getYPos() - 1); // Left
}

// Run one iteration of A* For pacman.
void pacmanAStarIteration(Ghost* ghost)
{
	vector<Cell*>::iterator notVisit;
	Cell* pacmanCurrent = nullptr; // Current cell.
	if (pacman->getEscapePq().empty())
	{
		cout << "Pacman No Solution!\n";
		pacman->setEscapeDone(true);
		runEscape = false;
		return;
	}
	else
	{
		if (maze[pacman->getCell()->getXPos() + 1][pacman->getCell()->getYPos()] == GHOST ||
			maze[pacman->getCell()->getXPos()][pacman->getCell()->getYPos() + 1] == GHOST ||
			maze[pacman->getCell()->getXPos() - 1][pacman->getCell()->getYPos()] == GHOST ||
			maze[pacman->getCell()->getXPos()][pacman->getCell()->getYPos() - 1] == GHOST)
		{
			cout << "Pacman DEAD!\n";
			gameOver = true;
			pacman->setEscapeDone(true);
			runEscape = false;
			return;
		}
		pacmanCurrent = new Cell(pacman->getEscapePq().top());
		pacman->getEscapePq().pop();
		if (pacman->getIsStart())
		{
			pacmanCurrent->setH(pacmanManhattanDistance(pacmanCurrent->getXPos(), pacmanCurrent->getYPos(), ghost));
			pacmanCurrent->setF(0);
			pacman->setIsStart(false);
		}
		if (maze[pacmanCurrent->getXPos()][pacmanCurrent->getYPos()] == GHOST)
		{
			runEscape = false;
			pacman->setEscapeDone(true);
			restoreEscapePath(pacmanCurrent);
			return;
		}
		else
		{
			pacman->getVisited().push_back(pacmanCurrent);
			for (notVisit = pacman->getNotVisited().begin(); notVisit < pacman->getNotVisited().end(); ++notVisit)
				if (pacmanCurrent->compareCells(*notVisit))
					break;
			if (notVisit != pacman->getNotVisited().end())
			{
				pacman->getNotVisited().erase(notVisit);
			}
			checkPacmanNeighbors(pacmanCurrent, ghost);
		}
	}
}

// Restore the path in pacman's A* run to escape a ghost.
void restoreEscapePath(Cell* cell)
{
	if (maze[cell->getXPos()][cell->getYPos()] != PACMAN)
	{
		while (maze[cell->getParent()->getXPos()][cell->getParent()->getYPos()] != PACMAN)
		{
			cell = cell->getParent();
		}
		if (maze[cell->getXPos()][cell->getYPos()] == COIN)
		{
			pacman->addCoin();
			cout << "Coins Collected: " << pacman->getNumOfCoins() << '\n';
		}
		maze[cell->getParent()->getXPos()][cell->getParent()->getYPos()] = SPACE;
		maze[cell->getXPos()][cell->getYPos()] = PACMAN;
		pacman->getCell()->setXPos(cell->getXPos());
		pacman->getCell()->setYPos(cell->getYPos());
	}
}

// Restore the path for a BFS run and move one step forward.
void restoreBFSPath(Cell* pcurrent)
{
	if (maze[pcurrent->getXPos()][pcurrent->getYPos()] != PACMAN)
	{
		while (maze[pcurrent->getParent()->getXPos()][pcurrent->getParent()->getYPos()] != PACMAN)
		{
			pcurrent = pcurrent->getParent();
		}
		if (maze[pcurrent->getXPos()][pcurrent->getYPos()] == COIN)
		{
			pacman->addCoin();
			cout << "Coins Collected: " << pacman->getNumOfCoins() << '\n';
		}
		maze[pcurrent->getParent()->getXPos()][pcurrent->getParent()->getYPos()] = SPACE;
		maze[pcurrent->getXPos()][pcurrent->getYPos()] = PACMAN;
		pacman->getCell()->setXPos(pcurrent->getXPos());
		pacman->getCell()->setYPos(pcurrent->getYPos());
		pacman->setBFSDone(true);
	}
}

// Reset the movement before starting a new search to a close ghost.
void resetEscape()
{
	pacman->getNotVisited().clear();
	pacman->getVisited().clear();
	pacman->setEscapeDone(false);
	while (!pacman->getEscapePq().empty())
		pacman->getEscapePq().pop();
	pacman->getEscapePq().push(*pacman->getCell());
	pacman->setIsStart(true);
	pacman->getCell()->setParentNull();
}

// BFS (Search Coins) when range from ghosts is safe (range from closest ghost is > minimum range in pacman). 
void resetBFS()
{
	pacman->getBFSCells().clear();
	pacman->setBFSDone(false);
	pacman->getBFSCells().push_back(pacman->getCell());
	pacman->getCell()->setParentNull();
}

// check pacman neighbors in BFS iteration.
void CheckBFSNeighbor(Cell* pcurrent, int row, int col)
{
	vector<Cell*>::iterator visitIter;
	// check the color of the neighbor cell
	if (maze[row][col] == SPACE || maze[row][col] == COIN)
	{
		Cell* pc = new Cell(row, col, 0, pcurrent);
		if (maze[row][col] == COIN) // the solution has been found
		{
			restoreBFSPath(pc);
			runBFS = false;
		}
		else // it is space neighbor, push it into the vector.
		{
			for (visitIter = pacman->getBFSCells().begin(); 
				visitIter < pacman->getBFSCells().end(); ++visitIter)
				if (pc->compareCells(*visitIter))
					break;
			if (visitIter == pacman->getBFSCells().end())
				pacman->getBFSCells().push_back(pc);
		}
	}
}

// Run a BFS iteration (runs when pacman searches for coins meaning no close ghosts).
void BFSIteration()
{
	Cell* pcurrent;

	if (pacman->getBFSCells().empty())
	{
		cout << "Pacman There is no solution\n";
		runBFS = false;
	}
	else
	{
		pcurrent = pacman->getBFSCells().front();  // save the FIRST element,
		pacman->getBFSCells().erase(pacman->getBFSCells().begin()); // remove it from the queue

		// now check the neighbors
		CheckBFSNeighbor(pcurrent, pcurrent->getXPos() + 1, pcurrent->getYPos());
		if (runBFS)
			CheckBFSNeighbor(pcurrent, pcurrent->getXPos() - 1, pcurrent->getYPos());
		if (runBFS)
			CheckBFSNeighbor(pcurrent, pcurrent->getXPos(), pcurrent->getYPos() + 1);
		if (runBFS)
			CheckBFSNeighbor(pcurrent, pcurrent->getXPos(), pcurrent->getYPos() - 1);
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer
	DrawMaze();
	glutSwapBuffers(); // show all
}

// Ghost run.
void initGhost(Ghost* ghost)
{
		while (!ghost->getIsDone())
			aStarIteration(ghost);
		if (ghost->getIsDone())
		{
			resetAStar(ghost);
			mute.lock();
			runAStar = true;
			mute.unlock();
		}
}

// Pacman run.
void initPacman()
{
	Ghost* ghost = checkDistanceFromGhosts();
	if (ghost != nullptr)
	{
		if (!pacman->getEscapePq().empty())
		{
			resetEscape();
		}
		while (!pacman->getEscapeDone())
		{
			pacmanAStarIteration(ghost);
		}
		if (pacman->getEscapeDone())
		{
			runEscape = true;
			resetEscape();
		}
	}
	else
	{
		while (!pacman->getBFSDone())
		{
			BFSIteration();
		}
		if (pacman->getBFSDone())
		{
			runBFS = true;
			resetBFS();
		}
	}
}

void idle()
{
	if (!gameOver)
	{
		if (!threadUp && runAStar)
		{
			threadUp = true;
			thread t1(initGhost, firstGhost);
			thread t2(initGhost, secGhost);
			thread t3(initGhost, thirdGhost);
			initPacman();
			t1.join();
			t2.join();
			t3.join();
		}
		threadUp = false;

		// To run without threads first comment lines 646-657.
		// Uncomment lines 662-665 to run without threads.

		//initGhost(firstGhost);
		//initGhost(secGhost);
		//initGhost(thirdGhost);
		//initPacman();

		glutPostRedisplay(); // go to display
	}
}

void menu(int choice)
{
	switch (choice)
	{
	case 0: // Start Game
		runAStar = true;
		runBFS = true;
		runEscape = true;
		gameOver = false;
		break;
	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("First Example");

	glutDisplayFunc(display); // display is window refresh function
	glutIdleFunc(idle);  // idle is background function

	// menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Start Game", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	init();

	glutMainLoop();
}