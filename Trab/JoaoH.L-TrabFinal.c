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
    char modo_jogo[30];
    char resultado[30];
    double duracao;
} RegistroPartida;

typedef enum {
    VAZIO,
    JOGADOR1,
    JOGADOR2
} EstadoEspaço;

typedef struct {
    float x, y;
} PontoTabuleiro;

void limpaTela();
void salvar_partida_atual_txt(const char *arquivo, EstadoEspaço tabuleiro[7], int jogador_atual, int p1, int p2, double tempo);
bool carregar_partida_txt(const char *arquivo, EstadoEspaço tabuleiro[7], int *jogador_atual, int *p1, int *p2, double *tempo);
void salvar_historico(const char *arquivo, RegistroPartida *partidas, int num_partidas);
void carregar_historico(const char *arquivo);
void pecas_jogadores(ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h, int pecas_jogador1, int pecas_jogador2);
void desenha_tabuleiroTriangulo(ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2,
    float mouse_x, float mouse_y, int placar_j1, int placar_j2, bool *showing_menu);
bool processar_clique_botao(
    float mx, float my, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    ALLEGRO_BITMAP *background, EstadoEspaço tabuleiro[7], int jogador, int *placar_j1, int *placar_j2,
    RegistroPartida *historico, int *num_partidas, bool *showing_menu, time_t inicio
);
void desenha_menu1(ALLEGRO_BITMAP *background, ALLEGRO_FONT *font);
void desenha_modos2(ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *play_image, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h);
void desenha_ajuda3(ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *help_image, ALLEGRO_FONT *font, int win_w, int win_h);
void desenha_historico4(ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *History_image, ALLEGRO_FONT *font, int win_w, int win_h);

bool executar_partida_pvp(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2,
    int win_w, int win_h, EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2, int *placar_j1, int *placar_j2,
    RegistroPartida *historico, int *num_partidas);

bool executar_partida_pvm(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2,
    int win_w, int win_h, EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2, int *placar_j1, int *placar_j2,
    RegistroPartida *historico, int *num_partidas);

bool movimento_valido(int origem, int destino, EstadoEspaço tabuleiro[7]);
EstadoEspaço verificar_vitoria(EstadoEspaço tabuleiro[7]);
void empate_final(ALLEGRO_FONT *font, int win_w, int win_h);
bool empate_espera(ALLEGRO_FONT *font, int win_w, int win_h, int jogador_atual, RegistroPartida *historico, int *num_partidas, bool *showing_menu);
void pausar_jogo(ALLEGRO_FONT *font2, int win_w, int win_h, EstadoEspaço tabuleiro[7], int jogador, int pecas_jogador1, int pecas_jogador2, time_t inicio);
void popup_sair(ALLEGRO_FONT *font2, int win_w, int win_h, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, bool *showing_menu);
bool exibir_vitoria(ALLEGRO_FONT *font, int win_w, int win_h, int vencedor, EstadoEspaço tabuleiro[7]);
int ia_escolher_jogada(EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7], int jogador_ia);
bool ia_escolher_movimento(EstadoEspaço tabuleiro[7], int jogador_ia, int *origem, int *destino);

bool posicionamento(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    EstadoEspaço *tabuleiro, PontoTabuleiro *pontos_tabuleiro,
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2, int *placar_j1, int *placar_j2,
    RegistroPartida *historico, int *num_partidas, bool *jogar_novamente);

bool posicionamento_pvm(
    ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    EstadoEspaço *tabuleiro, PontoTabuleiro *pontos_tabuleiro,
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2, int *placar_j1, int *placar_j2, int jogador_humano, int jogador_ia,
    RegistroPartida *historico, int *num_partidas, bool *jogar_novamente);

void movimentacao(
    ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    EstadoEspaço *tabuleiro, PontoTabuleiro *pontos_tabuleiro,
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2,
    int *placar_j1, int *placar_j2,
    RegistroPartida *historico, int *num_partidas, bool *jogar_novamente);

void movimentacao_pvm(
    ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    EstadoEspaço *tabuleiro, PontoTabuleiro *pontos_tabuleiro,
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2,
    int *placar_j1, int *placar_j2, int jogador_humano, int jogador_ia,
    RegistroPartida *historico, int *num_partidas,
    bool *jogar_novamente);

time_t iniciar_timer();
double calcular_duracao(time_t inicio);

void limpaTela() {
    system(CLEAR);
}

void salvar_partida_atual_txt(const char *arquivo, EstadoEspaço tabuleiro[7], int jogador_atual, int p1, int p2, double tempo) {
    FILE *f = fopen(arquivo, "w");
    if (!f) {
        fprintf(stderr, "Erro ao abrir arquivo para salvar estado do jogo!\n");
        return;
    }

    
    for (int i = 0; i < 7; i++) {
        fprintf(f, "%d ", tabuleiro[i]);
    }
    fprintf(f, "\n");

    
    fprintf(f, "%d %d %d %.2f\n", jogador_atual, p1, p2, tempo);

    fclose(f);
}

bool carregar_partida_txt(const char *arquivo, EstadoEspaço tabuleiro[7], int *jogador_atual, int *p1, int *p2, double *tempo) {
    FILE *f = fopen(arquivo, "r");
    if (!f) {
        fprintf(stderr, "Erro ao abrir arquivo para carregar estado do jogo!\n");
        return false;
    }

    
    for (int i = 0; i < 7; i++) {
        fscanf(f, "%d", (int *)&tabuleiro[i]);
    }

    
    fscanf(f, "%d %d %d %lf", jogador_atual, p1, p2, tempo);

    fclose(f);
    return true;
}

