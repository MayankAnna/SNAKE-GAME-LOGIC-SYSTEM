#include <iostream>
#include <conio.h>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <windows.h>

using namespace std;

// Board dimensions
const int WIDTH = 30;
const int HEIGHT = 15;

// Snake nod
struct Node
{
    int x;
    int y;
    Node* next;
};

// Linked-list pointers
Node* head = NULL;
Node* tail = NULL;

// Food and score
int foodX;
int foodY;
int score = 0;

// Current direction
char direction = 'D';

// Game status
bool gameOver = false;

// Simple pseudo-random number generator
unsigned int randomState = 12345u;


// Check whether a position belongs to the snake
// ignoreTail is true when the tail is about to move away
bool isSnakePosition(int x, int y, bool ignoreTail = false)
{
    Node* current = head;

    while (current != NULL)
    {
        if (current->x == x && current->y == y)
        {
            if (ignoreTail && current == tail)
            {
                return false;
            }

            return true;
        }

        current = current->next;
    }

    return false;
}


// Insert a new node at the head
void addHead(int x, int y)
{
    Node* newNode = new Node;

    newNode->x = x;
    newNode->y = y;
    newNode->next = head;

    head = newNode;

    // If the snake was empty, this node is also the tail
    if (tail == NULL)
    {
        tail = newNode;
    }
}


// Delete the last node
void removeTail()
{
    if (head == NULL)
    {
        return;
    }

    // Only one node exists
    if (head == tail)
    {
        delete head;
        head = NULL;
        tail = NULL;
        return;
    }

    Node* current = head;

    // Find the node just before the tail
    while (current->next != tail)
    {
        current = current->next;
    }

    delete tail;

    tail = current;
    tail->next = NULL;
}


// Delete every node before restarting or exiting
void clearSnake()
{
    while (head != NULL)
    {
        Node* temp = head;
        head = head->next;
        delete temp;
    }

    tail = NULL;
}


// Generate food away from the snake and walls
void generateFood()
{
    do
    {
        randomState =
            randomState * 1103515245u + 12345u;

        foodX = (randomState % (WIDTH - 2)) + 1;

        randomState =
            randomState * 1103515245u + 12345u;

        foodY = (randomState % (HEIGHT - 2)) + 1;

    } while (isSnakePosition(foodX, foodY));
}


// Initialize a new game
void setup()
{
    clearSnake();

    score = 0;
    direction = 'D';
    gameOver = false;

    int startX = WIDTH / 2;
    int startY = HEIGHT / 2;

    // Add the tail first, then body, then head
    addHead(startX - 2, startY);
    addHead(startX - 1, startY);
    addHead(startX, startY);

    generateFood();
}


// Draw the game board
void draw()
{
    // Clear the Windows console
    system("cls");

    cout << "========== SNAKE GAME ==========\n";
    cout << "Score: " << score << "\n";
    cout << "Controls: W A S D | X: Exit\n\n";

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            // Draw the walls
            if (x == 0 || x == WIDTH - 1 ||
                y == 0 || y == HEIGHT - 1)
            {
                cout << "#";
            }
            // Draw the food
            else if (x == foodX && y == foodY)
            {
                cout << "*";
            }
            else
            {
                Node* current = head;
                bool printed = false;

                while (current != NULL)
                {
                    if (current->x == x &&
                        current->y == y)
                    {
                        if (current == head)
                        {
                            cout << "O";
                        }
                        else
                        {
                            cout << "o";
                        }

                        printed = true;
                        break;
                    }

                    current = current->next;
                }

                if (!printed)
                {
                    cout << " ";
                }
            }
        }

        cout << "\n";
    }

    cout << "\nO = Head   o = Body   * = Food\n";
}


// Read keyboard input without pressing Enter
void input()
{
    if (!_kbhit())
    {
        return;
    }

    char key = _getch();

    // Convert uppercase letters to lowercase
    if (key >= 'A' && key <= 'Z')
    {
        key = key - 'A' + 'a';
    }

    // Exit command
    if (key == 'x')
    {
        gameOver = true;
        return;
    }

    // Prevent an immediate 180-degree reversal
    if (key == 'w' && direction != 'S')
    {
        direction = 'W';
    }
    else if (key == 's' && direction != 'W')
    {
        direction = 'S';
    }
    else if (key == 'a' && direction != 'D')
    {
        direction = 'A';
    }
    else if (key == 'd' && direction != 'A')
    {
        direction = 'D';
    }
}


// Move the snake and check collisions
void logic()
{
    int nextX = head->x;
    int nextY = head->y;

    // Calculate the next head position
    if (direction == 'W')
    {
        nextY--;
    }
    else if (direction == 'S')
    {
        nextY++;
    }
    else if (direction == 'A')
    {
        nextX--;
    }
    else if (direction == 'D')
    {
        nextX++;
    }

    // Check collision with the walls
    if (nextX <= 0 || nextX >= WIDTH - 1 ||
        nextY <= 0 || nextY >= HEIGHT - 1)
    {
        gameOver = true;
        return;
    }

    // Check whether the snake will eat the food
    bool eating = (nextX == foodX && nextY == foodY);

    // If not eating, the tail moves away.
    // Therefore, moving into the old tail position is allowed.
    if (isSnakePosition(nextX, nextY, !eating))
    {
        gameOver = true;
        return;
    }

    // Add the new head
    addHead(nextX, nextY);

    if (eating)
    {
        // Keep the tail to increase snake length
        score += 10;
        generateFood();
    }
    else
    {
        // Remove the tail to maintain the same length
        removeTail();
    }
}


// Main function
int main()
{
    setup();
    draw();

    const int MOVE_DELAY = 150;

    auto lastMove = chrono::steady_clock::now();

    while (!gameOver)
    {
        // Check for keys repeatedly without waiting for Enter
        input();

        if (gameOver)
        {
            break;
        }

        auto now = chrono::steady_clock::now();

        // Move automatically after the time interval
        if (chrono::duration_cast<chrono::milliseconds>(
                now - lastMove).count() >= MOVE_DELAY)
        {
            logic();
            lastMove = now;

            if (!gameOver)
            {
                draw();
            }
        }

        // Short pause to avoid using 100% CPU
        Sleep(10);
    }

    draw();

    cout << "\n========== GAME OVER ==========\n";
    cout << "Final Score: " << score << "\n";

    while (true)
    {
        cout << "\nPress R to restart or X to exit: ";

        char choice = _getch();

        if (choice >= 'A' && choice <= 'Z')
        {
            choice = choice - 'A' + 'a';
        }

        if (choice == 'r')
        {
            setup();
            draw();

            lastMove = chrono::steady_clock::now();

            while (!gameOver)
            {
                input();

                if (gameOver)
                {
                    break;
                }

                auto now = chrono::steady_clock::now();

                if (chrono::duration_cast<chrono::milliseconds>(
                        now - lastMove).count() >= MOVE_DELAY)
                {
                    logic();
                    lastMove = now;

                    if (!gameOver)
                    {
                        draw();
                    }
                }

                Sleep(10);
            }

            draw();

            cout << "\n========== GAME OVER ==========\n";
            cout << "Final Score: " << score << "\n";
        }
        else if (choice == 'x')
        {
            break;
        }
    }

    clearSnake();

    return 0;
}