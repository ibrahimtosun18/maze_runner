#include <SFML/Graphics.hpp>
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
    std::vector<std::vector<Cell>> cells; // 2D grid of cells
    std::default_random_engine rng; // Random number generator
    sf::Vector2i playerPosition; // Player position in the maze

    // Check if the cell at position (x, y) is a valid move and has not been visited
    bool isMoveValid(int x, int y) {
        return x >= 0 && x < width && y >= 0 && y < height && !cells[y][x].visited;
    }

    // Player movement handling
    bool canMove(int dx, int dy) {
        int newX = playerPosition.x + dx; //
        int newY = playerPosition.y + dy;
        if (newX < 0 || newX >= width || newY < 0 || newY >= height) return false; // Check if the new position is within the maze bounds
        
        // Check if there's a wall in the direction of movement
        if (dx == 1 && cells[playerPosition.y][playerPosition.x].walls[1]) return false; // Right wall
        if (dx == -1 && cells[playerPosition.y][playerPosition.x].walls[3]) return false; // Left wall
        if (dy == 1 && cells[playerPosition.y][playerPosition.x].walls[2]) return false; // Bottom wall
        if (dy == -1 && cells[playerPosition.y][playerPosition.x].walls[0]) return false; // Top wall
        return true;
    }
    // Depth-first search algorithm to generate the maze
    void dfs(int x, int y) {
            cells[y][x].visited = true; // Mark the current cell as visited
    
            std::vector<int> directions = {0, 1, 2, 3}; // 0: top, 1: right, 2: bottom, 3: left
            std::shuffle(directions.begin(), directions.end(), rng); // Randomize the order of directions
    
            for (auto direction : directions) {
                int nx = x, ny = y;
                switch (direction) {
                    case 0: ny--; break; // Move up
                    case 1: nx++; break; // Move right
                    case 2: ny++; break; // Move down
                    case 3: nx--; break; // Move left
                }
                if (isMoveValid(nx, ny) && !cells[ny][nx].visited) {
                    // Remove the walls between the current cell and the chosen cell
                    cells[y][x].walls[direction] = false;
                    cells[ny][nx].walls[(direction + 2) % 4] = false;
                    dfs(nx, ny); // Recursively visit the next cell
                }
            }
        }


public:
    
    Maze(int w, int h) : width(w), height(h), cells(h, std::vector<Cell>(w)), rng(std::random_device{}()) {
        playerPosition = sf::Vector2i(0, 0); // Start position at the maze entry
    }

    void generateMaze() {
        dfs(0, 0);  // Start the maze from the top-left corner
        // Ensure the entry and exit are open
        cells[0][0].walls[3] = false;        // Open the left wall of the first cell for entry
        cells[height - 1][width - 1].walls[1] = false;  // Open the right wall of the last cell for exit
    }
    
    void movePlayer(int dx, int dy) {
        if (canMove(dx, dy)) {
            playerPosition.x += dx;
            playerPosition.y += dy;
        }
    }

    // Draw the maze and player
    void draw(sf::RenderWindow &window) {
        float cellWidth = static_cast<float>(window.getSize().x) / width;
        float cellHeight = static_cast<float>(window.getSize().y) / height;

        sf::RectangleShape wallVertical(sf::Vector2f(4, cellHeight)); // Thicker walls for better visibility
        sf::RectangleShape wallHorizontal(sf::Vector2f(cellWidth, 4));
        wallVertical.setFillColor(sf::Color(70, 60, 120));
        wallHorizontal.setFillColor(sf::Color(70, 60, 120));

        // Draw the maze walls by iterating through each cell
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                // Draw the top wall if it exists
                if (cells[y][x].walls[0]) {
                    wallHorizontal.setPosition(x * cellWidth, y * cellHeight);
                    window.draw(wallHorizontal);
                }
                // Draw the left wall if it exists
                if (cells[y][x].walls[3]) {
                    wallVertical.setPosition(x * cellWidth, y * cellHeight);
                    window.draw(wallVertical);
                }
            }
        }
        // Draw exit indicator
        sf::RectangleShape exitIndicator(sf::Vector2f(cellWidth, cellHeight));
        exitIndicator.setFillColor(sf::Color(0, 110, 233, 213)); // dark blue, semi-transparent
        exitIndicator.setPosition((width - 1) * cellWidth, (height - 1) * cellHeight);
        window.draw(exitIndicator);


        // Draw the bottom and right boundary walls of the maze
        for (int x = 0; x < width; ++x) {
            wallHorizontal.setPosition(x * cellWidth, height * cellHeight);
            window.draw(wallHorizontal);
        }
        for (int y = 0; y < height; ++y) {
            wallVertical.setPosition(width * cellWidth, y * cellHeight);
            window.draw(wallVertical);
        }

    // Draw the player as a red circle
    sf::CircleShape player(std::min(cellWidth, cellHeight) / 2.0f * 0.3f); // Adjust size relative to cell size
    player.setFillColor(sf::Color(222,0,255));
    player.setPosition(playerPosition.x * cellWidth + cellWidth / 2 - player.getRadius(),
                       playerPosition.y * cellHeight + cellHeight / 2 - player.getRadius());
    window.draw(player);

        if (playerPosition.x == width - 1 && playerPosition.y == height - 1) {
            std::cout << "Congratulations! You've reached the exit!" << std::endl;
            window.close(); // Close the window to end the game
        }
    }

    
};

int main() {
    int width, height;
    std::cout << "Enter the width of the maze: ";
    std::cin >> width;
    std::cout << "Enter the height of the maze: ";
    std::cin >> height;

    //add clock to measure time
    sf::Clock clock;

    Maze maze(width, height);
    maze.generateMaze();

    sf::RenderWindow window(sf::VideoMode(800, 600), "Maze Game");
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::Resized) {
                // Adjust the viewport of the window when the window is resized
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }
            else if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::Up:
                        maze.movePlayer(0, -1);
                        break;
                    case sf::Keyboard::Left:
                        maze.movePlayer(-1, 0);
                        break;
                    case sf::Keyboard::Right:
                        maze.movePlayer(1, 0);
                        break;
                    case sf::Keyboard::Down:
                        maze.movePlayer(0, 1);
                        break;
                }
            }
        }

        window.clear(sf::Color::Black);
        maze.draw(window);
        window.display();
    }

    //print time elapsed
    sf::Time elapsed = clock.getElapsedTime();
    std::cout << "Time elapsed: " << elapsed.asSeconds() << " seconds" << std::endl;

    return 0;
}