void pecas_jogadores(ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h, int pecas_jogador1, int pecas_jogador2) {
    
    al_draw_text(font, al_map_rgb(255, 0, 0), 30, 10, ALLEGRO_ALIGN_LEFT, "Jogador 1");
    
    al_draw_text(font, al_map_rgb(0, 0, 255), win_w - 30, 10, ALLEGRO_ALIGN_RIGHT, "Jogador 2");

    ALLEGRO_BITMAP *pecapng_player1 = al_load_bitmap("Content/player1.png");
    ALLEGRO_BITMAP *pecapng_player2 = al_load_bitmap("Content/player2.png");

    
    for (int i = 0; i < 3; i++) {
        if (i < pecas_jogador1 && pecapng_player1) {
            al_draw_scaled_bitmap(
                pecapng_player1, 0, 0,
                al_get_bitmap_width(pecapng_player1), al_get_bitmap_height(pecapng_player1),
                28 + i * 30, 28, 24, 24, 0
            );
        } else {
            al_draw_filled_circle(40 + i * 30, 40, 12, al_map_rgb(200, 200, 200));
            al_draw_circle(40 + i * 30, 40, 12, al_map_rgb(0, 0, 0), 2);
        }
    }

    
    for (int i = 0; i < 3; i++) {
        if (i < pecas_jogador2 && pecapng_player2) {
            al_draw_scaled_bitmap(
                pecapng_player2, 0, 0,
                al_get_bitmap_width(pecapng_player2), al_get_bitmap_height(pecapng_player2),
                win_w - 52 - i * 30, 28, 24, 24, 0
            );
        } else {
            al_draw_filled_circle(win_w - 40 - i * 30, 40, 12, al_map_rgb(200, 200, 200));
            al_draw_circle(win_w - 40 - i * 30, 40, 12, al_map_rgb(0, 0, 0), 2);
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
    
    al_draw_bitmap(background, 0, 0, 0);

    
    int pecas_jogador1 = 0, pecas_jogador2 = 0;
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == JOGADOR1) pecas_jogador1++;
        if (tabuleiro[i] == JOGADOR2) pecas_jogador2++;
    }

    pecas_jogadores(font, font2, win_w, win_h, pecas_jogador1, pecas_jogador2);

    
    al_draw_line(pontos_tabuleiro[0].x, pontos_tabuleiro[0].y, pontos_tabuleiro[1].x, pontos_tabuleiro[1].y, al_map_rgb(0, 0, 0), 3);
    al_draw_line(pontos_tabuleiro[0].x, pontos_tabuleiro[0].y, pontos_tabuleiro[2].x, pontos_tabuleiro[2].y, al_map_rgb(0, 0, 0), 3);
    al_draw_line(pontos_tabuleiro[0].x, pontos_tabuleiro[0].y, pontos_tabuleiro[3].x, pontos_tabuleiro[3].y, al_map_rgb(0, 0, 0), 3);
    al_draw_line(pontos_tabuleiro[1].x, pontos_tabuleiro[1].y, pontos_tabuleiro[2].x, pontos_tabuleiro[2].y, al_map_rgb(0, 0, 0), 3);
    al_draw_line(pontos_tabuleiro[2].x, pontos_tabuleiro[2].y, pontos_tabuleiro[3].x, pontos_tabuleiro[3].y, al_map_rgb(0, 0, 0), 3);
    al_draw_line(pontos_tabuleiro[1].x, pontos_tabuleiro[1].y, pontos_tabuleiro[4].x, pontos_tabuleiro[4].y, al_map_rgb(0, 0, 0), 3);
    al_draw_line(pontos_tabuleiro[2].x, pontos_tabuleiro[2].y, pontos_tabuleiro[5].x, pontos_tabuleiro[5].y, al_map_rgb(0, 0, 0), 3);
    al_draw_line(pontos_tabuleiro[3].x, pontos_tabuleiro[3].y, pontos_tabuleiro[6].x, pontos_tabuleiro[6].y, al_map_rgb(0, 0, 0), 3);
    al_draw_line(pontos_tabuleiro[4].x, pontos_tabuleiro[4].y, pontos_tabuleiro[5].x, pontos_tabuleiro[5].y, al_map_rgb(0, 0, 0), 3);
    al_draw_line(pontos_tabuleiro[5].x, pontos_tabuleiro[5].y, pontos_tabuleiro[6].x, pontos_tabuleiro[6].y, al_map_rgb(0, 0, 0), 3);

    
    for (int i = 0; i < 7; i++) {
        float dx = mouse_x - pontos_tabuleiro[i].x;
        float dy = mouse_y - pontos_tabuleiro[i].y;
        bool mouse_sobre = (dx * dx + dy * dy <= 25 * 25);

        
        if (mouse_sobre && tabuleiro[i] == VAZIO) {
            al_draw_filled_circle(pontos_tabuleiro[i].x, pontos_tabuleiro[i].y, 27, al_map_rgba(255, 255, 0, 120));
        }

        
        if (tabuleiro[i] == JOGADOR1 && peca_player1) {
            al_draw_scaled_bitmap(
                peca_player1, 0, 0,
                al_get_bitmap_width(peca_player1), al_get_bitmap_height(peca_player1),
                pontos_tabuleiro[i].x - 25, pontos_tabuleiro[i].y - 25, 50, 50, 0
            );
        }
        
        else if (tabuleiro[i] == JOGADOR2 && peca_player2) {
            al_draw_scaled_bitmap(
                peca_player2, 0, 0,
                al_get_bitmap_width(peca_player2), al_get_bitmap_height(peca_player2),
                pontos_tabuleiro[i].x - 25, pontos_tabuleiro[i].y - 25, 50, 50, 0
            );
        }
        
        else {
            al_draw_filled_circle(pontos_tabuleiro[i].x, pontos_tabuleiro[i].y, 25, al_map_rgb(200, 200, 200));
            al_draw_circle(pontos_tabuleiro[i].x, pontos_tabuleiro[i].y, 25, al_map_rgb(0, 0, 0), 3);
        }
    }

    
    al_draw_text(font, al_map_rgb(0, 120, 0), win_w / 2, 30, ALLEGRO_ALIGN_CENTER, "Tri- Angle");

    
    al_draw_filled_rounded_rectangle(0, 370, 728, 410, 12, 12, al_map_rgb(230, 230, 240));
    al_draw_rounded_rectangle(0, 370, 728, 410, 12, 12, al_map_rgb(80, 80, 80), 2);

    
    al_draw_filled_rounded_rectangle(50, 378, 180, 402, 10, 10, al_map_rgb(255, 240, 240));
    al_draw_rounded_rectangle(50, 378, 180, 402, 10, 10, al_map_rgb(200, 0, 0), 2);
    al_draw_text(font, al_map_rgb(200, 0, 0), 115, 376, ALLEGRO_ALIGN_CENTER, "Pausar");

    
    char placar[32];
    sprintf(placar, "J1: %d  -  %d :J2", placar_j1, placar_j2);
    al_draw_filled_rounded_rectangle(270, 378, 458, 402, 10, 10, al_map_rgb(240, 240, 220));
    al_draw_rounded_rectangle(270, 378, 458, 402, 10, 10, al_map_rgb(120, 120, 120), 2);
    al_draw_text(font, al_map_rgb(60, 60, 60), 364, 378, ALLEGRO_ALIGN_CENTER, placar);

    
    al_draw_filled_rounded_rectangle(478, 378, 598, 402, 10, 10, al_map_rgb(240, 255, 240));
    al_draw_rounded_rectangle(478, 378, 598, 402, 10, 10, al_map_rgb(0, 150, 0), 2);
    al_draw_text(font, al_map_rgb(0, 120, 0), 538, 376, ALLEGRO_ALIGN_CENTER, "Empate");

    
    al_draw_filled_rounded_rectangle(618, 378, 708, 402, 10, 10, al_map_rgb(240, 240, 255));
    al_draw_rounded_rectangle(618, 378, 708, 402, 10, 10, al_map_rgb(0, 0, 200), 2);
    al_draw_text(font, al_map_rgb(0, 0, 200), 663, 376, ALLEGRO_ALIGN_CENTER, "Sair");

    al_flip_display();
}

