#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// Estruturas simples
typedef struct {
    int tamanho;
    int pecas;
    int max_jogadas;
} Config;

typedef struct {
    int** tab;
    int pecas1;
    int pecas2;
    int jogador;
    int jogadas;
    Config cfg;
} Jogo;

typedef struct {
    int pvp;
    int pvc;
    int vitorias1;
    int vitorias2;
    int vitorias_pc;
    int empates;
} Historico;

// Funções básicas
Config criar_config() {
    Config c;
    c.tamanho = 3;
    c.pecas = 3;
    c.max_jogadas = 20;
    return c;
}

Jogo* criar_jogo(Config c) {
    Jogo* j = (Jogo*)malloc(sizeof(Jogo));
    j->cfg = c;
    j->tab = (int**)malloc(c.tamanho * sizeof(int*));
    for (int i = 0; i < c.tamanho; i++) {
        j->tab[i] = malloc(c.tamanho * sizeof(int));
        for (int k = 0; k < c.tamanho; k++) j->tab[i][k] = 0;
    }
    j->pecas1 = c.pecas;
    j->pecas2 = c.pecas;
    j->jogador = 1;
    j->jogadas = 0;
    return j;
}

void destruir_jogo(Jogo* j) {
    for (int i = 0; i < j->cfg.tamanho; i++) free(j->tab[i]);
    free(j->tab);
    free(j);
}

void mostrar_tabuleiro(Jogo* j) {
    printf("Tabuleiro:\n");
    for (int i = 0; i < j->cfg.tamanho; i++) {
        for (int k = 0; k < j->cfg.tamanho; k++) {
            printf("%d ", j->tab[i][k]);
        }
        printf("\n");
    }
}

bool posicao_valida(Jogo* j, int x, int y) {
    if (x < 0 || y < 0 || x >= j->cfg.tamanho || y >= j->cfg.tamanho) return false;
    return j->tab[x][y] == 0;
}

bool colocar_peca(Jogo* j, int x, int y) {
    if (!posicao_valida(j, x, y)) return false;
    j->tab[x][y] = j->jogador;
    if (j->jogador == 1) j->pecas1--;
    else j->pecas2--;
    return true;
}

void trocar_jogador(Jogo* j) {
    if (j->jogador == 1) j->jogador = 2;
    else j->jogador = 1;
}

bool ganhou(Jogo* j, int x, int y) {
    int n = j->cfg.tamanho;
    int p = j->tab[x][y];
    bool lin = true, col = true, diag1 = true, diag2 = true;
    for (int i = 0; i < n; i++) {
        if (j->tab[x][i] != p) lin = false;
        if (j->tab[i][y] != p) col = false;
        if (j->tab[i][i] != p) diag1 = false;
        if (j->tab[i][n-1-i] != p) diag2 = false;
    }
    return lin || col || diag1 || diag2;
}

bool empate(Jogo* j) {
    return j->jogadas >= j->cfg.max_jogadas;
}

void menu() {
    printf("1. PvP\n2. PvC\n3. Sair\nEscolha: ");
}

void tutorial() {
    printf("Coloque suas peças no tabuleiro. Quem alinhar primeiro vence.\n");
}

void jogar_pvp(Jogo* j) {
    int x, y, fim = 0;
    while (!fim) {
        mostrar_tabuleiro(j);
        printf("Jogador %d, linha e coluna: ", j->jogador);
        scanf("%d %d", &x, &y);
        if (colocar_peca(j, x, y)) {
            j->jogadas++;
            if (ganhou(j, x, y)) {
                mostrar_tabuleiro(j);
                printf("Jogador %d venceu!\n", j->jogador);
                fim = 1;
            } else if (empate(j)) {
                mostrar_tabuleiro(j);
                printf("Empate!\n");
                fim = 1;
            } else {
                trocar_jogador(j);
            }
        } else {
            printf("Posição inválida!\n");
        }
    }
}

void jogar_pvc(Jogo* j) {
    int x, y, fim = 0;
    while (!fim) {
        mostrar_tabuleiro(j);
        if (j->jogador == 1) {
            printf("Sua vez, linha e coluna: ");
            scanf("%d %d", &x, &y);
        } else {
            x = rand() % j->cfg.tamanho;
            y = rand() % j->cfg.tamanho;
            printf("Computador jogou: %d %d\n", x, y);
        }
        if (colocar_peca(j, x, y)) {
            j->jogadas++;
            if (ganhou(j, x, y)) {
                mostrar_tabuleiro(j);
                if (j->jogador == 1) printf("Você venceu!\n");
                else printf("Computador venceu!\n");
                fim = 1;
            } else if (empate(j)) {
                mostrar_tabuleiro(j);
                printf("Empate!\n");
                fim = 1;
            } else {
                trocar_jogador(j);
            }
        }
    }
}

int main() {
    srand(time(NULL));
    Config c = criar_config();
    int op;
    do {
        menu();
        scanf("%d", &op);
        if (op == 1) {
            Jogo* j = criar_jogo(c);
            jogar_pvp(j);
            destruir_jogo(j);
        } else if (op == 2) {
            Jogo* j = criar_jogo(c);
            jogar_pvc(j);
            destruir_jogo(j);
        } else if (op == 3) {
            printf("Saindo...\n");
        } else {
            printf("Opção inválida!\n");
        }
    } while (op != 3);
    return 0;
}
