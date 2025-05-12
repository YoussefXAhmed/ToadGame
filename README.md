
# 2D Shooter Game (OpenGL)

A simple 2D shooter game built using OpenGL and C++. The player controls a character and shoots bullets to eliminate enemies. The game includes multiple levels, each with increasing difficulty.

## Features

* **Player Movement**: Use `W`, `A`, `S`, `D` to move the player around.
* **Shooting Mechanism**: The player can shoot bullets in the direction they are facing.
* **Enemy Types**: The game includes different types of enemies such as Cats and Toads, each with varying health and behavior.
* **Levels**: The game has multiple levels that get progressively harder as the player advances.
* **Score**: Keep track of your score as you defeat enemies.
* **Sounds**: Basic sound effects for hitting enemies, killing enemies, and leveling up (for Windows users).
* **Game Over/Win State**: Displays messages for game over or winning the game.

## Controls

* **W**: Move Up
* **A**: Move Left
* **S**: Move Down
* **D**: Move Right
* **Q**: Quit the game
* **N**: Move to the next level (after reaching the required score)

## Requirements

* **Operating System**: Cross-platform (Tested on Windows and Linux)
* **Libraries**:

  * OpenGL
  * GLUT (FreeGLUT)
  * C++11 or later

## Setup Instructions

### 1. Install Dependencies

Before running the game, make sure you have the following dependencies installed:

#### Windows

* Install **FreeGLUT**: [FreeGLUT download](http://freeglut.sourceforge.net/)
* Install a C++ compiler like **MinGW** or **MSVC**.

#### Linux (Ubuntu/Debian)

You can install the necessary libraries with the following commands:

```bash
sudo apt update
sudo apt install freeglut3 freeglut3-dev g++ make
```

### 2. Clone the Repository

Clone the repository to your local machine:

```bash
git clone https://github.com/yourusername/2D-Shooter-Game.git
cd 2D-Shooter-Game
```

### 3. Compile the Game

Once you have the repository, compile the game with the following command:

```bash
g++ -o shooter_game main.cpp -lGL -lGLU -lglut
```

### 4. Run the Game

After compiling, you can run the game with:

```bash
./shooter_game
```

#### Windows:

You can build and run the project in any IDE like **Visual Studio**, or simply compile with **g++** using **MinGW** in the command line.

### 5. Game Instructions

* Move the player with `W`, `A`, `S`, `D`.
* Shoot bullets with the spacebar.
* Defeat enemies to increase your score.
* Reach the required score to move to the next level.

## Gameplay Screenshots

![Screenshot 1](![image](https://github.com/user-attachments/assets/43944560-958f-4463-988a-d1364e829cba)