bool processar_clique_botao(
    float mx, float my, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    ALLEGRO_BITMAP *background, EstadoEspaço tabuleiro[7], int jogador, int *placar_j1, int *placar_j2,
    RegistroPartida *historico, int *num_partidas, bool *showing_menu, time_t inicio
) {
    
    if (mx >= 618 && mx <= 708 && my >= 378 && my <= 402) {
        popup_sair(font2, win_w, win_h, background, font, showing_menu);
        if (*showing_menu) {
            
            return false;
        }
    }

    
    if (mx >= 50 && mx <= 180 && my >= 378 && my <= 402) {
        pausar_jogo(font2, win_w, win_h, tabuleiro, jogador, *placar_j1, *placar_j2, inicio);
        return true;
    }

    
    if (mx >= 478 && mx <= 598 && my >= 378 && my <= 402) {
        if (empate_espera(font, win_w, win_h, jogador, historico, num_partidas, showing_menu)) {
            
            return false;
        }
    }
    return true;
}



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

bool movimento_valido(int origem, int destino, EstadoEspaço tabuleiro[7]) {
    int adjacentes[7][4] = {
        {1, 2, 3, -1},
        {0, 2, 4, -1},
        {0, 1, 3, 5},
        {0, 2, 6, -1},
        {1, 5, -1, -1},
        {2, 4, 6, -1},
        {3, 5, -1, -1}
    };
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
    for (int i = 0; i < 4; i++) {
        if (adjacentes[origem][i] == -1) break;
        if (adjacentes[origem][i] == destino) {
            if (tabuleiro[destino] == VAZIO) {
                return true;
            }
        }
    }
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
    int linhas[7][3] = {
        {0, 1, 4},
        {0, 2, 5},
        {0, 3, 6},
        {1, 2, 3},
        {4, 5, 6},
        {1, 5, 2},
        {3, 5, 2}
    };
    for (int i = 0; i < 7; i++) {
        int a = linhas[i][0];
        int b = linhas[i][1];
        int c = linhas[i][2];
        if (tabuleiro[a] != VAZIO && tabuleiro[a] == tabuleiro[b] && tabuleiro[a] == tabuleiro[c]) {
            return tabuleiro[a];
        }
    }
    return VAZIO;
}

