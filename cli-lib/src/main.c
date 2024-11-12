#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "screen.h"  
#include "keyboard.h"
#include "timer.h"  

#define INITIAL_DELAY 100 
#define SCORE_FILE "scores.txt" 
#define NAME_MAX_LENGTH 50

typedef struct {
    int x;
    int y;
    int largura;
} Bola;

typedef struct {
    int x;
    int largura;
} Cesta;

typedef struct {
    char name[NAME_MAX_LENGTH];
    int score;
} ScoreEntry;

void draw_scoreboard(int score) {
    screenClear();
    screenGotoxy(1, 1); 
    printf("╔════════════════════════╗\n");
    printf("║        PLACAR          ║\n");
    printf("╠════════════════════════╣\n");
    printf("║  SCORE:       %02d     ║\n", score);
    printf("╚════════════════════════╝\n");
}

void save_score(const char *name, int score) {
    FILE *file = fopen(SCORE_FILE, "a");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo de pontuação");
        return;
    }
    fprintf(file, "%s %d\n", name, score);
    fclose(file);
}

void display_leaderboard() {
    FILE *file = fopen(SCORE_FILE, "r");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo de pontuação");
        return;
    }

    ScoreEntry *scores = malloc(100 * sizeof(ScoreEntry));
    int count = 0;
    while (fscanf(file, "%s %d", scores[count].name, &scores[count].score) != EOF && count < 100) {
        count++;
    }
    fclose(file);

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (scores[i].score < scores[j].score) {
                ScoreEntry temp = scores[i];
                scores[i] = scores[j];
                scores[j] = temp;
            }
        }
    }

    printf("\n╔════════════════════════╗\n");
    printf("  ║    PLACAR DE LÍDERES   ║\n");
    printf("  ╠════════════════════════╣\n");
    for (int i = 0; i < count && i < 10; i++) {
        printf("║  %2dº: %s - %02d     ║\n", i + 1, scores[i].name, scores[i].score);
    }
    printf("╚════════════════════════╝\n");

    free(scores);
}

int main() {
    char playerName[NAME_MAX_LENGTH];
    Bola bola = {37, 5, 3};
    Cesta cesta = {20, 5};
    int max_y = 20;
    int score = 0;
    int ball_falling = 0;
    int ch;
    int delay = INITIAL_DELAY;

    keyboardInit();
    screenInit(1); 
    timerInit(delay);

    srand(time(NULL));

    while (1) {
        draw_scoreboard(score); 

        for (int y = 0; y < max_y; y++) {
            for (int x = 0; x < 40; x++) {
                screenGotoxy(x + 1, y + 6); 
                if (y == bola.y && (x == bola.x || x == bola.x - 1 || x == bola.x + 1)) {
                    printf("O");
                } else if (y == max_y - 2 && x >= cesta.x - 2 && x <= cesta.x + 2) {
                    printf("-");
                } else if ((y > max_y - 2) && (x == cesta.x - 2 || x == cesta.x + 2)) {
                    printf("|");
                } else {
                    printf(" ");
                }
            }
        }

        screenUpdate(); 

        if (ball_falling) {
            bola.y++;
            if (bola.y >= max_y - 1) {
                if (bola.x >= cesta.x - 2 && bola.x <= cesta.x + 2) {
                    score++;
                    delay = (delay > 20) ? delay - 10 : 20;
                    ball_falling = 0;
                    bola.x = 37;
                    bola.y = 5;
                    timerUpdateTimer(delay);
                } else {
                    break;
                }
            }
        } else {
            bola.x--;
            if (bola.x < 2) {
                bola.x = 37;
            }
        }

        if (keyhit()) {
            ch = readch();
            if (ch == ' ') {
                ball_falling = 1;
            } else if (ch == 'q') {
                break;
            }
        }

        while (!timerTimeOver()) {
        }
        timerUpdateTimer(delay);
    }

    screenClear();

    printf("Sua pontuação: %d\n", score);
    printf("Digite seu nome: ", NAME_MAX_LENGTH - 1);
    fflush(stdout); 
    fgets(playerName, NAME_MAX_LENGTH, stdin);
    playerName[strcspn(playerName, "\n")] = '\0'; 

    save_score(playerName, score);
    display_leaderboard(); 
    printf("\nFim de jogo! Pressione qualquer tecla para sair.\n");
    readch(); 

    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    return 0;
}