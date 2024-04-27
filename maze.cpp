#include <iostream>
#include <vector>
#include <stack>
#include <random>
#include <algorithm>

class Cell {
public:
    bool visited;
    bool walls[4]; // 0: top, 1: right, 2: bottom, 3: left

    Cell() : visited(false), walls{true, true, true, true} {}
};

class Maze {
private:
    int width, height;
    std::vector<std::vector<Cell>> cells; // 2D vector of cells
    std::default_random_engine rng; // Random number generator

    // Check if the move is valid by checking if the cell is within the maze boundaries
    bool isMoveValid(int x, int y) {
        return x >= 0 && x < width && y >= 0 && y < height;
    }

    // Depth-first search algorithm to generate the maze
    // It basically carves out passages in the maze by removing walls between cells
    void dfs(int x, int y) {
        cells[y][x].visited = true;

        // 0: top, 1: right, 2: bottom, 3: left
        std::vector<int> directions = {0, 1, 2, 3};
        std::shuffle(directions.begin(), directions.end(), rng);

        for (int direction : directions) {
            int nx = x, ny = y;
            switch (direction) {
                case 0: ny--; break; // Move up
                case 1: nx++; break; // Move right
                case 2: ny++; break; // Move down
                case 3: nx--; break; // Move left
            }
            // Check if the move is valid and the cell is not visited
            if (isMoveValid(nx, ny) && !cells[ny][nx].visited) {
                cells[y][x].walls[direction] = false;
                cells[ny][nx].walls[(direction + 2) % 4] = false;
                dfs(nx, ny);
            }
        }
    }

public:
    Maze(int w, int h) : width(w), height(h), cells(h, std::vector<Cell>(w)), rng(std::random_device{}()) {}

    void generateMaze() {
        dfs(0, 0);  // Start the maze from the top-left corner
        // Ensure the entry and exit are open
        cells[0][0].walls[3] = false;        // Open the left wall of the first cell for entry
        cells[height - 1][width - 1].walls[1] = false;  // Open the right wall of the last cell for exit
    }

    void displayMaze() {
        // Display the top boundary
        std::cout << "  +"; // Entry gap
        for (int x = 1; x < width; ++x) std::cout << "--+";
        std::cout << '\n';

        for (int y = 0; y < height; ++y) {
            // Display the left boundary of the maze
            if (y == 0) std::cout << " ";
            else std::cout << "|";

            // Display the passage ways or walls between cells
            for (int x = 0; x < width; ++x) {
                std::cout << "  "; // Passage way
                if (x < width - 1) std::cout << (cells[y][x].walls[1] ? "|" : " ");
                else {
                    if (y == height - 1) std::cout << " "; // Exit gap
                    else std::cout << "|";
                }
            }
            std::cout << '\n';

            // Display the bottom boundary of the cells
            std::cout << "+";
            for (int x = 0; x < width; ++x) {
                if (y < height - 1) std::cout << (cells[y][x].walls[2] ? "--" : "  ");
                else std::cout << "--";
                std::cout << "+";
            }
            std::cout << '\n';
        }
    }
};

int main() {
    int width, height;
    std::cout << "Enter the width of the maze: ";
    std::cin >> width;
    std::cout << "Enter the height of the maze: ";
    std::cin >> height;

    Maze maze(width, height);
    maze.generateMaze();
    maze.displayMaze();

    return 0;
}