bool empate_espera(ALLEGRO_FONT *font, int win_w, int win_h, int jogador_atual, RegistroPartida *historico, int *num_partidas, bool *showing_menu) {
    al_draw_filled_rectangle(0, 0, win_w, win_h, al_map_rgba(0, 0, 0, 180));
    al_draw_filled_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(240, 240, 255));
    al_draw_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(0, 0, 0), 3);
    char msg[64];
    sprintf(msg, "Jogador %d pediu empate", jogador_atual);
    al_draw_text(font, al_map_rgb(200, 120, 0), 364, 175, ALLEGRO_ALIGN_CENTER, msg);
    al_draw_filled_rounded_rectangle(264, 215, 464, 255, 10, 10, al_map_rgb(240, 255, 240));
    al_draw_rounded_rectangle(264, 215, 464, 255, 10, 10, al_map_rgb(0, 150, 0), 2);
    al_draw_text(font, al_map_rgb(0, 120, 0), 364, 225, ALLEGRO_ALIGN_CENTER, "Confirmar Empate");
    al_flip_display();
    ALLEGRO_EVENT_QUEUE *popup_queue = al_create_event_queue();
    al_register_event_source(popup_queue, al_get_mouse_event_source());
    ALLEGRO_EVENT ev;
    while (1) {
        al_wait_for_event(popup_queue, &ev);
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            float mx = ev.mouse.x;
            float my = ev.mouse.y;
            if (mx >= 264 && mx <= 464 && my >= 215 && my <= 255) {
                printf("Empate confirmado. Retornando ao menu principal.\n");
                strcpy(historico[*num_partidas].modo_jogo, "PvP");
                strcpy(historico[*num_partidas].resultado, "Empate");
                historico[*num_partidas].duracao = 0;
                (*num_partidas)++;
                salvar_historico("historico.txt", historico, *num_partidas);
                *showing_menu = true;
                al_destroy_event_queue(popup_queue);
                return true;
            }
        }
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }
    }
    al_destroy_event_queue(popup_queue);
    return false;
}

void pausar_jogo(ALLEGRO_FONT *font2, int win_w, int win_h, EstadoEspaço tabuleiro[7], int jogador, int pecas_jogador1, int pecas_jogador2, time_t inicio) {
    al_draw_filled_rectangle(0, 0, win_w, win_h, al_map_rgba(0, 0, 0, 180));
    al_draw_filled_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(240, 240, 255));
    al_draw_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(0, 0, 0), 3);
    al_draw_text(font2, al_map_rgb(255, 255, 255), 364, 155, ALLEGRO_ALIGN_CENTER, "Jogo Pausado");
    al_draw_filled_rounded_rectangle(264, 215, 464, 255, 10, 10, al_map_rgb(240, 240, 240));
    al_draw_rounded_rectangle(264, 215, 464, 255, 10, 10, al_map_rgb(0, 0, 0), 2);
    al_draw_text(font2, al_map_rgb(0, 0, 0), 364, 225, ALLEGRO_ALIGN_CENTER, "Retornar");
    al_flip_display();
    ALLEGRO_EVENT_QUEUE *popup_queue = al_create_event_queue();
    al_register_event_source(popup_queue, al_get_mouse_event_source());
    ALLEGRO_EVENT ev;
    while (1) {
        al_wait_for_event(popup_queue, &ev);
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            float mx = ev.mouse.x;
            float my = ev.mouse.y;
            if (mx >= 264 && mx <= 464 && my >= 215 && my <= 255) {
                printf("Retomando o Jogo\n");
                break;
            }
        }
    }
    al_destroy_event_queue(popup_queue);
}

void popup_sair(ALLEGRO_FONT *font2, int win_w, int win_h, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, bool *showing_menu) {
    al_draw_filled_rectangle(0, 0, win_w, win_h, al_map_rgba(0, 0, 0, 180));

    al_draw_filled_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(240, 240, 255));
    al_draw_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(0, 0, 0), 3);

    al_draw_text(font2, al_map_rgb(255, 0, 0), 364, 155, ALLEGRO_ALIGN_CENTER, "Deseja sair do jogo?");

    al_draw_filled_rounded_rectangle(264, 215, 364, 255, 10, 10, al_map_rgb(240, 240, 240));
    al_draw_rounded_rectangle(264, 215, 364, 255, 10, 10, al_map_rgb(0, 0, 0), 2);
    al_draw_text(font2, al_map_rgb(0, 0, 0), 314, 225, ALLEGRO_ALIGN_CENTER, "Sim");

    al_draw_filled_rounded_rectangle(364, 215, 464, 255, 10, 10, al_map_rgb(240, 240, 240));
    al_draw_rounded_rectangle(364, 215, 464, 255, 10, 10, al_map_rgb(0, 0, 0), 2);
    al_draw_text(font2, al_map_rgb(0, 0, 0), 414, 225, ALLEGRO_ALIGN_CENTER, "Não");

    al_flip_display();

    ALLEGRO_EVENT_QUEUE *popup_queue = al_create_event_queue();
    al_register_event_source(popup_queue, al_get_mouse_event_source());
    ALLEGRO_EVENT ev;
    while (1) {
        al_wait_for_event(popup_queue, &ev);
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            float mx = ev.mouse.x;
            float my = ev.mouse.y;

            if (mx >= 264 && mx <= 364 && my >= 215 && my <= 255) {
                printf("Retornando ao menu principal.\n");
                *showing_menu = true;
                desenha_menu1(background, font);
                al_destroy_event_queue(popup_queue);
                return;
            }

            if (mx >= 364 && mx <= 464 && my >= 215 && my <= 255) {
                printf("Cancelando saída.\n");
                al_destroy_event_queue(popup_queue);
                return;
            }
        }
    }
    al_destroy_event_queue(popup_queue);
}

