#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>

#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

typedef struct {
    char modo_jogo[30];  // Modo de jogo (PvP ou PvC)
    char resultado[30];  // Resultado da partida (Vitória, Empate, etc.)
    double duracao;      // Duração da partida em segundos
} RegistroPartida;

// Define um tipo enum para representar o estado de cada espaço do tabuleiro: vazio, jogador 1 ou jogador 2
typedef enum {
    VAZIO,
    JOGADOR1,
    JOGADOR2
} EstadoEspaço;

// Estrutura para armazenar as coordenadas (x, y) de cada ponto do tabuleiro
typedef struct {
    float x, y;
} PontoTabuleiro;

// Funções utilitárias
void limpaTela();

// Funções para salvar e carregar estado do jogo
void salvar_partida_atual_txt(const char *arquivo, EstadoEspaço tabuleiro[7], int jogador_atual, int p1, int p2, double tempo);
bool carregar_partida_txt(const char *arquivo, EstadoEspaço tabuleiro[7], int *jogador_atual, int *p1, int *p2, double *tempo);

// Funções para salvar e carregar histórico
void salvar_historico(const char *arquivo, RegistroPartida *partidas, int num_partidas);
void carregar_historico(const char *arquivo);

// Funções de desenho
void pecas_jogadores(ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h, int pecas_jogador1, int pecas_jogador2);
void desenha_tabuleiroTriangulo(ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
                                EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
                                ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2,
                                float mouse_x, float mouse_y, int placar_j1, int placar_j2, bool *showing_menu);
void desenha_menu1(ALLEGRO_BITMAP *background, ALLEGRO_FONT *font);
void desenha_modos2(ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *play_image, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h);
void desenha_ajuda3(ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *help_image, ALLEGRO_FONT *font, int win_w, int win_h);
void desenha_historico4(ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *History_image, ALLEGRO_FONT *font, int win_w, int win_h);

// Funções de lógica do jogo
bool movimento_valido(int origem, int destino, EstadoEspaço tabuleiro[7]);
EstadoEspaço verificar_vitoria(EstadoEspaço tabuleiro[7]);
void empate_final(ALLEGRO_FONT *font, int win_w, int win_h);
void empate_espera(ALLEGRO_FONT *font, int win_w, int win_h, int jogador_atual, RegistroPartida *historico, int *num_partidas);
void pausar_jogo(ALLEGRO_FONT *font2, int win_w, int win_h, EstadoEspaço tabuleiro[7], int jogador, int pecas_jogador1, int pecas_jogador2, time_t inicio);
void popup_sair(ALLEGRO_FONT *font2, int win_w, int win_h, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, bool *showing_menu);
void exibir_vitoria(ALLEGRO_FONT *font, int win_w, int win_h, int vencedor, EstadoEspaço tabuleiro[7]);
// Funções de IA
int ia_escolher_jogada(EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7], int jogador_ia);
bool ia_escolher_movimento(EstadoEspaço tabuleiro[7], int jogador_ia, int *origem, int *destino);

// Funções de posicionamento e movimentação
void posicionamento(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
                    EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
                    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2, int *placar_j1, int *placar_j2);

void posicionamento_pvm(
    ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2, int *placar_j1, int *placar_j2, int jogador_humano, int jogador_ia);

void movimentacao(
    ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2,
    int *placar_j1, int *placar_j2,
    RegistroPartida *historico, int *num_partidas);
// Funções para o modo Player vs Computador
void movimentacao_pvm(
    ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2,
    int *placar_j1, int *placar_j2, int jogador_humano, int jogador_ia,
    RegistroPartida *historico, int *num_partidas);

// Funções de tempo
time_t iniciar_timer();
double calcular_duracao(time_t inicio);














// Funções utilitárias

// Função para limpar o terminal, usando o comando apropriado para o sistema operacional
void limpaTela() {
    system(CLEAR);
}

void salvar_partida_atual_txt(const char *arquivo, EstadoEspaço tabuleiro[7], int jogador_atual, int p1, int p2, double tempo) {
    FILE *f = fopen(arquivo, "w");
    if (!f) {
        fprintf(stderr, "Erro ao abrir arquivo para salvar estado do jogo!\n");
        return;
    }

    // Salva o estado do tabuleiro
    for (int i = 0; i < 7; i++) {
        fprintf(f, "%d ", tabuleiro[i]);
    }
    fprintf(f, "\n");

    // Salva o jogador atual, peças posicionadas e tempo decorrido
    fprintf(f, "%d %d %d %.2f\n", jogador_atual, p1, p2, tempo);

    fclose(f);
}

bool carregar_partida_txt(const char *arquivo, EstadoEspaço tabuleiro[7], int *jogador_atual, int *p1, int *p2, double *tempo) {
    FILE *f = fopen(arquivo, "r");
    if (!f) {
        fprintf(stderr, "Erro ao abrir arquivo para carregar estado do jogo!\n");
        return false;
    }

    // Carrega o estado do tabuleiro
    for (int i = 0; i < 7; i++) {
        fscanf(f, "%d", (int *)&tabuleiro[i]);
    }

    // Carrega o jogador atual, peças posicionadas e tempo decorrido
    fscanf(f, "%d %d %d %lf", jogador_atual, p1, p2, tempo);

    fclose(f);
    return true;
}



// Função para desenhar a quantidade de peças de cada jogador na tela
void pecas_jogadores(ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h, int pecas_jogador1, int pecas_jogador2) {
    // Desenha o texto "Jogador 1" no canto superior esquerdo
    al_draw_text(font, al_map_rgb(255, 0, 0), 30, 10, ALLEGRO_ALIGN_LEFT, "Jogador 1");
    // Desenha o texto "Jogador 2" no canto superior direito
    al_draw_text(font, al_map_rgb(0, 0, 255), win_w - 30, 10, ALLEGRO_ALIGN_RIGHT, "Jogador 2");

    // Carrega as imagens das peças dos jogadores
    ALLEGRO_BITMAP *pecapng_player1 = al_load_bitmap("Content/player1.png");
    ALLEGRO_BITMAP *pecapng_player2 = al_load_bitmap("Content/player2.png");

    // Desenha as peças do Jogador 1 (PNG se já posicionada, círculo cinza se não)
    for (int i = 0; i < 3; i++) {
        if (i < pecas_jogador1 && pecapng_player1) {
            al_draw_scaled_bitmap(
                pecapng_player1, 0, 0,
                al_get_bitmap_width(pecapng_player1), al_get_bitmap_height(pecapng_player1),
                28 + i * 30, 28, 24, 24, 0
            );
        } else {
            al_draw_filled_circle(40 + i * 30, 40, 12, al_map_rgb(200, 200, 200));
            al_draw_circle(40 + i * 30, 40, 12, al_map_rgb(0,0,0), 2);
        }
    }

    // Desenha as peças do Jogador 2 (PNG se já posicionada, círculo cinza se não)
    for (int i = 0; i < 3; i++) {
        if (i < pecas_jogador2 && pecapng_player2) {
            al_draw_scaled_bitmap(
                pecapng_player2, 0, 0,
                al_get_bitmap_width(pecapng_player2), al_get_bitmap_height(pecapng_player2),
                win_w - 52 - i * 30, 28, 24, 24, 0
            );
        } else {
            al_draw_filled_circle(win_w - 40 - i * 30, 40, 12, al_map_rgb(200, 200, 200));
            al_draw_circle(win_w - 40 - i * 30, 40, 12, al_map_rgb(0,0,0), 2);
        }
    }

    if (pecapng_player1) al_destroy_bitmap(pecapng_player1);
    if (pecapng_player2) al_destroy_bitmap(pecapng_player2);
}

