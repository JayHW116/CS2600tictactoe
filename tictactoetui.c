#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define BROKER "34.145.124.95"
#define GRID_SIZE 10
#define PLAYER_SIZE 2
#define COUNT_SIZE 16

void printGrid(const char *grid) {
    printf("\n");
    for (int i = 0; i < 9; i++) {
        printf(" %c ", grid[i] == ' ' ? '-' : grid[i]);
        if ((i + 1) % 3 == 0) printf("\n");
    }
    printf("\n");
}

void sendMove(char player, int pos) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "mosquitto_pub -h %s -t game/move -m %c%d", BROKER, player, pos);
    system(cmd);
}

int getMove(const char *grid) {
    int pos = -1;
    while (1) {
        printf("Enter a move (0-8): ");
        scanf("%d", &pos);
        if (pos >= 0 && pos <= 8 && grid[pos] == ' ') break;
        printf("Invalid move. Try again.\n");
    }
    return pos;
}

void flushInput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void update(char *grid, char *player, char *count) {
    memset(grid, 0, GRID_SIZE);
    memset(player, 0, PLAYER_SIZE);
    memset(count, 0, COUNT_SIZE);

    FILE *fp;

    fp = _popen("mosquitto_sub -h " BROKER " -t game/board -C 1 -W 2", "r");
    if (fp) { fgets(grid, GRID_SIZE, fp); _pclose(fp); }

    fp = _popen("mosquitto_sub -h " BROKER " -t game/turn -C 1 -W 2", "r");
    if (fp) { fgets(player, PLAYER_SIZE, fp); _pclose(fp); }

    fp = _popen("mosquitto_sub -h " BROKER " -t game/complete -C 1 -W 2", "r");
    if (fp) { fgets(count, COUNT_SIZE, fp); _pclose(fp); }

    grid[9] = '\0';
    player[1] = '\0';
    count[strcspn(count, "\n")] = '\0';
}

int hasWinner(const char *g) {
    return
        strstr(g, "XXX") || strstr(g, "OOO") ||
        (g[0] == g[3] && g[3] == g[6] && g[0] != ' ') ||
        (g[1] == g[4] && g[4] == g[7] && g[1] != ' ') ||
        (g[2] == g[5] && g[5] == g[8] && g[2] != ' ') ||
        (g[0] == g[4] && g[4] == g[8] && g[0] != ' ') ||
        (g[2] == g[4] && g[4] == g[6] && g[2] != ' ');
}

void draw(const char *grid, const char *player, const char *count) {
    system("cls");
    printf("ESP32 Tic-Tac-Toe\n");
    printGrid(grid);
    printf("Turn: %s\tGames Played: %s\n", player, count);
}

int main() {
    char mode;
    char grid[GRID_SIZE] = "         ";
    char player[PLAYER_SIZE] = "?";
    char count[COUNT_SIZE] = "0";
    char lastCount[COUNT_SIZE] = "-1";

    printf("Select Game Mode:\n");
    printf("1 - 1 Player (You vs Bash Script)\n");
    printf("2 - 2 Player (Local Human vs Human)\n");
    printf("3 - Automated Mode (C vs Bash on GCP)\n");
    printf("Choice: ");
    scanf("%c", &mode);
    flushInput();

    printf("\nStarting TUI Tic-Tac-Toe...\n\n");

    if (mode == '3') {
        while (1) {
            update(grid, player, count);
            draw(grid, player, count);
            Sleep(1500);
        }
    }

    while (1) {
        update(grid, player, count);

        int isClear = strcmp(grid, "         ") == 0;
        int newGame = strcmp(count, lastCount) != 0;
        int gameEnded = hasWinner(grid);

        if (newGame && gameEnded) {
            printf("\nGame finished. Waiting for board reset...\n");
            strcpy(lastCount, count);

            for (int i = 0; i < 5; i++) {
                update(grid, player, count);
                if (strcmp(grid, "         ") == 0) break;
                Sleep(1000);
            }

            draw(grid, player, count);
            Sleep(1500);
            continue;
        }

        draw(grid, player, count);

        if (isClear && !(
            (mode == '1' && player[0] == 'X') ||
            (mode == '2' && (player[0] == 'X' || player[0] == 'O'))
        )) {
            printf("\nBoard cleared. Waiting for next round...\n");
            Sleep(2000);
            continue;
        }

        if ((mode == '1' && player[0] == 'X') ||
            (mode == '2' && (player[0] == 'X' || player[0] == 'O'))) {

            update(grid, player, count);
            printf("Your turn (%c).\n", player[0]);
            int pos = getMove(grid);
            sendMove(player[0], pos);
            Sleep(1500);
            continue;
        }

        printf("Waiting for opponent move...\n");
        Sleep(2000);
    }

    return 0;
}
