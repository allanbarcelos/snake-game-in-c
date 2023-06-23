#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

// Function to get keyboard input
int kbhit() {
    struct termios oldt, newt;
    int ch, oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

// Function to set the cursor position
void gotoxy(int x, int y) {
    printf("\033[%d;%df", y, x);
    fflush(stdout);
}

int main() {
    // Game setup
    int width = 20;     // Width of the game area
    int height = 20;    // Height of the game area
    int gameOver = 0;   // Flag to indicate game over
    int score = 0;      // Current score
    int x, y;           // Current position of the snake head
    int fruitX, fruitY; // Position of the fruit
    int tailX[100], tailY[100]; // Position of each tail segment
    int tailLength = 0; // Length of the tail
    int dir;            // Direction of the snake (1 = up, 2 = down, 3 = left, 4 = right)

    // Initialize the game
    x = width / 2;
    y = height / 2;

    // Generate initial fruit position
    srand(time(NULL));
    fruitX = rand() % width;
    fruitY = rand() % height;

    // Game loop
    while (!gameOver) {
        // Draw the game area
        system("clear");

        // Draw the top border
        for (int i = 0; i < width + 2; i++)
            printf("#");
        printf("\n");

        // Draw the game area
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (j == 0)
                    printf("#"); // Draw the left border

                // Draw the snake head or tail segment
                if (i == y && j == x)
                    printf("O");
                else if (i == fruitY && j == fruitX)
                    printf("F"); // Draw the fruit
                else {
                    int isTail = 0;
                    for (int k = 0; k < tailLength; k++) {
                        if (i == tailY[k] && j == tailX[k]) {
                            printf("o"); // Draw the tail segment
                            isTail = 1;
                        }
                    }
                    if (!isTail)
                        printf(" ");
                }

                if (j == width - 1)
                    printf("#"); // Draw the right border
            }
            printf("\n");
        }

        // Draw the bottom border
        for (int i = 0; i < width + 2; i++)
            printf("#");
        printf("\n");

        // Display the score
        printf("Score: %d\n", score);

        // Update the game logic
        if (kbhit()) {
            int key = getchar();
            if (key == 27 && getchar() == '[') { // Check for arrow key
                switch (getchar()) {
                    case 'A': // Up arrow
                        dir = 1; // Up
                        break;
                    case 'B': // Down arrow
                        dir = 2; // Down
                        break;
                    case 'C': // Right arrow
                        dir = 4; // Right
                        break;
                    case 'D': // Left arrow
                        dir = 3; // Left
                        break;
                }
            } else if (key == 'x') {
                gameOver = 1; // Press 'x' to exit the game
            }
        }

        // Update the snake position
        switch (dir) {
            case 1: // Up
                y--;
                break;
            case 2: // Down
                y++;
                break;
            case 3: // Left
                x--;
                break;
            case 4: // Right
                x++;
                break;
        }

        // Check for collision with the fruit
        if (x == fruitX && y == fruitY) {
            score++;
            tailLength++;

            // Generate new fruit position
            fruitX = rand() % width;
            fruitY = rand() % height;
        }

        // Check for collision with the borders
        if (x < 0 || x >= width || y < 0 || y >= height)
            gameOver = 1;

        // Check for collision with the tail
        for (int i = 0; i < tailLength; i++) {
            if (x == tailX[i] && y == tailY[i])
                gameOver = 1;
        }

        // Shift the tail positions
        for (int i = tailLength - 1; i > 0; i--) {
            tailX[i] = tailX[i - 1];
            tailY[i] = tailY[i - 1];
        }

        // Update the tail's first position
        tailX[0] = x;
        tailY[0] = y;

        usleep(100000); // Pause for a short duration
    }

    printf("\nGame Over!\n");
    printf("Your score: %d\n", score);

    return 0;
}