void desenha_tabuleiroTriangulo(
    ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2,
    float mouse_x, float mouse_y, int placar_j1, int placar_j2,
    bool *showing_menu
) {
    // Desenha a imagem de fundo na tela
    al_draw_bitmap(background, 0, 0, 0);

    // Conta as peças de cada jogador no tabuleiro
    int pecas_jogador1 = 0, pecas_jogador2 = 0;
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == JOGADOR1) pecas_jogador1++; // Conta peças do jogador 1
        if (tabuleiro[i] == JOGADOR2) pecas_jogador2++; // Conta peças do jogador 2
    }
    // Desenha a quantidade de peças de cada jogador na tela
    pecas_jogadores(font, font2, win_w, win_h, pecas_jogador1, pecas_jogador2);

    // Desenha as linhas do triângulo conectando os pontos do tabuleiro
    al_draw_line(pontos_tabuleiro[0].x, pontos_tabuleiro[0].y, pontos_tabuleiro[1].x, pontos_tabuleiro[1].y, al_map_rgb(0,0,0), 3);
    al_draw_line(pontos_tabuleiro[0].x, pontos_tabuleiro[0].y, pontos_tabuleiro[2].x, pontos_tabuleiro[2].y, al_map_rgb(0,0,0), 3);
    al_draw_line(pontos_tabuleiro[0].x, pontos_tabuleiro[0].y, pontos_tabuleiro[3].x, pontos_tabuleiro[3].y, al_map_rgb(0,0,0), 3);
    al_draw_line(pontos_tabuleiro[1].x, pontos_tabuleiro[1].y, pontos_tabuleiro[2].x, pontos_tabuleiro[2].y, al_map_rgb(0,0,0), 3);
    al_draw_line(pontos_tabuleiro[2].x, pontos_tabuleiro[2].y, pontos_tabuleiro[3].x, pontos_tabuleiro[3].y, al_map_rgb(0,0,0), 3);
    al_draw_line(pontos_tabuleiro[1].x, pontos_tabuleiro[1].y, pontos_tabuleiro[4].x, pontos_tabuleiro[4].y, al_map_rgb(0,0,0), 3);
    al_draw_line(pontos_tabuleiro[2].x, pontos_tabuleiro[2].y, pontos_tabuleiro[5].x, pontos_tabuleiro[5].y, al_map_rgb(0,0,0), 3);
    al_draw_line(pontos_tabuleiro[3].x, pontos_tabuleiro[3].y, pontos_tabuleiro[6].x, pontos_tabuleiro[6].y, al_map_rgb(0,0,0), 3);
    al_draw_line(pontos_tabuleiro[4].x, pontos_tabuleiro[4].y, pontos_tabuleiro[5].x, pontos_tabuleiro[5].y, al_map_rgb(0,0,0), 3);
    al_draw_line(pontos_tabuleiro[5].x, pontos_tabuleiro[5].y , pontos_tabuleiro[6].x, pontos_tabuleiro[6].y, al_map_rgb(0,0,0), 3);

    // Percorre todas as casas do tabuleiro para desenhar círculos e peças
    for (int i = 0; i < 7; i++) {
        // Calcula a distância do mouse até o centro da casa
        float dx = mouse_x - pontos_tabuleiro[i].x;
        float dy = mouse_y - pontos_tabuleiro[i].y;
        bool mouse_sobre = (dx*dx + dy*dy <= 25*25); // Verifica se o mouse está sobre a casa

        // Se o mouse está sobre uma casa vazia, desenha um destaque amarelo semi-transparente
        if (mouse_sobre && tabuleiro[i] == VAZIO) {
            al_draw_filled_circle(pontos_tabuleiro[i].x, pontos_tabuleiro[i].y, 27, al_map_rgba(255, 255, 0, 120));
        }

        // Se a casa tem peça do jogador 1, desenha a imagem da peça do jogador 1
        if (tabuleiro[i] == JOGADOR1 && peca_player1) {
            al_draw_scaled_bitmap(
                peca_player1, 0, 0,
                al_get_bitmap_width(peca_player1), al_get_bitmap_height(peca_player1),
                pontos_tabuleiro[i].x - 25, pontos_tabuleiro[i].y - 25, 50, 50, 0
            );
        }
        // Se a casa tem peça do jogador 2, desenha a imagem da peça do jogador 2
        else if (tabuleiro[i] == JOGADOR2 && peca_player2) {
            al_draw_scaled_bitmap(
                peca_player2, 0, 0,
                al_get_bitmap_width(peca_player2), al_get_bitmap_height(peca_player2),
                pontos_tabuleiro[i].x - 25, pontos_tabuleiro[i].y - 25, 50, 50, 0
            );
        }
        // Se a casa está vazia, desenha um círculo cinza claro com borda preta
        else {
            al_draw_filled_circle(pontos_tabuleiro[i].x, pontos_tabuleiro[i].y, 25, al_map_rgb(200, 200, 200));
            al_draw_circle(pontos_tabuleiro[i].x, pontos_tabuleiro[i].y, 25, al_map_rgb(0,0,0), 3);
        }
    }

    // Desenha o título do jogo no topo do tabuleiro
    al_draw_text(font, al_map_rgb(0, 0, 0), win_w / 2, 30, ALLEGRO_ALIGN_CENTER, "Tri- Angle");

    // Área inferior (barra de botões e placar) - visual moderno
    al_draw_filled_rounded_rectangle(0, 370, 728, 410, 12, 12, al_map_rgb(230, 230, 240));
    al_draw_rounded_rectangle(0, 370, 728, 410, 12, 12, al_map_rgb(80,80,80), 2);

    // Botão "Pausar" (esquerda)
    al_draw_filled_rounded_rectangle(50, 378, 180, 402, 10, 10, al_map_rgb(255, 240, 240));
    al_draw_rounded_rectangle(50, 378, 180, 402, 10, 10, al_map_rgb(200,0,0), 2);
    al_draw_text(font, al_map_rgb(200, 0, 0), 115, 376, ALLEGRO_ALIGN_CENTER, "Pausar");

    // Placar (centralizado)
    char placar[32];
    sprintf(placar, "J1: %d  -  %d :J2", placar_j1, placar_j2);
    al_draw_filled_rounded_rectangle(270, 378, 458, 402, 10, 10, al_map_rgb(240, 240, 220));
    al_draw_rounded_rectangle(270, 378, 458, 402, 10, 10, al_map_rgb(120,120,120), 2);
    al_draw_text(font, al_map_rgb(60, 60, 60), 364, 378, ALLEGRO_ALIGN_CENTER, placar);

    // Botão "Empate" (à direita do placar)
    al_draw_filled_rounded_rectangle(478, 378, 598, 402, 10, 10, al_map_rgb(240, 255, 240));
    al_draw_rounded_rectangle(478, 378, 598, 402, 10, 10, al_map_rgb(0,150,0), 2);
    al_draw_text(font, al_map_rgb(0, 120, 0), 538, 376, ALLEGRO_ALIGN_CENTER, "Empate");

    // Botão "Sair" (direita)
    al_draw_filled_rounded_rectangle(618, 378, 708, 402, 10, 10, al_map_rgb(240, 240, 255));
    al_draw_rounded_rectangle(618, 378, 708, 402, 10, 10, al_map_rgb(0,0,200), 2);
    al_draw_text(font, al_map_rgb(0, 0, 200), 663, 376, ALLEGRO_ALIGN_CENTER, "Sair");

    // Atualiza a tela para mostrar tudo desenhado
    al_flip_display();

    if (mouse_x >= 618 && mouse_x <= 708 && mouse_y >= 378 && mouse_y <= 402) {
        popup_sair(font2, win_w, win_h, background, font, showing_menu); // Chama o popup de saída
    }
}

