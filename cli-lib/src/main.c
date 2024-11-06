#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#define INITIAL_DELAY 100000 // Velocidade inicial da bola em microsegundos

// Função para limpar a tela
void clear_screen() {
    printf("\033[H\033[J");
}

// Função para configurar o terminal para entrada não bloqueante
void configure_terminal() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO); // Desabilita modo canônico e eco
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Função para restaurar as configurações do terminal
void restore_terminal() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO); // Reativa modo canônico e eco
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Função para verificar se uma tecla foi pressionada
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

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

// Função para capturar a tecla pressionada
int getch() {
    int ch;
    ch = getchar();
    return ch;
}

// Função para dormir por um tempo em milissegundos
void sleep_milliseconds(int milliseconds) {
    usleep(milliseconds * 1000);
}

int main() {
    int max_y = 20; // Altura do terminal
    int max_x = 40; // Largura do terminal
    int ball_x = max_x - 3; // Inicia a bola na extrema direita
    int ball_y = 5; // Linha onde a bola está se movendo horizontalmente
    int basket_x = max_x / 2; // Posição da cesta (fixa)
    int score = 0;
    int ball_falling = 0; // Flag para indicar se a bola está caindo
    int ch;
    int delay = INITIAL_DELAY; // Velocidade inicial da bola

    // Configurar o terminal para entrada não bloqueante
    configure_terminal();

    // Semente para números aleatórios
    srand(time(NULL));

    while (1) {
        clear_screen();

        // Desenhar a tela
        for (int y = 0; y < max_y; y++) {
            for (int x = 0; x < max_x; x++) {
                // Desenhar a bola maior (3 caracteres de largura)
                if (y == ball_y && (x == ball_x || x == ball_x - 1 || x == ball_x + 1)) {
                    putchar('O');
                } 
                // Desenhar o aro da cesta (linha horizontal)
                else if (y == max_y - 2 && x >= basket_x - 2 && x <= basket_x + 2) {
                    putchar('-');
                } 
                // Desenhar as bordas da cesta (linhas verticais)
                else if ((y > max_y - 2) && (x == basket_x - 2 || x == basket_x + 2)) {
                    putchar('|');
                }
                // Espaço em branco
                else {
                    putchar(' ');
                }
            }
            putchar('\n');
        }

        // Mostrar pontuação
        printf("Pontuação: %d\n", score);

        // Verificar se a bola está caindo
        if (ball_falling) {
            ball_y++;
            if (ball_y >= max_y - 1) { // Verificar se a bola chegou ao fundo
                if (ball_x >= basket_x - 2 && ball_x <= basket_x + 2) {
                    score++;
                    // Aumentar a velocidade da bola (diminuir o delay)
                    delay = (delay > 20000) ? delay - 5000 : 20000; // Limite mínimo para o delay
                    ball_falling = 0; // Reiniciar a bola
                    ball_x = max_x - 3;
                    ball_y = 5;
                } else {
                    // Fim de jogo
                    break;
                }
            }
        } else {
            // Mover a bola horizontalmente
            ball_x--;
            if (ball_x < 2) {
                ball_x = max_x - 3; // Reinicia na direita
            }
        }

        // Capturar input do usuário
        if (kbhit()) {
            ch = getch();
            if (ch == ' ') {
                ball_falling = 1; // Iniciar queda da bola
            } else if (ch == 'q') {
                // Sair do jogo
                break;
            }
        }

        sleep_milliseconds(delay / 1000);
    }

    // Restaurar as configurações do terminal
    restore_terminal();

    // Exibir mensagem de fim de jogo
    clear_screen();
    printf("Fim de jogo! Pontuação: %d\n", score);
    printf("Pressione qualquer tecla para sair.\n");
    getchar(); // Aguarda entrada do usuário para sair

    return 0;
}