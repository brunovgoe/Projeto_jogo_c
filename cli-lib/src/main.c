#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "screen.h"  
#include "keyboard.h"
#include "timer.h"  

#define INITIAL_DELAY 100 // Velocidade inicial da bola em milissegundos
#define SCORE_FILE "scores.txt" // Nome do arquivo de pontuações

// Estrutura para representar a bola
typedef struct {
    int x;
    int y;
    int largura;
} Bola;

// Estrutura para representar a cesta
typedef struct {
    int x;
    int largura;
} Cesta;

// Função para desenhar uma tabela de pontuação estilizada
void draw_scoreboard(int score) {
    screenClear();
    screenGotoxy(1, 1); // Posiciona o cursor no início da tela
    printf("╔════════════════════════╗\n");
    printf("║        PLACAR          ║\n");
    printf("╠════════════════════════╣\n");
    printf("║  SCORE:       %02d      ║\n", score);
    printf("╚════════════════════════╝\n");
}

// Função para salvar a pontuação no arquivo
void save_score(int score) {
    FILE *file = fopen(SCORE_FILE, "a");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo de pontuação");
        return;
    }
    fprintf(file, "%d\n", score);
    fclose(file);
}

// Função para ler e exibir as pontuações em ordem decrescente
void display_leaderboard() {
    FILE *file = fopen(SCORE_FILE, "r");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo de pontuação");
        return;
    }

    int *scores = malloc(100 * sizeof(int));
    int count = 0;
    while (fscanf(file, "%d", &scores[count]) != EOF && count < 100) {
        count++;
    }
    fclose(file);

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (scores[i] < scores[j]) {
                int temp = scores[i];
                scores[i] = scores[j];
                scores[j] = temp;
            }
        }
    }

    screenGotoxy(1, 5); // Posiciona o cursor abaixo do placar
    printf("╔════════════════════════╗\n");
    printf("║     PLACAR DE LÍDERES  ║\n");
    printf("╠════════════════════════╣\n");
    for (int i = 0; i < count && i < 10; i++) {
        printf("║  %2dº:        %02d        ║\n", i + 1, scores[i]);
    }
    printf("╚════════════════════════╝\n");

    free(scores);
}

int main() {
    Bola bola = {37, 5, 3};
    Cesta cesta = {20, 5};
    int max_y = 20;
    int score = 0;
    int ball_falling = 0;
    int ch;
    int delay = INITIAL_DELAY;

    // Inicializa as bibliotecas
    keyboardInit();
    screenInit(1); // Inicializa a tela com bordas
    timerInit(delay);

    srand(time(NULL));

    while (1) {
        draw_scoreboard(score);

        for (int y = 0; y < max_y; y++) {
            for (int x = 0; x < 40; x++) {
                screenGotoxy(x + 1, y + 6); // Ajusta a posição do cursor para desenhar o jogo
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

        screenUpdate(); // Atualiza a tela imediatamente

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
            // Aguarda o término do tempo
        }
        timerUpdateTimer(delay);
    }

    screenClear();
    save_score(score);
    draw_scoreboard(score);
    display_leaderboard();
    printf("Fim de jogo! Pressione qualquer tecla para sair.\n");
    readch(); // Aguarda entrada do usuário

    // Finaliza o uso das bibliotecas
    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    return 0;
}