// Funções de desenho de telas
void desenha_menu1(ALLEGRO_BITMAP *background, ALLEGRO_FONT *font) {
    al_draw_bitmap(background, 0, 0, 0);
    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 75, ALLEGRO_ALIGN_CENTER, "Tri Angle");
    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 125, ALLEGRO_ALIGN_CENTER, "Jogar");
    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 150, ALLEGRO_ALIGN_CENTER, "Ajuda");
    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 175, ALLEGRO_ALIGN_CENTER, "Historico");
    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 200, ALLEGRO_ALIGN_CENTER, "Sair");
    al_flip_display();
}

void desenha_modos2(ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *play_image, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h) {
    al_draw_bitmap(background, 0, 0, 0);
    int play_w = al_get_bitmap_width(play_image);
    int play_h = al_get_bitmap_height(play_image);
    al_draw_scaled_bitmap(play_image, 0, 0, play_w, play_h, 0, 0, win_w, win_h, 0);
    al_draw_text(font2, al_map_rgb(0, 0, 0), 150, 310, ALLEGRO_ALIGN_CENTER, "Jogar");
    al_draw_text(font2, al_map_rgb(0, 0, 0), 585, 310, ALLEGRO_ALIGN_CENTER, "Jogar");
    al_draw_text(font, al_map_rgb(255, 0, 0), 370, 300, ALLEGRO_ALIGN_CENTER, "Retornar ao Menu");
    al_flip_display();
}

void desenha_ajuda3(ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *help_image, ALLEGRO_FONT *font, int win_w, int win_h) {
    al_draw_bitmap(background, 0, 0, 0);
    int img_w = al_get_bitmap_width(help_image);
    int img_h = al_get_bitmap_height(help_image);
    al_draw_scaled_bitmap(help_image, 0, 0, img_w, img_h, 0, 0, win_w, win_h, 0);
    al_draw_text(font, al_map_rgb(255, 0, 0), 160, 300, ALLEGRO_ALIGN_CENTER, "Retornar ao Menu");
    al_flip_display();
}

void desenha_historico4(ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *History_image, ALLEGRO_FONT *font, int win_w, int win_h) {
    al_draw_bitmap(background, 0, 0, 0);
    int history_w = al_get_bitmap_width(History_image);
    int history_h = al_get_bitmap_height(History_image);
    al_draw_scaled_bitmap(History_image, 0, 0, history_w, history_h, 0, 0, win_w, win_h, 0);
    al_draw_text(font, al_map_rgb(0, 0, 0), 364, 40, ALLEGRO_ALIGN_CENTER, "Historico");
    al_draw_text(font, al_map_rgb(255, 0, 0), 160, 300, ALLEGRO_ALIGN_CENTER, "Retornar ao Menu");
    al_flip_display();
}

// Função para verificar se um movimento entre duas casas é válido (adjacente ou salto)
bool movimento_valido(int origem, int destino, EstadoEspaço tabuleiro[7]) {
    // Matriz de adjacências diretas
    int adjacentes[7][4] = {
        {1, 2, 3, -1},    // 0
        {0, 2, 4, -1},    // 1
        {0, 1, 3, 5},     // 2
        {0, 2, 6, -1},    // 3
        {1, 5, -1, -1},   // 4
        {2, 4, 6, -1},    // 5
        {3, 5, -1, -1}    // 6
    };

    // Matriz de saltos possíveis: {origem, sobre, destino}
    int saltos[][3] = {
        {0, 2, 5}, {0, 1, 4}, {0, 3, 6},
        {1, 2, 3}, {1, 4, 5},
        {2, 0, 3}, {2, 5, 6}, {2, 1, 4},
        {3, 2, 1}, {3, 6, 5}, {3, 0, 1},
        {4, 1, 0}, {4, 5, 6},
        {5, 2, 0}, {5, 4, 1}, {5, 6, 3},
        {6, 3, 0}, {6, 5, 2}, {6, 4, 1}
    };
    int num_saltos = sizeof(saltos) / sizeof(saltos[0]);

    // Verifica adjacência direta
    for (int i = 0; i < 4; i++) {
        if (adjacentes[origem][i] == -1) break;
        if (adjacentes[origem][i] == destino) return true;
    }
    // Verifica salto sobre peça adversária
    for (int i = 0; i < num_saltos; i++) {
        if (saltos[i][0] == origem && saltos[i][2] == destino) {
            int sobre = saltos[i][1];
            if (tabuleiro[sobre] != VAZIO && tabuleiro[sobre] != tabuleiro[origem] && tabuleiro[destino] == VAZIO)
                return true;
        }
    }
    return false;
}

EstadoEspaço verificar_vitoria(EstadoEspaço tabuleiro[7]) {
    // Todas as combinações de 3 casas alinhadas possíveis
    int linhas[8][3] = {
        {0, 1, 4}, {0, 2, 5}, {0, 3, 6},
        {1, 2, 3}, {4, 5, 6}, {1, 5, 6},
        {3, 5, 4}, {2, 5, 3}
    };
    for (int i = 0; i < 8; i++) {
        int a = linhas[i][0], b = linhas[i][1], c = linhas[i][2];
        if (tabuleiro[a] != VAZIO &&
            tabuleiro[a] == tabuleiro[b] &&
            tabuleiro[a] == tabuleiro[c]) {
            return tabuleiro[a];
        }
    }
    return VAZIO;
}

void empate_final(ALLEGRO_FONT *font, int win_w, int win_h) {
    // Fundo semi-transparente
    al_draw_filled_rectangle(0, 0, win_w, win_h, al_map_rgba(0, 0, 0, 180));
    // Retângulo do popup
    al_draw_filled_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(240, 240, 255));
    al_draw_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(0,0,0), 3);

    // Mensagem
    al_draw_text(font, al_map_rgb(0, 120, 0), 364, 175, ALLEGRO_ALIGN_CENTER, "Empate! Ambos os jogadores concordaram em empatar.");
    al_draw_text(font, al_map_rgb(80, 80, 80), 364, 215, ALLEGRO_ALIGN_CENTER, "Clique para fechar");
    al_flip_display();

    // Espera um clique para fechar
    ALLEGRO_EVENT_QUEUE *popup_queue = al_create_event_queue();
    al_register_event_source(popup_queue, al_get_mouse_event_source());
    ALLEGRO_EVENT ev;
    while (1) {
        al_wait_for_event(popup_queue, &ev);
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
            break;
    }
    al_destroy_event_queue(popup_queue);
}