bool exibir_vitoria(ALLEGRO_FONT *font, int win_w, int win_h, int vencedor, EstadoEspaço tabuleiro[7]) {
    al_draw_filled_rectangle(0, 0, win_w, win_h, al_map_rgba(0, 0, 0, 180));

    al_draw_filled_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(240, 240, 255));
    al_draw_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(0, 0, 0), 3);

    char msg[64];
    sprintf(msg, "Vencedor: Jogador %d", vencedor);
    al_draw_text(font, al_map_rgb(0, 120, 0), 364, 175, ALLEGRO_ALIGN_CENTER, msg);

    al_draw_filled_rounded_rectangle(264, 215, 464, 255, 10, 10, al_map_rgb(240, 240, 240));
    al_draw_rounded_rectangle(264, 215, 464, 255, 10, 10, al_map_rgb(0, 0, 0), 2);
    al_draw_text(font, al_map_rgb(0, 0, 0), 364, 225, ALLEGRO_ALIGN_CENTER, "Jogar Novamente");

    al_flip_display();

    ALLEGRO_EVENT_QUEUE *popup_queue = al_create_event_queue();
    al_register_event_source(popup_queue, al_get_mouse_event_source());
    ALLEGRO_EVENT ev;

    while (1) {
        al_wait_for_event(popup_queue, &ev);
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            float mx = ev.mouse.x;
            float my = ev.mouse.y;

            if (mx >= 264 && mx <= 464 && my >= 215 && my <= 255) {
                printf("Reiniciando Partida.\n");
                al_destroy_event_queue(popup_queue);
                return true;
            }
        }
    }

    al_destroy_event_queue(popup_queue);
    return false;
}

int ia_escolher_jogada(EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7], int jogador_ia) {
    int oponente = (jogador_ia == JOGADOR1) ? JOGADOR2 : JOGADOR1;

    
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

    
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == VAZIO) {
            tabuleiro[i] = jogador_ia;
            bool cria_ameaca = false;
            bool oponente_venceria = false;

            
            if (i == 2 || i == 5 || i == 0) {
                cria_ameaca = true;
            }

            tabuleiro[i] = VAZIO;
            if (cria_ameaca && !oponente_venceria) {
                return i;
            }
        }
    }

    
    int posicoes_estrategicas[] = {2, 0, 5, 1, 3};
    for (int i = 0; i < 5; i++) {
        int pos = posicoes_estrategicas[i];
        if (tabuleiro[pos] == VAZIO) {
            return pos;
        }
    }

    
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == VAZIO) {
            return i;
        }
    }

    return -1;
}

bool ia_escolher_movimento(EstadoEspaço tabuleiro[7], int jogador_ia, int *origem, int *destino) {
    int oponente = (jogador_ia == JOGADOR1) ? JOGADOR2 : JOGADOR1;

    
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == jogador_ia) {
            for (int j = 0; j < 7; j++) {
                if (movimento_valido(i, j, tabuleiro)) {
                    tabuleiro[j] = jogador_ia;
                    tabuleiro[i] = VAZIO;
                    if (verificar_vitoria(tabuleiro) == jogador_ia) {
                        tabuleiro[i] = jogador_ia;
                        tabuleiro[j] = VAZIO;
                        *origem = i;
                        *destino = j;
                        return true;
                    }
                    tabuleiro[i] = jogador_ia;
                    tabuleiro[j] = VAZIO;
                }
            }
        }
    }

    
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == oponente) {
            for (int j = 0; j < 7; j++) {
                if (movimento_valido(i, j, tabuleiro)) {
                    tabuleiro[j] = oponente;
                    tabuleiro[i] = VAZIO;
                    if (verificar_vitoria(tabuleiro) == oponente) {
                        tabuleiro[i] = oponente;
                        tabuleiro[j] = VAZIO;
                        
                        for (int ia_origem = 0; ia_origem < 7; ia_origem++) {
                            if (tabuleiro[ia_origem] == jogador_ia && movimento_valido(ia_origem, j, tabuleiro)) {
                                *origem = ia_origem;
                                *destino = j;
                                return true;
                            }
                        }
                    }
                    tabuleiro[i] = oponente;
                    tabuleiro[j] = VAZIO;
                }
            }
        }
    }

    
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == jogador_ia) {
            int posicoes_estrategicas[] = {2, 0, 5, 1, 3, 4, 6};
            for (int k = 0; k < 7; k++) {
                int j = posicoes_estrategicas[k];
                if (movimento_valido(i, j, tabuleiro)) {
                    *origem = i;
                    *destino = j;
                    return true;
                }
            }
        }
    }

    
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

