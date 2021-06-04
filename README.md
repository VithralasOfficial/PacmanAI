# PacmanAI
 A pacman based game running A* and BFS algorithms.
 
 Color Sheet:
  Walls - Brown Color
  Spaces (free blocks) - White Color
  Pacman - Red Color
  Ghosts - Light Blue Color
  Coins - Grey Color
  
  Pacman:
  
  Pacman has two modes which he operates in: Coin search mode and Escape mode.
  Pacman decides on which mode to operate by checking his distance from each ghost.
  Inside Pacman.h there is a privte attribute that defines the minimum range pacman will tolerate from a ghost.
  While no ghost gets closer than the range presented in that attribute Pacman will run in Coin search mode.
  If a ghost gets closer than the range presented in that attribute Pacman will run in Escape mode.
  
  Coin search mode:
    In Coin search mode Pacman will run a BFS algorithm to find the closest Coin and move towards it.
    In that mode Pacman only cares about getting coins.
    
   Escape mode:
    In Escape mode Pacman will run reverse A* algorithm (The comparator is reversed [from the one in the priority queue of the ghosts]) in the priority queue to the ghost who
    was found closest to Pacman (by Manhattan Distance), searching for the longest route to that ghost.
    That will make pacman run in the opposite direction from the closest ghost chasing him.
    If any ghost gets closer than that ghost mid run, Pacman will run the reversed A* to that ghost instead.
    In that mode Pacman only cares about survival and doesnt try to find Coins (he still collects them on the way if found mid escape).
    
    Ghosts:
     The ghosts have only one goal and it is to catch Pacman.
     Each ghost runs A* algorithm towards Pacman.
     
     The program can be run either with multi threads or by turn (each ghost takes its turn and calculates the movement and then Pacman does so as well).
     Read the instructions in Idle function to change between modes.
     When in multi threaded mode, each ghost will run in a differend thread, while the main thread will run Pacman's calculations.