void empate_espera(ALLEGRO_FONT *font, int win_w, int win_h, int jogador_atual, RegistroPartida *historico, int *num_partidas) {
    // Fundo semi-transparente
    al_draw_filled_rectangle(0, 0, win_w, win_h, al_map_rgba(0, 0, 0, 180));
    
    // Retângulo do popup
    al_draw_filled_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(240, 240, 255));
    al_draw_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(0, 0, 0), 3);

    // Mensagem
    char msg[64];
    sprintf(msg, "Jogador %d pediu empate", jogador_atual);
    al_draw_text(font, al_map_rgb(200, 120, 0), 364, 175, ALLEGRO_ALIGN_CENTER, msg);
    al_draw_text(font, al_map_rgb(80, 80, 80), 364, 215, ALLEGRO_ALIGN_CENTER, "Aguardando o outro jogador confirmar");

    // Botão "Confirmar Empate"
    al_draw_filled_rounded_rectangle(264, 215, 464, 255, 10, 10, al_map_rgb(240, 255, 240));
    al_draw_rounded_rectangle(264, 215, 464, 255, 10, 10, al_map_rgb(0, 150, 0), 2);
    al_draw_text(font, al_map_rgb(0, 120, 0), 364, 225, ALLEGRO_ALIGN_CENTER, "Confirmar Empate");

    al_flip_display();

    // Espera o clique no botão "Confirmar Empate"
    ALLEGRO_EVENT_QUEUE *popup_queue = al_create_event_queue();
    al_register_event_source(popup_queue, al_get_mouse_event_source());
    ALLEGRO_EVENT ev;
    while (1) {
        al_wait_for_event(popup_queue, &ev);
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            float mx = ev.mouse.x;
            float my = ev.mouse.y;

            // Verifica se o clique foi dentro do botão "Confirmar Empate"
            if (mx >= 264 && mx <= 464 && my >= 215 && my <= 255) {
                printf("Empate confirmado pelo outro jogador.\n");

                // Chama a função empate_final
                empate_final(font, win_w, win_h);

                // Salva o histórico de empate
                strcpy(historico[*num_partidas].modo_jogo, "PvP");
                strcpy(historico[*num_partidas].resultado, "Empate");
                historico[*num_partidas].duracao = 0; // Pode ser ajustado para incluir a duração real
                (*num_partidas)++;

                salvar_historico("historico.txt", historico, *num_partidas);

                break; // Sai do loop e retorna ao jogo
            }
        }
    }
    al_destroy_event_queue(popup_queue);
}

// Função para exibir la tela de pausa do jogo
void pausar_jogo(ALLEGRO_FONT *font2, int win_w, int win_h, EstadoEspaço tabuleiro[7], int jogador, int pecas_jogador1, int pecas_jogador2, time_t inicio) {
    // Fundo semi-transparente
    al_draw_filled_rectangle(0, 0, win_w, win_h, al_map_rgba(0, 0, 0, 180));

    // Retângulo do popup
    al_draw_filled_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(240, 240, 255));
    al_draw_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(0, 0, 0), 3);

    // Mensagem "Jogo Pausado"
    al_draw_text(font2, al_map_rgb(255, 255, 255), 364, 155, ALLEGRO_ALIGN_CENTER, "Jogo Pausado");

    // Botão "Retornar ao Jogo"
    al_draw_filled_rounded_rectangle(264, 215, 464, 255, 10, 10, al_map_rgb(240, 240, 240));
    al_draw_rounded_rectangle(264, 215, 464, 255, 10, 10, al_map_rgb(0, 0, 0), 2);
    al_draw_text(font2, al_map_rgb(0, 0, 0), 364, 225, ALLEGRO_ALIGN_CENTER, "Retornar");

    al_flip_display();

    // Espera o clique no botão para retornar ao jogo
    ALLEGRO_EVENT_QUEUE *popup_queue = al_create_event_queue();
    al_register_event_source(popup_queue, al_get_mouse_event_source());
    ALLEGRO_EVENT ev;
    while (1) {
        al_wait_for_event(popup_queue, &ev);
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            float mx = ev.mouse.x;
            float my = ev.mouse.y;

            // Verifica se o clique foi dentro do botão "Retornar ao Jogo"
            if (mx >= 264 && mx <= 464 && my >= 215 && my <= 255) {
                printf("Retomando o Jogo\n");
                break; // Sai do loop e retorna ao jogo
            }
        }
    }
    al_destroy_event_queue(popup_queue);
}

void popup_sair(ALLEGRO_FONT *font2, int win_w, int win_h, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, bool *showing_menu) {
    // Fundo semi-transparente
    al_draw_filled_rectangle(0, 0, win_w, win_h, al_map_rgba(0, 0, 0, 180));

    // Retângulo do popup
    al_draw_filled_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(240, 240, 255));
    al_draw_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(0, 0, 0), 3);

    // Mensagem "Deseja sair?"
    al_draw_text(font2, al_map_rgb(255, 0, 0), 364, 155, ALLEGRO_ALIGN_CENTER, "Deseja sair do jogo?");

    // Botão "Sim"
    al_draw_filled_rounded_rectangle(264, 215, 364, 255, 10, 10, al_map_rgb(240, 240, 240));
    al_draw_rounded_rectangle(264, 215, 364, 255, 10, 10, al_map_rgb(0, 0, 0), 2);
    al_draw_text(font2, al_map_rgb(0, 0, 0), 314, 225, ALLEGRO_ALIGN_CENTER, "Sim");

    // Botão "Não"
    al_draw_filled_rounded_rectangle(364, 215, 464, 255, 10, 10, al_map_rgb(240, 240, 240));
    al_draw_rounded_rectangle(364, 215, 464, 255, 10, 10, al_map_rgb(0, 0, 0), 2);
    al_draw_text(font2, al_map_rgb(0, 0, 0), 414, 225, ALLEGRO_ALIGN_CENTER, "Não");

    al_flip_display();

    // Espera o clique no botão para decidir
    ALLEGRO_EVENT_QUEUE *popup_queue = al_create_event_queue();
    al_register_event_source(popup_queue, al_get_mouse_event_source());
    ALLEGRO_EVENT ev;
    while (1) {
        al_wait_for_event(popup_queue, &ev);
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            float mx = ev.mouse.x;
            float my = ev.mouse.y;

            // Verifica se o clique foi dentro do botão "Sim"
            if (mx >= 264 && mx <= 364 && my >= 215 && my <= 255) {
                printf("Retornando ao menu principal.\n");
                *showing_menu = true; // Retorna ao menu principal
                al_destroy_event_queue(popup_queue);
                return; // Sai do loop
            }

            // Verifica se o clique foi dentro do botão "Não"
            if (mx >= 364 && mx <= 464 && my >= 215 && my <= 255) {
                printf("Cancelando saída.\n");
                al_destroy_event_queue(popup_queue);
                return; // Sai do loop e retorna ao jogo
            }
        }
    }
    al_destroy_event_queue(popup_queue);
}