bool posicionamento(
    ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2, int *placar_j1, int *placar_j2,
    RegistroPartida *historico, int *num_partidas,
    bool *jogar_novamente
) {
    time_t inicio = iniciar_timer();
    int jogador_atual = JOGADOR1;
    int pecas_jogador1 = 0, pecas_jogador2 = 0;
    bool posicionando = true;
    float mx = 0, my = 0;

    while (posicionando) {
        bool showing_menu = false;
        desenha_tabuleiroTriangulo(
            background, font, font2, win_w, win_h,
            tabuleiro, pontos_tabuleiro,
            peca_player1, peca_player2,
            mx, my, *placar_j1, *placar_j2, &showing_menu
        );
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mx = event.mouse.x;
            my = event.mouse.y;
        }

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            float mx_click = event.mouse.x;
            float my_click = event.mouse.y;

            bool jogo_continua = processar_clique_botao(
                mx, my, font, font2, win_w, win_h, background,
                tabuleiro, jogador_atual, placar_j1, placar_j2,
                historico, num_partidas, &showing_menu, time(NULL)
            );
            if (!jogo_continua) {
                return false;
            }
            if (showing_menu) {
                return false;
            }

            for (int i = 0; i < 7; i++) {
                float dx = mx_click - pontos_tabuleiro[i].x;
                float dy = my_click - pontos_tabuleiro[i].y;
                if (dx * dx + dy * dy <= 25 * 25 && tabuleiro[i] == VAZIO) {
                    if ((jogador_atual == JOGADOR1 && pecas_jogador1 < 3) ||
                        (jogador_atual == JOGADOR2 && pecas_jogador2 < 3)) {
                        tabuleiro[i] = jogador_atual;
                        if (jogador_atual == JOGADOR1)
                            pecas_jogador1++;
                        else
                            pecas_jogador2++;

                        EstadoEspaço vencedor = verificar_vitoria(tabuleiro);
                        if (vencedor != VAZIO) {
                            double duracao = calcular_duracao(inicio);
                            printf("Duração da partida: %.2f segundos\n", duracao);

                            if (vencedor == JOGADOR1)
                                (*placar_j1)++;
                            else if (vencedor == JOGADOR2)
                                (*placar_j2)++;

                            *jogar_novamente = exibir_vitoria(font, win_w, win_h, vencedor, tabuleiro);
                            return false;
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
    printf("Fase de posicionamento concluída. Iniciando movimentação.\n");
    return true;
}

bool posicionamento_pvm(
    ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2, int *placar_j1, int *placar_j2, int jogador_humano, int jogador_ia,
    RegistroPartida *historico, int *num_partidas,
    bool *jogar_novamente
) {
    time_t inicio = iniciar_timer();
    int pecas_humano = 0;
    int pecas_ia = 0;
    bool posicionando = true;
    float mx_hover = 0, my_hover = 0;
    int jogador_atual = jogador_humano;

    while (posicionando) {
        bool showing_menu_flag_local = false;

        if (jogador_atual == jogador_humano) {
            desenha_tabuleiroTriangulo(
                background, font, font2, win_w, win_h,
                tabuleiro, pontos_tabuleiro,
                peca_player1, peca_player2,
                mx_hover, my_hover, *placar_j1, *placar_j2, &showing_menu_flag_local
            );

            ALLEGRO_EVENT event;
            al_wait_for_event(queue, &event);

            if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                *jogar_novamente = false;
                return false;
            }

            if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
                mx_hover = event.mouse.x;
                my_hover = event.mouse.y;
                continue;
            }

            if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                float mx_click = event.mouse.x;
                float my_click = event.mouse.y;

                if (!processar_clique_botao(
                        mx_click, my_click, font, font2, win_w, win_h, background, tabuleiro, jogador_atual,
                        placar_j1, placar_j2, historico, num_partidas, &showing_menu_flag_local, time(NULL))) {
                    return false;
                }

                for (int i = 0; i < 7; i++) {
                    float dx = mx_click - pontos_tabuleiro[i].x;
                    float dy = my_click - pontos_tabuleiro[i].y;
                    if (dx * dx + dy * dy <= 25 * 25 && tabuleiro[i] == VAZIO) {
                        tabuleiro[i] = jogador_humano;
                        pecas_humano++;

                        EstadoEspaço vencedor = verificar_vitoria(tabuleiro);
                        if (vencedor != VAZIO) {
                            double duracao = calcular_duracao(inicio);
                            printf("Duração da partida: %.2f segundos\n", duracao);

                            if (vencedor == JOGADOR1)
                                (*placar_j1)++;
                            else
                                (*placar_j2)++;

                            *jogar_novamente = exibir_vitoria(font, win_w, win_h, vencedor, tabuleiro);
                            return false;
                        }

                        jogador_atual = jogador_ia;
                        break;
                    }
                }
            }
        } else if (jogador_atual == jogador_ia) {
            printf("IA está posicionando sua peça...\n");
            desenha_tabuleiroTriangulo(
                background, font, font2, win_w, win_h,
                tabuleiro, pontos_tabuleiro,
                peca_player1, peca_player2,
                -1, -1, *placar_j1, *placar_j2, &showing_menu_flag_local
            );
            al_rest(0.5);

            int posicao = ia_escolher_jogada(tabuleiro, pontos_tabuleiro, jogador_ia);
            if (posicao != -1) {
                tabuleiro[posicao] = jogador_ia;
                pecas_ia++;

                EstadoEspaço vencedor = verificar_vitoria(tabuleiro);
                if (vencedor != VAZIO) {
                    double duracao = calcular_duracao(inicio);
                    printf("Duração da partida: %.2f segundos\n", duracao);

                    if (vencedor == JOGADOR1)
                        (*placar_j1)++;
                    else
                        (*placar_j2)++;

                    *jogar_novamente = exibir_vitoria(font, win_w, win_h, vencedor, tabuleiro);
                    return false;
                }
            }
            jogador_atual = jogador_humano;
        }

        if (pecas_humano == 3 && pecas_ia == 3) {
            posicionando = false;
        }
    }

    printf("Fase de posicionamento concluída. Iniciando movimentação.\n");
    return true;
}

void movimentacao(
    ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2,
    int *placar_j1, int *placar_j2,
    RegistroPartida *historico, int *num_partidas,
    bool *jogar_novamente
) {
    bool empate_pedido[2] = {false, false};
    int jogador = JOGADOR1;
    int selecionada = -1;
    float mouse_x = 0, mouse_y = 0;

    while (1) {
        bool showing_menu = false;
        desenha_tabuleiroTriangulo(
            background, font, font2, win_w, win_h,
            tabuleiro, pontos_tabuleiro,
            peca_player1, peca_player2,
            mouse_x, mouse_y, *placar_j1, *placar_j2, &showing_menu
        );
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mouse_x = ev.mouse.x;
            mouse_y = ev.mouse.y;
        }

        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            float mx = ev.mouse.x;
            float my = ev.mouse.y;

            bool jogo_continua = processar_clique_botao(
                mx, my, font, font2, win_w, win_h, background,
                tabuleiro, jogador, placar_j1, placar_j2,
                historico, num_partidas, &showing_menu, time(NULL)
            );

            if (!jogo_continua) {
                return;
            }

            if (showing_menu) {
                return;
            }

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
                                if (vencedor == JOGADOR1)
                                    (*placar_j1)++;
                                else if (vencedor == JOGADOR2)
                                    (*placar_j2)++;
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
    RegistroPartida *historico, int *num_partidas,
    bool *jogar_novamente
) {
    int jogador_atual = jogador_humano;
    int selecionada = -1;
    float mouse_x = 0, mouse_y = 0;
    bool showing_menu = false;

    while (true) {
        if (jogador_atual == jogador_humano) {
            desenha_tabuleiroTriangulo(
                background, font, font2, win_w, win_h,
                tabuleiro, pontos_tabuleiro,
                peca_player1, peca_player2,
                mouse_x, mouse_y, *placar_j1, *placar_j2, &showing_menu
            );

            ALLEGRO_EVENT ev;
            al_wait_for_event(queue, &ev);

            if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                *jogar_novamente = false;
                return;
            }

            if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
                mouse_x = ev.mouse.x;
                continue;
            }

            if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                float mx = ev.mouse.x;
                float my = ev.mouse.y;

                bool jogo_continua = processar_clique_botao(
                    mx, my, font, font2, win_w, win_h, background,
                    tabuleiro, jogador_atual, placar_j1, placar_j2,
                    historico, num_partidas, &showing_menu, time(NULL)
                );

                if (!jogo_continua) {
                    return;
                }

                if (showing_menu) {
                    return;
                }

                if (selecionada == -1) {
                    for (int i = 0; i < 7; i++) {
                        float dx = mx - pontos_tabuleiro[i].x;
                        float dy = my - pontos_tabuleiro[i].y;
                        if (dx * dx + dy * dy <= 25 * 25 && tabuleiro[i] == jogador_atual) {
                            selecionada = i;
                            break;
                        }
                    }
                } else {
                    for (int i = 0; i < 7; i++) {
                        float dx = mx - pontos_tabuleiro[i].x;
                        float dy = my - pontos_tabuleiro[i].y;
                        if (dx * dx + dy * dy <= 25 * 25 && tabuleiro[i] == VAZIO && movimento_valido(selecionada, i, tabuleiro)) {
                            tabuleiro[i] = jogador_atual;
                            tabuleiro[selecionada] = VAZIO;

                            EstadoEspaço vencedor = verificar_vitoria(tabuleiro);
                            if (vencedor != VAZIO) {
                                desenha_tabuleiroTriangulo(
                                    background, font, font2, win_w, win_h,
                                    tabuleiro, pontos_tabuleiro,
                                    peca_player1, peca_player2,
                                    mx, my, *placar_j1, *placar_j2, &showing_menu
                                );
                                if (vencedor == JOGADOR1)
                                    (*placar_j1)++;
                                else
                                    (*placar_j2)++;
                                *jogar_novamente = exibir_vitoria(font, win_w, win_h, vencedor, tabuleiro);
                                return;
                            }

                            selecionada = -1;
                            jogador_atual = jogador_ia;
                            break;
                        }
                    }
                    selecionada = -1;
                }
            }
        } else if (jogador_atual == jogador_ia) {
            printf("IA está pensando...\n");
            desenha_tabuleiroTriangulo(
                background, font, font2, win_w, win_h,
                tabuleiro, pontos_tabuleiro,
                peca_player1, peca_player2,
                mouse_x, mouse_y, *placar_j1, *placar_j2, &showing_menu
            );
            al_rest(1.0);

            int origem, destino;
            if (ia_escolher_movimento(tabuleiro, jogador_ia, &origem, &destino)) {
                tabuleiro[destino] = jogador_ia;
                tabuleiro[origem] = VAZIO;
                printf("IA moveu de %d para %d.\n", origem, destino);

                EstadoEspaço vencedor = verificar_vitoria(tabuleiro);
                if (vencedor != VAZIO) {
                    desenha_tabuleiroTriangulo(
                        background, font, font2, win_w, win_h,
                        tabuleiro, pontos_tabuleiro,
                        peca_player1, peca_player2,
                        mouse_x, mouse_y, *placar_j1, *placar_j2, &showing_menu
                    );
                    if (vencedor == JOGADOR1)
                        (*placar_j1)++;
                    else
                        (*placar_j2)++;
                    *jogar_novamente = exibir_vitoria(font, win_w, win_h, vencedor, tabuleiro);
                    return;
                }

                jogador_atual = jogador_humano;
            } else {
                printf("ERRO: IA não encontrou um movimento válido.\n");
                *jogar_novamente = false;
                return;
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
    return time(NULL);
}

double calcular_duracao(time_t inicio) {
    time_t agora = time(NULL);
    return difftime(agora, inicio);
}


bool executar_partida_pvp(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, 
                         int win_w, int win_h, EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7], 
                         ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2, int *placar_j1, int *placar_j2, 
                         RegistroPartida *historico, int *num_partidas) {
    bool decisao_jogar_novamente = false;

    if (posicionamento(queue, background, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, 
                       peca_player1, peca_player2, placar_j1, placar_j2, historico, num_partidas, &decisao_jogar_novamente)) {
        movimentacao(queue, background, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, 
                     peca_player1, peca_player2, placar_j1, placar_j2, historico, num_partidas, &decisao_jogar_novamente);
    }

    return decisao_jogar_novamente;
}

