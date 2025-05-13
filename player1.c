#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define BROKER "34.145.124.95"
#define TURN_TOPIC "game/turn"
#define BOARD_TOPIC "game/board"
#define MOVE_TOPIC "game/move"
#define PLAYER 'X'
#define DELAY 5

char getTurn() {
    char turn = '?';
    FILE *f = popen("mosquitto_sub -h " BROKER " -t " TURN_TOPIC " -C 1 -W 2", "r");
    if (f) { fscanf(f, "%c", &turn); pclose(f); }
    return turn;
}

void getBoard(char *b) {
    FILE *f = popen("mosquitto_sub -h " BROKER " -t " BOARD_TOPIC " -C 1 -W 2", "r");
    if (f) { fgets(b, 10, f); pclose(f); } 
    else strcpy(b, "         ");
    b[9] = '\0';
}

int pickMove(const char *b) {
    int open[9], n = 0;
    for (int i = 0; i < 9; i++)
        if (b[i] == ' ') open[n++] = i;
    return n ? open[rand() % n] : -1;
}

void sendMove(int m) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "mosquitto_pub -h %s -t %s -m \"%c%d\"", BROKER, MOVE_TOPIC, PLAYER, m);
    system(cmd);
}

int main() {
    srand(time(NULL));
    while (1) {
        if (getTurn() != PLAYER) {
            printf("[X] Waiting...\n");
            sleep(DELAY);
            continue;
        }

        char board[10];
        getBoard(board);

        int move = pickMove(board);
        if (move == -1) {
            printf("[X] No valid move.\n");
        } else {
            printf("[X] Move: %d\n", move);
            sendMove(move);
        }

        sleep(DELAY);
    }
}