void exibir_vitoria(ALLEGRO_FONT *font, int win_w, int win_h, int vencedor, EstadoEspaço tabuleiro[7]) {
    // Fundo semi-transparente
    al_draw_filled_rectangle(0, 0, win_w, win_h, al_map_rgba(0, 0, 0, 180));
    
    // Retângulo do popup
    al_draw_filled_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(240, 240, 255));
    al_draw_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(0, 0, 0), 3);

    // Mensagem de vitória
    char msg[64];
    sprintf(msg, "Vencedor: Jogador %d", vencedor);
    al_draw_text(font, al_map_rgb(0, 120, 0), 364, 175, ALLEGRO_ALIGN_CENTER, msg);

    // Botão "Jogar Novamente"
    al_draw_filled_rounded_rectangle(264, 215, 464, 255, 10, 10, al_map_rgb(240, 240, 240));
    al_draw_rounded_rectangle(264, 215, 464, 255, 10, 10, al_map_rgb(0, 0, 0), 2);
    al_draw_text(font, al_map_rgb(0, 0, 0), 364, 225, ALLEGRO_ALIGN_CENTER, "Jogar Novamente");

    al_flip_display();

    // Espera o clique no botão para retornar ao jogo
    ALLEGRO_EVENT_QUEUE *popup_queue = al_create_event_queue();
    al_register_event_source(popup_queue, al_get_mouse_event_source());
    ALLEGRO_EVENT ev;
    while (1) {
        al_wait_for_event(popup_queue, &ev);
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            float mx = ev.mouse.x;
            float my = ev.mouse.y;

            // Verifica se o clique foi dentro do botão "Jogar Novamente"
            if (mx >= 264 && mx <= 464 && my >= 215 && my <= 255) {
                printf("Reiniciando Partida.\n");

                // Reinicia o tabuleiro
                for (int i = 0; i < 7; i++) {
                    tabuleiro[i] = VAZIO;
                }

                break; // Sai do loop e retorna ao jogo
            }
        }
    }
    al_destroy_event_queue(popup_queue);
}



// Função para implementar IA simples
int ia_escolher_jogada(EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7], int jogador_ia) {
    // 1. Tenta vencer
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == VAZIO) {
            tabuleiro[i] = jogador_ia;
            if (verificar_vitoria(tabuleiro) == jogador_ia) {
                tabuleiro[i] = VAZIO;
                return i;
            }
            tabuleiro[i] = VAZIO;
        }
    }

    // 2. Tenta bloquear
    int oponente = (jogador_ia == JOGADOR1) ? JOGADOR2 : JOGADOR1;
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == VAZIO) {
            tabuleiro[i] = oponente;
            if (verificar_vitoria(tabuleiro) == oponente) {
                tabuleiro[i] = VAZIO;
                return i;
            }
            tabuleiro[i] = VAZIO;
        }
    }

    // 3. Prioriza posições estratégicas
    if (tabuleiro[2] == VAZIO) return 2; // Centro
    if (tabuleiro[5] == VAZIO) return 5; // Centro inferior

    // 4. Joga aleatoriamente
    int posicoes_validas[7];
    int num_validas = 0;
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == VAZIO) {
            posicoes_validas[num_validas++] = i;
        }
    }
    if (num_validas > 0) {
        return posicoes_validas[rand() % num_validas];
    }
    return -1; // Sem jogada válida
}

// Função para IA escolher movimento
bool ia_escolher_movimento(EstadoEspaço tabuleiro[7], int jogador_ia, int *origem, int *destino) {
    // 1. Tenta vencer
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == jogador_ia) {
            for (int j = 0; j < 7; j++) {
                if (movimento_valido(i, j, tabuleiro)) {
                    EstadoEspaço temp = tabuleiro[j];
                    tabuleiro[j] = tabuleiro[i];
                    tabuleiro[i] = VAZIO;
                    if (verificar_vitoria(tabuleiro) == jogador_ia) {
                        tabuleiro[i] = tabuleiro[j];
                        tabuleiro[j] = temp;
                        *origem = i;
                        *destino = j;
                        return true;
                    }
                    tabuleiro[i] = tabuleiro[j];
                    tabuleiro[j] = temp;
                }
            }
        }
    }

    // 2. Tenta bloquear
    int oponente = (jogador_ia == JOGADOR1) ? JOGADOR2 : JOGADOR1;
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == jogador_ia) {
            for (int j = 0; j < 7; j++) {
                if (movimento_valido(i, j, tabuleiro)) {
                    EstadoEspaço temp = tabuleiro[j];
                    tabuleiro[j] = tabuleiro[i];
                    tabuleiro[i] = VAZIO;
                    bool bloqueia = false;
                    for (int k = 0; k < 7; k++) {
                        if (tabuleiro[k] == VAZIO) {
                            tabuleiro[k] = oponente;
                            if (verificar_vitoria(tabuleiro) == oponente) {
                                bloqueia = true;
                            }
                            tabuleiro[k] = VAZIO;
                            if (bloqueia) break;
                        }
                    }
                    tabuleiro[i] = tabuleiro[j];
                    tabuleiro[j] = temp;
                    if (bloqueia) {
                        *origem = i;
                        *destino = j;
                        return true;
                    }
                }
            }
        }
    }

    // 3. Prioriza posições estratégicas
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == jogador_ia) {
            for (int j = 0; j < 7; j++) {
                if (movimento_valido(i, j, tabuleiro) && (j == 2 || j == 5)) { // Centro ou centro inferior
                    *origem = i;
                    *destino = j;
                    return true;
                }
            }
        }
    }

    // 4. Move aleatoriamente
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == jogador_ia) {
            for (int j = 0; j < 7; j++) {
                if (movimento_valido(i, j, tabuleiro)) {
                    *origem = i;
                    *destino = j;
                    return true;
                }
            }
        }
    }
    return false;
}

void posicionamento(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2, int *placar_j1, int *placar_j2) {
   
    time_t inicio = iniciar_timer();
    int jogador_atual = JOGADOR1;
    int pecas_jogador1 = 0, pecas_jogador2 = 0;
    bool posicionando = true;
    float mx = 0, my = 0;

    while (posicionando) {
        bool showing_menu = false;
        desenha_tabuleiroTriangulo(background, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, peca_player1, peca_player2, mx, my, *placar_j1, *placar_j2, &showing_menu);
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mx = event.mouse.x;
            my = event.mouse.y;
        }

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            float mx_click = event.mouse.x;
            float my_click = event.mouse.y;

            if (mx_click >= 50 && mx_click <= 180 && my_click >= 378 && my_click <= 402) {
                pausar_jogo(font2, win_w, win_h, tabuleiro, jogador_atual, pecas_jogador1, pecas_jogador2, inicio);
                continue;
            }

            for (int i = 0; i < 7; i++) {
                float dx = mx_click - pontos_tabuleiro[i].x;
                float dy = my_click - pontos_tabuleiro[i].y;
                if (dx*dx + dy*dy <= 25*25 && tabuleiro[i] == VAZIO) {
                    if ((jogador_atual == JOGADOR1 && pecas_jogador1 < 3) ||
                        (jogador_atual == JOGADOR2 && pecas_jogador2 < 3)) {
                        tabuleiro[i] = jogador_atual;
                        if (jogador_atual == JOGADOR1) pecas_jogador1++;
                        else pecas_jogador2++;

                        EstadoEspaço vencedor = verificar_vitoria(tabuleiro);
                       if (vencedor != VAZIO) {
                            exibir_vitoria(font, win_w, win_h, vencedor, tabuleiro);
                            if (vencedor == JOGADOR1) (*placar_j1)++;
                            else if (vencedor == JOGADOR2) (*placar_j2)++;
                            return;
                        }

                        jogador_atual = (jogador_atual == JOGADOR1) ? JOGADOR2 : JOGADOR1;
                    }
                    break;
                }
            }
        }
        if (pecas_jogador1 == 3 && pecas_jogador2 == 3) {
            posicionando = false;
        }
    }

    double duracao = calcular_duracao(inicio);
    printf("Duração da partida: %.2f segundos\n", duracao);

}