bool executar_partida_pvm(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, 
                         int win_w, int win_h, EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7], 
                         ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2, int *placar_j1, int *placar_j2, 
                         RegistroPartida *historico, int *num_partidas) {
    bool decisao_jogar_novamente = false;
    int jogador_humano = JOGADOR1;
    int jogador_ia = JOGADOR2;

    if (posicionamento_pvm(queue, background, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, 
                           peca_player1, peca_player2, placar_j1, placar_j2, jogador_humano, jogador_ia, historico, num_partidas, &decisao_jogar_novamente)) {
        movimentacao_pvm(queue, background, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, 
                         peca_player1, peca_player2, placar_j1, placar_j2, jogador_humano, jogador_ia, historico, num_partidas, &decisao_jogar_novamente);
    }

    return decisao_jogar_novamente;
}

int main(void) {
    RegistroPartida historico[100];
    int num_partidas = 0;
    int placar_j1 = 0, placar_j2 = 0;
    EstadoEspaço tabuleiro[7];
    PontoTabuleiro pontos_tabuleiro[7] = {
        {364, 80},   
        {214, 180},  
        {364, 180},  
        {514, 180},  
        {114, 330},  
        {364, 330},  
        {614, 330}   
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

            if (!showing_play && !showing_help && !showing_history) {

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

                else if (mouse_x >= ajuda_x - ajuda_w / 2 && mouse_x <= ajuda_x + ajuda_w / 2 &&
                         mouse_y >= ajuda_y && mouse_y <= ajuda_y + ajuda_h) {
                    help_image = al_load_bitmap("Content/help.png");
                    if (help_image) {
                        desenha_ajuda3(background, help_image, font, win_w, win_h);
                        showing_menu = false;
                        showing_help = true;
                    }
                }

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

                else if (mouse_x >= sair_x - sair_h / 2 && mouse_x <= sair_x + sair_w / 2 &&
                         mouse_y >= sair_y && mouse_y <= sair_y + sair_h) {
                    running = false;
                }
            }

            else if (showing_play) {
                int pvp_x = 150, pvp_y = 310, pvp_w = 250, pvp_h = al_get_font_line_height(font2);
                int pvm_x = 585, pvm_y = 310, pvm_w = 250, pvm_h = al_get_font_line_height(font2);
                int rmenu_x = 370, rmenu_y = 300, rmenu_w = 200, rmenu_h = 40;

                if (mouse_x >= pvp_x - pvp_w / 2 && mouse_x <= pvp_x + pvp_w / 2 && mouse_y >= pvp_y && mouse_y <= pvp_y + pvp_h) {
                    bool continuar_jogando;
                    do {
                        for (int i = 0; i < 7; i++) {
                            tabuleiro[i] = VAZIO;
                        }

                        Chess_background_image = al_load_bitmap("Content/boardbackground.jpeg");
                        if (!Chess_background_image) Chess_background_image = background;

                        continuar_jogando = executar_partida_pvp(queue, Chess_background_image, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, peca_player1, peca_player2, &placar_j1, &placar_j2, historico, &num_partidas);

                        if (Chess_background_image != background) al_destroy_bitmap(Chess_background_image);

                    } while (continuar_jogando);

                    desenha_menu1(background, font);
                    showing_play = false;
                    showing_menu = true;
                }

                else if (mouse_x >= pvm_x - pvm_w / 2 && mouse_x <= pvm_x + pvm_w / 2 && mouse_y >= pvm_y && mouse_y <= pvm_y + pvm_h) {
                    bool continuar_jogando;
                    do {
                        for (int i = 0; i < 7; i++) {
                            tabuleiro[i] = VAZIO;
                        }
                        Chess_background_image = al_load_bitmap("Content/boardbackground.jpeg");
                        if (!Chess_background_image) Chess_background_image = background;

                        continuar_jogando = executar_partida_pvm(queue, Chess_background_image, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, peca_player1, peca_player2, &placar_j1, &placar_j2, historico, &num_partidas);

                        if (Chess_background_image != background) al_destroy_bitmap(Chess_background_image);

                    } while (continuar_jogando);

                    desenha_menu1(background, font);
                    showing_play = false;
                    showing_menu = true;
                }

                else if (mouse_x >= rmenu_x - rmenu_w / 2 && mouse_x <= rmenu_x + rmenu_w / 2 && mouse_y >= rmenu_y && mouse_y <= rmenu_y + rmenu_h) {
                    printf("Retornando ao menu principal\n");
                    desenha_menu1(background, font);
                    if (play_image) al_destroy_bitmap(play_image);
                    play_image = NULL;
                    showing_play = false;
                    showing_menu = true;
                }
            }
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
            else if (showing_history) {
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
