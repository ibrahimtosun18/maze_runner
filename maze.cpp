#include <iostream>
#include <vector>
#include <stack>
#include <random>
#include <algorithm>
#include <SFML/Graphics.hpp>

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

    void draw(sf::RenderWindow &window) {
        float cellWidth = static_cast<float>(window.getSize().x) / width;
        float cellHeight = static_cast<float>(window.getSize().y) / height;
        sf::RectangleShape wallVertical(sf::Vector2f(4, cellHeight)); // Slightly thicker wall
        sf::RectangleShape wallHorizontal(sf::Vector2f(cellWidth, 4)); // Slightly thicker wall
        wallVertical.setFillColor(sf::Color::Black); // Set wall color to black
        wallHorizontal.setFillColor(sf::Color::Black); // Set wall color to black

        // Draw the vertical walls
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (cells[y][x].walls[1]) { // Right wall
                    wallVertical.setPosition((x + 1) * cellWidth - wallVertical.getSize().x / 2, y * cellHeight);
                    window.draw(wallVertical);
                }
                if (x == 0) { // Left wall
                    wallVertical.setPosition(0, y * cellHeight);
                    window.draw(wallVertical);
                }
            }
        }

        // Draw the horizontal walls
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                if (cells[y][x].walls[2]) { // Bottom wall
                    wallHorizontal.setPosition(x * cellWidth, (y + 1) * cellHeight - wallHorizontal.getSize().y / 2);
                    window.draw(wallHorizontal);
                }
                if (y == 0) { // Top wall
                    wallHorizontal.setPosition(x * cellWidth, 0);
                    window.draw(wallHorizontal);
                }
            }
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

    sf::RenderWindow window(sf::VideoMode(800, 600), "Maze Game");
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::Resized) {
                // Adjust the viewport of the window when the window is resized
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }
        }

        window.clear(sf::Color::White);
        maze.draw(window);
        window.display();
    }

    return 0;
}