void posicionamento_pvm(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2, int *placar_j1, int *placar_j2, int jogador_humano, int jogador_ia) {
   
    time_t inicio = iniciar_timer();
    int pecas_jogador1 = 0, pecas_jogador2 = 0;
    bool posicionando = true;
    float mx = 0, my = 0;
    int jogador_atual = jogador_humano; // Começa com o jogador humano

    while (posicionando) {
        bool showing_menu = false;
        desenha_tabuleiroTriangulo(background, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, peca_player1, peca_player2, mx, my, *placar_j1, *placar_j2, &showing_menu);
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mx = event.mouse.x;
            my = event.mouse.y;
        }

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && jogador_atual == jogador_humano) {
            float mx_click = event.mouse.x;
            float my_click = event.mouse.y;

            if (mx_click >= 50 && mx_click <= 180 && my_click >= 378 && my_click <= 402) {
                pausar_jogo(font2, win_w, win_h, tabuleiro, jogador_humano, pecas_jogador1, pecas_jogador2, inicio);
                continue;
            }

            // Jogador humano posiciona suas peças
            if (pecas_jogador1 < 3) {
                for (int i = 0; i < 7; i++) {
                    float dx = mx_click - pontos_tabuleiro[i].x;
                    float dy = my_click - pontos_tabuleiro[i].y;
                    if (dx * dx + dy * dy <= 25 * 25 && tabuleiro[i] == VAZIO) {
                        tabuleiro[i] = jogador_humano;
                        pecas_jogador1++;
                        jogador_atual = jogador_ia; // Alterna para o jogador IA
                        break;
                    }
                }
            }
        }

        // Jogador IA posiciona suas peças automaticamente
        if (jogador_atual == jogador_ia && pecas_jogador2 < 3) {
            int posicao = ia_escolher_jogada(tabuleiro, pontos_tabuleiro, jogador_ia);
            if (posicao != -1) {
                tabuleiro[posicao] = jogador_ia;
                pecas_jogador2++;
                jogador_atual = jogador_humano; // Alterna para o jogador humano
            }
        }

        // Finaliza o posicionamento após ambos os jogadores terem colocado 3 peças
        if (pecas_jogador1 == 3 && pecas_jogador2 == 3) {
            posicionando = false;
        }
    }

    double duracao = calcular_duracao(inicio);
    printf("Duração da partida: %.2f segundos\n", duracao);
}


void movimentacao(
    ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2,
    int *placar_j1, int *placar_j2,
    RegistroPartida *historico, int *num_partidas) {

    bool empate_pedido[2] = {false, false};
    int jogador = JOGADOR1;
    int selecionada = -1;
    float mouse_x = 0, mouse_y = 0;

    while (1) {
        bool showing_menu = false;
        desenha_tabuleiroTriangulo(background, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, peca_player1, peca_player2, mouse_x, mouse_y, *placar_j1, *placar_j2, &showing_menu);
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mouse_x = ev.mouse.x;
            mouse_y = ev.mouse.y;
        }

        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            float mx = ev.mouse.x;
            float my = ev.mouse.y;

            if (mx >= 50 && mx <= 180 && my >= 378 && my <= 402) {
                pausar_jogo(font2, win_w, win_h, tabuleiro, jogador, *placar_j1, *placar_j2, time(NULL));
                continue;
            }

            if (mx >= 478 && mx <= 598 && my >= 378 && my <= 402) { // Botão "Empate"
                empate_pedido[jogador - 1] = true; // Marca que o jogador atual pediu empate

                if (empate_pedido[0] && empate_pedido[1]) { // Ambos os jogadores pediram empate
                    empate_espera(font, win_w, win_h, jogador, historico, num_partidas); // Chama empate_espera
                    return; // Termina a partida
                }
                continue; // Continua o loop
            }

            // Verifica se o jogador selecionou uma peça
            if (selecionada == -1) {
                for (int i = 0; i < 7; i++) {
                    float dx = mx - pontos_tabuleiro[i].x;
                    float dy = my - pontos_tabuleiro[i].y;
                    if (dx * dx + dy * dy <= 25 * 25 && tabuleiro[i] == jogador) {
                        selecionada = i;
                        break;
                    }
                }
            } else { // Jogador já selecionou uma peça, tenta mover
                for (int i = 0; i < 7; i++) {
                    float dx = mx - pontos_tabuleiro[i].x;
                    float dy = my - pontos_tabuleiro[i].y;
                    if (dx * dx + dy * dy <= 25 * 25 && tabuleiro[i] == VAZIO) {
                        if (movimento_valido(selecionada, i, tabuleiro)) {
                            tabuleiro[i] = jogador;
                            tabuleiro[selecionada] = VAZIO;

                            EstadoEspaço vencedor = verificar_vitoria(tabuleiro);
                            if (vencedor != VAZIO) {
                                exibir_vitoria(font, win_w, win_h, vencedor, tabuleiro);
                                if (vencedor == JOGADOR1) (*placar_j1)++;
                                else if (vencedor == JOGADOR2) (*placar_j2)++;
                                return;
                            }

                            selecionada = -1;
                            jogador = (jogador == JOGADOR1) ? JOGADOR2 : JOGADOR1;
                        } else {
                            selecionada = -1;
                        }
                        break;
                    }
                }
                selecionada = -1;
            }
        }
    }
}

