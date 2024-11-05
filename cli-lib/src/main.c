#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 

#define DELAY 80000

int main() {
    int max_y = 25; // Defina os valores conforme necessário ou obtenha-os da cli-lib
    int max_x = 80;
    int ball_x = 0, ball_y = 2;
    int direction = 1; // 1 para direita, -1 para esquerda
    int score = 0;
    int speed = DELAY;
    int ch;

    // Inicialização da cli-lib
    cli_init();
    cli_noecho();
    cli_curs_set(0);
    cli_keypad(1);
    cli_nodelay(1);

    // Semente para números aleatórios
    srand(time(NULL));

    // Obter tamanho da janela, se a cli-lib suportar
    cli_getmaxyx(&max_y, &max_x);

    // Posicionar a bola no topo
    ball_x = rand() % max_x;

    while (1) {
        cli_clear();

        // Desenhar a tabela (cesta) no centro inferior
        cli_mvprintw(max_y - 2, max_x / 2, "|_|");

        // Desenhar a bola
        cli_mvprintw(ball_y, ball_x, "O");

        // Mostrar pontuação
        cli_mvprintw(1, 2, "Pontuação: %d", score);

        cli_refresh();

        // Controle da bola
        ball_x += direction;
        if (ball_x >= max_x - 1 || ball_x <= 0) {
            direction *= -1;
        }

        // Capturar input do usuário
        ch = cli_getch();
        if (ch == ' ' || ch == CLI_KEY_DOWN) {
            // Verificar se a bola está alinhada com a cesta
            if (ball_y >= max_y - 3 && abs(ball_x - max_x / 2) <= 1) {
                score++;
                speed = (speed > 20000) ? speed - 5000 : speed;
            } else {
                // Fim de jogo
                break;
            }
            // Reiniciar posição da bola
            ball_x = rand() % max_x;
            ball_y = 2;
        } else if (ch == 'q') {
            // Sair do jogo
            break;
        }

        // Aumentar a posição vertical da bola
        ball_y++;
        if (ball_y >= max_y - 2) {
            ball_y = 2;
        }

        usleep(speed);
    }

    // Exibir mensagem de fim de jogo
    cli_clear();
    cli_mvprintw(max_y / 2, (max_x - 20) / 2, "Fim de jogo! Pontuação: %d", score);
    cli_mvprintw(max_y / 2 + 1, (max_x - 30) / 2, "Pressione qualquer tecla para sair.");
    cli_nodelay(0);
    cli_getch();

    cli_endwin();

    // Aqui você pode adicionar código para salvar a pontuação e atualizar o ranking.

    return 0;