void movimentacao_pvm(
    ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2,
    int *placar_j1, int *placar_j2, int jogador_humano, int jogador_ia,
    RegistroPartida *historico, int *num_partidas) {

    int jogador = jogador_humano; // Começa com o jogador humano
    int selecionada = -1;
    float mouse_x = 0, mouse_y = 0;

    while (1) {
        bool showing_menu = false;
        desenha_tabuleiroTriangulo(background, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, peca_player1, peca_player2, mouse_x, mouse_y, *placar_j1, *placar_j2, &showing_menu);
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mouse_x = ev.mouse.x;
            mouse_y = ev.mouse.y;
        }

        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            float mx = ev.mouse.x;
            float my = ev.mouse.y;

            if (mx >= 478 && mx <= 598 && my >= 378 && my <= 402) { // Botão "Empate"
                empate_espera(font, win_w, win_h, jogador, historico, num_partidas); // Chama empate_espera
                return; // Termina a partida
            }

            // Jogador humano faz sua jogada
            if (jogador == jogador_humano) {
                if (selecionada == -1) {
                    for (int i = 0; i < 7; i++) {
                        float dx = mx - pontos_tabuleiro[i].x;
                        float dy = my - pontos_tabuleiro[i].y;
                        if (dx * dx + dy * dy <= 25 * 25 && tabuleiro[i] == jogador) {
                            selecionada = i;
                            break;
                        }
                    }
                } else {
                    for (int i = 0; i < 7; i++) {
                        float dx = mx - pontos_tabuleiro[i].x;
                        float dy = my - pontos_tabuleiro[i].y;
                        if (dx * dx + dy * dy <= 25 * 25 && tabuleiro[i] == VAZIO) {
                            if (movimento_valido(selecionada, i, tabuleiro)) {
                                tabuleiro[i] = jogador;
                                tabuleiro[selecionada] = VAZIO;

                                EstadoEspaço vencedor = verificar_vitoria(tabuleiro);
                                if (vencedor != VAZIO) {
                                    exibir_vitoria(font, win_w, win_h, vencedor, tabuleiro);
                                    if (vencedor == JOGADOR1) (*placar_j1)++;
                                    else if (vencedor == JOGADOR2) (*placar_j2)++;
                                    return;
                                }

                                selecionada = -1;
                                jogador = jogador_ia; // Passa a vez para o computador
                            } else {
                                selecionada = -1;
                            }
                            break;
                        }
                    }
                    selecionada = -1;
                }
            }

            // Computador faz sua jogada automaticamente
            if (jogador == jogador_ia) {
                int origem, destino;
                if (ia_escolher_movimento(tabuleiro, jogador_ia, &origem, &destino)) {
                    tabuleiro[destino] = jogador_ia;
                    tabuleiro[origem] = VAZIO;

                    EstadoEspaço vencedor = verificar_vitoria(tabuleiro);
                    if (vencedor != VAZIO) {
                        exibir_vitoria(font, win_w, win_h, vencedor, tabuleiro);
                        if (vencedor == JOGADOR1) (*placar_j1)++;
                        else if (vencedor == JOGADOR2) (*placar_j2)++;
                        return;
                    }

                    jogador = jogador_humano; // Passa a vez para o jogador humano
                }
            }
        }
    }
}



void salvar_historico(const char *arquivo, RegistroPartida *partidas, int num_partidas) {
    FILE *fp = fopen(arquivo, "w");
    if (!fp) {
        fprintf(stderr, "Erro ao abrir arquivo para salvar histórico!\n");
        return;
    }

    for (int i = 0; i < num_partidas; i++) {
        fprintf(fp, "Modo: %s | Resultado: %s | Duração: %.2f segundos\n",
                partidas[i].modo_jogo, partidas[i].resultado, partidas[i].duracao);
    }

    fclose(fp);
}

void carregar_historico(const char *arquivo) {
    FILE *fp = fopen(arquivo, "r");
    if (!fp) {
        fprintf(stderr, "Erro ao abrir arquivo para carregar histórico!\n");
        return;
    }

    char linha[256];
    while (fgets(linha, sizeof(linha), fp)) {
        printf("%s", linha);
    }

    fclose(fp);
}


time_t iniciar_timer() {
    return time(NULL); // Retorna o tempo atual em segundos desde 01/01/1970
}

double calcular_duracao(time_t inicio) {
    time_t agora = time(NULL); // Obtém o tempo atual
    return difftime(agora, inicio); // Calcula a diferença em segundos
}























// Função principal
int main(void) {

    RegistroPartida historico[100];
    int num_partidas = 0;


    int placar_j1 = 0, placar_j2 = 0;

    EstadoEspaço tabuleiro[7];
    PontoTabuleiro pontos_tabuleiro[7] = {
        {364, 80},   // 0 - topo
        {214, 180},  // 1 - esquerda superior
        {364, 180},  // 2 - centro
        {514, 180},  // 3 - direita superior
        {114, 330},  // 4 - esquerda inferior
        {364, 330},  // 5 - centro inferior
        {614, 330}   // 6 - direita inferior
    };

    ALLEGRO_DISPLAY *janela = NULL;
    ALLEGRO_FONT *font = NULL;
    ALLEGRO_FONT *font2 = NULL;

    if (!al_init()) {
        fprintf(stderr, "Falha ao inicializar Allegro!\n");
        return -1;
    }
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();

    if (!al_install_mouse()) {
        fprintf(stderr, "Falha ao inicializar o mouse!\n");
        return -1;
    }

    font = al_load_ttf_font("Content/alice.ttf", 24, 0);
    if (!font) {
        fprintf(stderr, "Falha ao carregar fonte alice.ttf!\n");
        return -1;
    }

    font2 = al_load_ttf_font("Content/mine.otf", 24, 0);
    if (!font2) {
        fprintf(stderr, "Falha ao carregar fonte mine.otf!\n");
        al_destroy_font(font);
        return -1;
    }

    janela = al_create_display(728, 410);
    if (!janela) {
        fprintf(stderr, "Falha ao criar janela!\n");
        al_destroy_font(font);
        al_destroy_font(font2);
        return -1;
    }

    al_set_window_title(janela, "Tri Angle");
    al_init_image_addon();
    ALLEGRO_BITMAP *background = al_load_bitmap("Content/background.jpg");
    if (!background) {
        fprintf(stderr, "Falha ao carregar imagem de fundo!\n");
        al_destroy_display(janela);
        al_destroy_font(font);
        al_destroy_font(font2);
        return -1;
    }

    int win_w = al_get_display_width(janela);
    int win_h = al_get_display_height(janela);

    desenha_menu1(background, font);

    int jogar_x = 160, jogar_y = 125, jogar_w = 100, jogar_h = 30;
    int ajuda_x = 160, ajuda_y = 150, ajuda_w = 100, ajuda_h = 30;
    int historico_x = 160, historico_y = 175, historico_w = 100, historico_h = 30;
    int sair_x = 160, sair_y = 200, sair_w = 100, sair_h = 30;

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    if (!queue) {
        fprintf(stderr, "Falha ao criar fila de eventos!\n");
        al_destroy_display(janela);
        al_destroy_font(font);
        al_destroy_font(font2);
        al_destroy_bitmap(background);
        return -1;
    }

    ALLEGRO_BITMAP *peca_player1 = al_load_bitmap("Content/player1.png");
    ALLEGRO_BITMAP *peca_player2 = al_load_bitmap("Content/player2.png");
    if (!peca_player1 || !peca_player2) {
        fprintf(stderr, "Falha ao carregar imagens das peças!\n");
        al_destroy_event_queue(queue);
        al_destroy_display(janela);
        al_destroy_font(font);
        al_destroy_font(font2);
        al_destroy_bitmap(background);
        return -1;
    }

    al_register_event_source(queue, al_get_display_event_source(janela));
    al_register_event_source(queue, al_get_mouse_event_source());

    bool showing_menu = true;
    bool showing_play = false;
    bool showing_help = false;
    bool showing_history = false;

    ALLEGRO_BITMAP *help_image = NULL;
    ALLEGRO_BITMAP *play_image = NULL;
    ALLEGRO_BITMAP *History_image = NULL;
    ALLEGRO_BITMAP *Chess_background_image = NULL;

    int mouse_x = 0, mouse_y = 0;
    bool running = true;

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            mouse_x = event.mouse.x;
            mouse_y = event.mouse.y;

            // 1. Menu Principal
            if (!showing_play && !showing_help && !showing_history) {
                // Botão "Jogar"
                if (mouse_x >= jogar_x - jogar_w / 2 && mouse_x <= jogar_x + jogar_w / 2 &&
                    mouse_y >= jogar_y && mouse_y <= jogar_y + jogar_h) {
                    printf("Jogar selecionado - Mostrando Modos.\n");
                    play_image = al_load_bitmap("Content/Modos.png");
                    if (play_image) {
                        desenha_modos2(background, play_image, font, font2, win_w, win_h);
                        showing_menu = false;
                        showing_play = true;
                    }
                }
                // Botão "Ajuda"
                else if (mouse_x >= ajuda_x - ajuda_w / 2 && mouse_x <= ajuda_x + ajuda_w / 2 &&
                         mouse_y >= ajuda_y && mouse_y <= ajuda_y + ajuda_h) {
                    help_image = al_load_bitmap("Content/help.png");
                    if (help_image) {
                        desenha_ajuda3(background, help_image, font, win_w, win_h);
                        showing_menu = false;
                        showing_help = true;
                    }
                }
                // Botão "Histórico"
                else if (mouse_x >= historico_x - historico_w / 2 && mouse_x <= historico_x + historico_w / 2 &&
                         mouse_y >= historico_y && mouse_y <= historico_y + historico_h) {
                    History_image = al_load_bitmap("Content/historico.png");
                    if (History_image) {
                        printf("Historico selecionado.\n");
                        desenha_historico4(background, History_image, font, win_w, win_h);
                        showing_menu = false;
                        showing_history = true;
                    }
                }
                // Botão "Sair"
                else if (mouse_x >= sair_x - sair_h / 2 && mouse_x <= sair_x + sair_w / 2 &&
                         mouse_y >= sair_y && mouse_y <= sair_y + sair_h) {
                    running = false;
                }
            }
            // 2. Tela de Modos de Jogo
            else if (showing_play) {
                int pvp_x = 150, pvp_y = 310, pvp_w = 250, pvp_h = al_get_font_line_height(font2);
                int pvm_x = 585, pvm_y = 310, pvm_w = 250, pvm_h = al_get_font_line_height(font2);
                int rmenu_x = 370, rmenu_y = 300, rmenu_w = 200, rmenu_h = 40;

                // Botão "PVP"
                if (mouse_x >= pvp_x - pvp_w / 2 && mouse_x <= pvp_x + pvp_w / 2 && mouse_y >= pvp_y && mouse_y <= pvp_y + pvp_h) {
                    limpaTela();
                    printf("Modo PVP selecionado.\n");
                    for (int i = 0; i < 7; i++) tabuleiro[i] = VAZIO;
                    Chess_background_image = al_load_bitmap("Content/boardbackground.jpeg");
                    if (!Chess_background_image) Chess_background_image = background;

                    posicionamento(queue, Chess_background_image, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, peca_player1, peca_player2, &placar_j1, &placar_j2);
                    movimentacao(queue, Chess_background_image, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, peca_player1, peca_player2, &placar_j1, &placar_j2, historico, &num_partidas);

                    if (Chess_background_image != background) al_destroy_bitmap(Chess_background_image);
                    desenha_menu1(background, font); // Retorna ao menu após o jogo
                    showing_play = false;
                }
                // Botão "PVM"
                else if (mouse_x >= pvm_x - pvm_w / 2 && mouse_x <= pvm_x + pvm_w / 2 && mouse_y >= pvm_y && mouse_y <= pvm_y + pvm_h) {
                    printf("Modo PVM selecionado.\n");

                        // Inicializa o tabuleiro
                        for (int i = 0; i < 7; i++) tabuleiro[i] = VAZIO;

                        // Define os jogadores
                        int jogador_humano = JOGADOR1;
                        int jogador_ia = JOGADOR2;

                        // Carrega o fundo do tabuleiro
                        Chess_background_image = al_load_bitmap("Content/boardbackground.jpeg");
                        if (!Chess_background_image) Chess_background_image = background;

                        // Chama a função movimentacao_pvm
                        // Dentro do modo PvM
                        posicionamento_pvm(queue, Chess_background_image, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, peca_player1, peca_player2, &placar_j1, &placar_j2, jogador_humano, jogador_ia);
                        movimentacao_pvm(queue, Chess_background_image, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, peca_player1, peca_player2, &placar_j1, &placar_j2, jogador_humano, jogador_ia, historico, &num_partidas);

                        // Retorna ao menu após o jogo
                        if (Chess_background_image != background) al_destroy_bitmap(Chess_background_image);
                        desenha_menu1(background, font);
                        showing_play = false;
                }
                // Botão "Retornar ao Menu"
                else if (mouse_x >= rmenu_x - rmenu_w / 2 && mouse_x <= rmenu_x + rmenu_w / 2 && mouse_y >= rmenu_y && mouse_y <= rmenu_y + rmenu_h) {
                    printf("Retornando ao menu principal\n");
                    desenha_menu1(background, font);
                    if (play_image) al_destroy_bitmap(play_image);
                    play_image = NULL;
                    showing_play = false;
                    showing_menu = true;
                }
            }
            // 3. Tela de Ajuda
            else if (showing_help) {
                int rmenu_x = 160, rmenu_y = 300, rmenu_w = 200, rmenu_h = 40;
                if (mouse_x >= rmenu_x - rmenu_w / 2 && mouse_x <= rmenu_x + rmenu_w / 2 && mouse_y >= rmenu_y && mouse_y <= rmenu_y + rmenu_h) {
                    printf("Retornando ao menu principal.\n");
                    desenha_menu1(background, font);
                    al_destroy_bitmap(help_image);
                    help_image = NULL;
                    showing_help = false;
                    showing_menu = true;

                }
            }
            // 4. Tela de Histórico
            else if (showing_history) {
                // Carrega o histórico de partidas
                carregar_historico("historico.txt");
                desenha_historico4(background, History_image, font, win_w, win_h);

                int rmenu_x = 160, rmenu_y = 300, rmenu_w = 200, rmenu_h = 40;
                if (mouse_x >= rmenu_x - rmenu_w / 2 && mouse_x <= rmenu_x + rmenu_w / 2 && mouse_y >= rmenu_y && mouse_y <= rmenu_y + rmenu_h) {
                    printf("Retornando ao menu principal.\n");
                    desenha_menu1(background, font);
                    al_destroy_bitmap(History_image);
                    showing_menu = true;
                    showing_play = false;
                    History_image = NULL;
                    showing_history = false;
                }
            }
        }
    }

    // Libera recursos
    if (help_image) al_destroy_bitmap(help_image);
    if (play_image) al_destroy_bitmap(play_image);
    if (History_image) al_destroy_bitmap(History_image);
    if (background) al_destroy_bitmap(background);
    if (peca_player1) al_destroy_bitmap(peca_player1);
    if (peca_player2) al_destroy_bitmap(peca_player2);
    al_destroy_event_queue(queue);
    al_destroy_display(janela);
    al_destroy_font(font);
    al_destroy_font(font2);
    al_uninstall_mouse();

    return 0;
}